/* rk_pen - posture edit navigator of a kinematic chain */

#include <pthread.h>
#include <roki-gl/rkgl_glut.h>
#include <roki-gl/roki-gl.h>
#include <roki/rk_ik.h>

#define RK_PEN_TITLE "RK-PEN"

enum{
  OPT_MODELFILE=0, OPT_ENVFILE, OPT_INITFILE,
  OPT_PAN, OPT_TILT, OPT_ROLL,
  OPT_OX, OPT_OY, OPT_OZ,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_WIREFRAME, OPT_BB, OPT_BONE, OPT_ELLIPS,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_FOG, OPT_SHADOW,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.zkc file>", "kinematic chain model file", NULL, false },
  { "env", NULL, "<.z3d file>", "environment model file", NULL, false },
  { "init", NULL, "<.zkci file>", "initial state file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "width", NULL, "<value>", "window width", (char *)"500", false },
  { "height", NULL, "<value>", "window height", (char *)"500", false },
  { "wireframe", NULL, NULL, "draw kinematic chain as wireframe model", NULL, false },
  { "bb", NULL, NULL, "draw kinematic chain bounding box", NULL, false },
  { "bone", NULL, "<value>", "draw kinematic chain as bone model with specified radius", (char *)"0.006", false },
  { "ellips", NULL, "<value>", "draw kinematic chain as inertial ellipsoid model with specified magnitude", (char *)"1.0", false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#505050", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "smooth", NULL, NULL, "enable antialias", NULL, false },
  { "fog", NULL, NULL, "enable fog", NULL, false },
  { "shadow", NULL, NULL, "enable shadow", NULL, false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

rkChain chain;
rkglChain gr;
int env;

rkglCamera cam;
rkglLight light;
rkglShadow shadow;

/**********************************************************/

void rk_penUsage(void)
{
  eprintf( "Usage: rk_pen <options>\n" );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  exit( 0 );
}

rkLink *rk_penLink(void)
{
  int id;

  printf( "enter link ID> " );
  if( scanf( "%d", &id ) == 0 ) return NULL;
  return id >= 0 ? rkChainLink( &chain, id ) : NULL;
}

void rk_penPos(double *x, double *y, double *z)
{
  printf( "enter position vector> " );
  *x = zFDouble( stdin );
  *y = zFDouble( stdin );
  *z = zFDouble( stdin );
  printf( "*** entered position ***\n" );
  printf( "(%.10g %.10g %.10g)\n", *x, *y, *z );
}

void rk_penZYX(double *v1, double *v2, double *v3)
{
  printf( "enter z-y-x Eulerian angles> " );
  *v1 = zDeg2Rad( zFDouble( stdin ) );
  *v2 = zDeg2Rad( zFDouble( stdin ) );
  *v3 = zDeg2Rad( zFDouble( stdin ) );
  printf( "*** entered z-y-x Eulerian angles ***\n" );
  printf( "(%.10g %.10g %.10g)\n", *v1, *v2, *v3 );
}

void rk_penShowLinkList(void)
{
  int i;

  for( i=0; i<rkChainLinkNum(&chain); i++ )
    printf( "[%2d] %s\n", i, zName(rkChainLink(&chain,i)) );
}

void rk_penShowConnect(void)
{
  rkChainConnectionPrint( &chain );
}

void rk_penShowJointDis(void)
{
  register int i, j;
  double dis[6];

  printf( "*** joint displacements ***\n" );
  for( i=0; i<rkChainLinkNum(&chain); i++ ){
    printf( "[link:%d:%s] \t", i, rkChainLinkName(&chain,i) );
    rkChainLinkGetJointDis( &chain, i, dis );
    for( j=0; j<rkChainLinkJointSize(&chain,i); j++ )
      printf( "%.6g ", dis[j] );
    printf( "\n" );
  }
}

void rk_penShowLinkFrame(void)
{
  rkLink *l;

  if( !( l = rk_penLink() ) ) return;
  printf( "[link:%s]\n", zName(l) );
  printf( "with respect to the parent frame\n" );
  zFrame3DPrint( rkLinkAdjFrame(l) );
  printf( "with respect to the world frame\n" );
  zFrame3DPrint( rkLinkWldFrame(l) );
}

void rk_penShowLinkMass(void)
{
  rkLink *l;

  if( !( l = rk_penLink() ) ) return;
  printf( "[link:%s]\n", zName(l) );
  printf( "(mass) = %f\n", rkLinkMass(l) );
  printf( "(inertia) =\n" );
  printf( "with respect to the adjacent frame\n" );
  printf( "TODO\n" );
}

void rk_penSetJointDis(void)
{
  int i;
  double dis[6];
  rkLink *l;

  if( !( l = rk_penLink() ) ) return;
  printf( "change joint displacement of link %s\n", zName( l ) );
  printf( "enter values [1-%d]> ", rkLinkJointSize(l) );
  for( i=0; i<rkLinkJointSize(l); i++ )
    dis[i] = zFDouble( stdin );
  if( rkLinkJoint(l)->com == &rk_joint_revol ||
      rkLinkJoint(l)->com == &rk_joint_cylin ){
    dis[0] = zDeg2Rad( dis[0] );
  } else
  if( rkLinkJoint(l)->com == &rk_joint_hooke ){
    dis[0] = zDeg2Rad( dis[0] );
    dis[1] = zDeg2Rad( dis[1] );
  }
  rkLinkSetJointDis( l, dis );
  rkChainUpdateFK( &chain );
}

void rk_penResetJointDis(void)
{
  int i;

  for( i=0; i<rkChainLinkNum(&chain); i++ )
    rkJointNeutral( rkChainLinkJoint( &chain, i ) );
  rkChainUpdateFK( &chain );
}

void rk_penSetLinkPos(void)
{
  rkIK ik;
  rkIKCellAttr attr;
  rkIKCell *cell;
  rkLink *l, *lp;
  double p[3];
  zVec dis;

  if( !( l = rk_penLink() ) ) return;
  printf( "[link:%s] frame\n", zName(l) );
  zVec3DPrint( rkLinkWldPos(l) );
  rk_penPos( &p[0], &p[1], &p[2] );
  dis = zVecAlloc( rkChainJointSize(&chain) );

  rkIKCreate( &ik, &chain );
  for( lp=l; lp!=rkChainRoot(&chain); lp=rkLinkParent(lp) )
    if( rkLinkJointSize(lp) > 0 ){
      printf( "register joint [%s].\n", zName(lp) );
      rkIKJointReg( &ik, lp - rkChainRoot(&chain), 0.001 );
    }
  attr.id = l - rkChainRoot(&chain);
  printf( "IK of link [%s].\n", rkChainLinkName(&chain,attr.id) );
  zVec3DZero( &attr.ap );
  cell = rkIKCellRegWldPos( &ik, &attr, RK_IK_CELL_ATTR_ID | RK_IK_CELL_ATTR_AP );
  rkIKDeactivate( &ik );
  rkIKBind( &ik );
  rkIKCellSetRef( cell, p[0], p[1], p[2] );

  rkIKSolve( &ik, dis, zTOL, 0 );
  rkChainFK( &chain, dis );
  rkIKDestroy( &ik );
  zVecFree( dis );
  zFrame3DPrint( rkLinkWldFrame(l) );
}

void rk_penSetLinkFrame(void)
{
  rkIK ik;
  rkIKCellAttr attr;
  rkIKCell *cell_pos, *cell_att;
  rkLink *l, *lp;
  double p[3], a[3];
  zVec dis;

  if( !( l = rk_penLink() ) ) return;
  printf( "[link:%s] frame\n", zName(l) );
  zFrame3DPrint( rkLinkWldFrame(l) );
  rk_penPos( &p[0], &p[1], &p[2] );
  rk_penZYX( &a[0], &a[1], &a[2] );
  dis = zVecAlloc( rkChainJointSize(&chain) );

  rkIKCreate( &ik, &chain );
  for( lp=l; lp!=rkChainRoot(&chain); lp=rkLinkParent(lp) )
    if( rkLinkJointSize(lp) > 0 ){
      printf( "register joint [%s].\n", zName(lp) );
      rkIKJointReg( &ik, lp - rkChainRoot(&chain), 0.001 );
    }
  attr.id = l - rkChainRoot(&chain);
  printf( "IK of link [%s].\n", rkChainLinkName(&chain,attr.id) );
  zVec3DZero( &attr.ap );
  cell_pos = rkIKCellRegWldPos( &ik, &attr, RK_IK_CELL_ATTR_ID | RK_IK_CELL_ATTR_AP );
  cell_att = rkIKCellRegWldAtt( &ik, &attr, RK_IK_CELL_ATTR_ID );
  rkIKDeactivate( &ik );
  rkIKBind( &ik );
  rkIKCellSetRef( cell_pos, p[0], p[1], p[2] );
  rkIKCellSetRef( cell_att, a[0], a[1], a[2] );

  rkIKSolve( &ik, dis, zTOL, 0 );
  rkChainFK( &chain, dis );
  rkIKDestroy( &ik );
  zVecFree( dis );
  zFrame3DPrint( rkLinkWldFrame(l) );
}

void rk_penSetRootFrame(void)
{
  double p[3], a[3];

  printf( "root frame\n" );
  zFrame3DPrint( rkChainLinkOrgFrame(&chain,0) );
  rk_penPos( &p[0], &p[1], &p[2] );
  rk_penZYX( &a[0], &a[1], &a[2] );
  zFrame3DFromZYX( rkChainLinkOrgFrame(&chain,0),
    p[0], p[1], p[2], a[0], a[1], a[2] );
  rkChainUpdateFK( &chain );
}

void rk_penShowChainMass(void)
{
  printf( "%.10f\n", rkChainMass(&chain) );
}

void rk_penCalcLinkCOM(void)
{
  rkLink *l;
  double mass;
  zVec3D com;

  mass = 0;
  zVec3DZero( &com );
  while( ( l = rk_penLink() ) ){
    printf( "link %s asserted.\n", zName(l) );
    printf( "mass=%f, COM=", rkLinkMass(l) );
    zVec3DPrint( rkLinkWldCOM(l) );
    mass += rkLinkMass(l);
    zVec3DCatDRC( &com, rkLinkMass(l), rkLinkWldCOM(l) );
  }
  if( !zIsTiny(mass) )
    zVec3DDivDRC( &com, mass );

  printf( "grouped mass and COM with respect to the world frame\n" );
  printf( "mass=%.10g, COM=", mass );
  zVec3DPrint( &com );
}

void rk_penCalcChainCOM(void)
{
  printf( "chain COM with respect to the world frame\n" );
  zVec3DPrint( rkChainWldCOM( &chain ) );
}

void rk_penExportInit(void)
{
  char filename[BUFSIZ];

  printf( "enter file name: " );
  if( !fgets( filename, BUFSIZ, stdin ) ) return;
  zCutNL( filename );
  zAddSuffix( filename, ZEDA_ZTK_SUFFIX, filename, BUFSIZ );
  rkChainInitWriteZTK( &chain, filename );
}

void rk_penCapture(void)
{
#define SUFSIZ 10
  static int count = 0; /* for capture count */
  char xwdcommand[BUFSIZ];
  char xwdfile[BUFSIZ], imgfile[BUFSIZ];
  char imgsuf[SUFSIZ];

  printf( "enter a suffix for image format: " );
  if( scanf( "%s", imgsuf ) == 0 ) return;
  sprintf( xwdfile, "%d.xwd", count );
  sprintf( imgfile, "%d.%s", count, imgsuf );
  sprintf( xwdcommand, "xwd -name %s -nobdrs -out %s",
    RK_PEN_TITLE, xwdfile );
  if( system( xwdcommand ) == -1 ) return;
  sprintf( xwdcommand, "convert %s %s", xwdfile, imgfile );
  if( system( xwdcommand ) == -1 ) return;
  count++;
  printf( "output image file \'%s\'\n", imgfile );
}

void rk_penQuit(void)
{
  exit( 0 );
}

/**********************************************************/
/* OpenGL operation */

void scene(void)
{
  glCallList( env );
  rkglChainDraw( &gr );
}

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  scene();
  glutSwapBuffers();
}

void display_shadow(void)
{
  rkglShadowDraw( &shadow, &cam, &light, scene );
  glutSwapBuffers();
}

void rk_penInit(void)
{
  zRGB rgb;
  rkglChainAttr attr;
  zMShape3D envshape;

  zRGBDec( &rgb, opt[OPT_BG].arg );
  rkglBGSet( &cam, rgb.r, rgb.g, rgb.b );
  rkglVPCreate( &cam, 0, 0,
    atoi( opt[OPT_WIDTH].arg ), atoi( opt[OPT_HEIGHT].arg ) );
  rkglCASet( &cam,
    atof( opt[OPT_OX].arg ),
    atof( opt[OPT_OY].arg ),
    atof( opt[OPT_OZ].arg ),
    atof( opt[OPT_PAN].arg ),
    atof( opt[OPT_TILT].arg ),
    atof( opt[OPT_ROLL].arg ) );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0, 0 );
  rkglLightSetPos( &light,
    atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg) );
  rkglShadowInit( &shadow, 1024, 1024, 2, 0.2 );
  rkglTextureEnable();

  rkglChainAttrInit( &attr );
  if( opt[OPT_WIREFRAME].flag ) attr.disptype = RKGL_WIREFRAME;
  if( opt[OPT_BB].flag )        attr.disptype = RKGL_BB;
  if( opt[OPT_BONE].flag ){
    attr.disptype = RKGL_STICK;
    attr.bone_r = atof( opt[OPT_BONE].arg );
  }
  if( opt[OPT_ELLIPS].flag ){
    attr.disptype = RKGL_ELLIPS;
    attr.ellips_mag = atof( opt[OPT_ELLIPS].arg );
  }
  if( !rkChainReadZTK( &chain, opt[OPT_MODELFILE].arg ) ||
      !rkglChainLoad( &gr, &chain, &attr, &light ) )
    exit( 1 );

  if( opt[OPT_ENVFILE].flag ){
    if( !zMShape3DReadZTK( &envshape, opt[OPT_ENVFILE].arg ) ){
      ZOPENERROR( opt[OPT_ENVFILE].arg );
      rk_penUsage();
      exit( 1 );
    }
    if( attr.disptype == RKGL_STICK || attr.disptype == RKGL_ELLIPS )
      attr.disptype = RKGL_FACE;
    env = rkglMShapeEntry( &envshape, attr.disptype, &light );
    zMShape3DDestroy( &envshape );
    if( env < 0 ) exit( 1 );
  }
  if( opt[OPT_INITFILE].flag &&
      !rkChainInitReadZTK( &chain, opt[OPT_INITFILE].arg ) )
    exit( 1 );
  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
  if( opt[OPT_FOG].flag ) glEnable( GL_FOG );

  rkglSetCallbackParamGLUT( &cam, 0.2, 1, 200, 0.02, 5.0 );
  if( opt[OPT_SHADOW].flag )
    glutDisplayFunc( display_shadow );
  else
    glutDisplayFunc( display );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
}

bool rk_penCommandArgs(int argc, char *argv[])
{
  zStrAddrList arglist;
  char *modelfile;

  if( argc <= 1 ) rk_penUsage();
  zOptionRead( opt, argv, &arglist );
  zStrListGetPtr( &arglist, 1, &modelfile );
  if( opt[OPT_HELP].flag ) rk_penUsage();
  if( modelfile ){
    opt[OPT_MODELFILE].flag = true;
    opt[OPT_MODELFILE].arg  = modelfile;
  }
  rkglWindowCreateGLUT( 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg), RK_PEN_TITLE );
  rk_penInit();
  zStrAddrListDestroy( &arglist );
  return true;
}

/**********************************************************/

struct{
  char *msg;
  void (* action)(void);
} menu[] = {
  { "show link list", rk_penShowLinkList },
  { "show structure", rk_penShowConnect },
  { "show joint displacements", rk_penShowJointDis },
  { "show link frame", rk_penShowLinkFrame },
  { "show link mass property", rk_penShowLinkMass },
  { "set joint displacements", rk_penSetJointDis },
  { "set link position", rk_penSetLinkPos },
  { "set link frame", rk_penSetLinkFrame },
  { "set root frame", rk_penSetRootFrame },
  { "reset joint displacements", rk_penResetJointDis },
  { "calculate mass of kinematic chain", rk_penShowChainMass },
  { "calculate link COM", rk_penCalcLinkCOM },
  { "calculate COM of kinematic chain", rk_penCalcChainCOM },
  { "export .zri file", rk_penExportInit },
  { "capture", rk_penCapture },
  { "quit", rk_penQuit },
  { NULL, NULL },
};

void rk_penOperate(void)
{
  int i, result;
  char buf[BUFSIZ];

  while( 1 ){
    do{
      printf( "\n" );
      for( i=0; menu[i].msg; i++ )
        printf( "%d. %s\n", i, menu[i].msg );
      printf( "Select(0-%d)>", --i );
      result = atoi( zFToken( stdin, buf, BUFSIZ ) );
    } while( result < 0 || result > i );
    menu[result].action();
    glutPostRedisplay();
  }
}

int main(int argc, char *argv[])
{
  pthread_t mainloop;

  rkglInitGLUT( &argc, argv );
  rk_penCommandArgs( argc, argv+1 );
  pthread_create( &mainloop, NULL, (void *)rk_penOperate, (void *)NULL );
  glutMainLoop();
  return 0;
}
