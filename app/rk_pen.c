/* rk_pen - posture edit navigator of a kinematic chain */

#if defined(__ROKI_GL_USE_GLUT)
#include <unistd.h>
#include <pthread.h>
#include <roki_gl/roki_glut.h>

#define RK_PEN_TITLE "RK-PEN"

enum{
  OPT_MODELFILE=0, OPT_ENVFILE, OPT_INITFILE,
  OPT_PAN, OPT_TILT, OPT_ROLL,
  OPT_OX, OPT_OY, OPT_OZ, OPT_AUTO,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_DRAW_WIREFRAME,
  OPT_DRAW_BB,
  OPT_DRAW_BONE,
  OPT_DRAW_COORD,
  OPT_DRAW_ELLIPS,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_FOG, OPT_SHADOW,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.ztk/.urdf file>", "kinematic chain model file", NULL, false },
  { "env", NULL, "<.ztk file>", "environment shape model file", NULL, false },
  { "init", NULL, "<.ztk file>", "initial state file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "auto", NULL, NULL, "automatic allocation of camera", NULL, false },
  { "width", NULL, "<value>", "window width", (char *)"500", false },
  { "height", NULL, "<value>", "window height", (char *)"500", false },
  { "wireframe", NULL, NULL, "draw kinematic chain as wireframe model", NULL, false },
  { "bb", NULL, NULL, "draw kinematic chain bounding box", NULL, false },
  { "bone", NULL, "<value>", "draw kinematic chain as bone model with specified radius", (char *)"0.006", false },
  { "coord", NULL, "<value>", "draw cascaded coordinate frameschained of kinematic chain with specified length of arrows of axes", (char *)"0.1", false },
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
  return id >= 0 && id < rkChainLinkNum(&chain) ? rkChainLink( &chain, id ) : NULL;
}

void rk_penPos(double *x, double *y, double *z)
{
  printf( "enter position vector> " );
  zFDouble( stdin, x );
  zFDouble( stdin, y );
  zFDouble( stdin, z );
  printf( "*** entered position ***\n" );
  printf( "(%.10g %.10g %.10g)\n", *x, *y, *z );
}

void rk_penZYX(double *v1, double *v2, double *v3)
{
  printf( "enter z-y-x Eulerian angles> " );
  zFDouble( stdin, v1 ); *v1 = zDeg2Rad( *v1 );
  zFDouble( stdin, v2 ); *v2 = zDeg2Rad( *v2 );
  zFDouble( stdin, v3 ); *v3 = zDeg2Rad( *v3 );
  printf( "*** entered z-y-x Eulerian angles ***\n" );
  printf( "(%.10g %.10g %.10g)\n", *v1, *v2, *v3 );
}

void rk_penShowLinkList(void)
{
  int i;

  for( i=0; i<rkChainLinkNum(&chain); i++ )
    printf( "[%2d] %s\n", i, zName(rkChainLink(&chain,i)) );
}

void rk_penShowConnectivity(void)
{
  rkChainConnectivityPrint( &chain );
}

void rk_penShowJointDis(void)
{
  register int i, j;
  double dis[6];

  printf( "*** joint displacements ***\n" );
  for( i=0; i<rkChainLinkNum(&chain); i++ ){
    printf( "[link:%d:%s] \t", i, rkChainLinkName(&chain,i) );
    rkChainLinkJointGetDis( &chain, i, dis );
    for( j=0; j<rkChainLinkJointDOF(&chain,i); j++ )
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

void rk_penRad2DegJointDis(rkLink *l, double dis[])
{
  if( rkLinkJoint(l)->com == &rk_joint_revol ||
      rkLinkJoint(l)->com == &rk_joint_cylin ){
    dis[0] = zRad2Deg( dis[0] );
  } else
  if( rkLinkJoint(l)->com == &rk_joint_hooke ){
    dis[0] = zRad2Deg( dis[0] );
    dis[1] = zRad2Deg( dis[1] );
  }
}

void rk_penDeg2RadJointDis(rkLink *l, double dis[])
{
  if( rkLinkJoint(l)->com == &rk_joint_revol ||
      rkLinkJoint(l)->com == &rk_joint_cylin ){
    dis[0] = zDeg2Rad( dis[0] );
  } else
  if( rkLinkJoint(l)->com == &rk_joint_hooke ){
    dis[0] = zDeg2Rad( dis[0] );
    dis[1] = zDeg2Rad( dis[1] );
  }
}

void rk_penSetJointDis(void)
{
  int i;
  double dis[6], min[6], max[6];
  rkLink *l;

  if( !( l = rk_penLink() ) ) return;
  if( rkLinkJoint(l)->com == &rk_joint_fixed ){
    printf( "This is a fixed joint.\n" );
    return;
  }
  printf( "change joint displacement of link %s\n", zName( l ) );
  rkLinkJointGetMin( l, min );
  rkLinkJointGetMax( l, max );
  rk_penRad2DegJointDis( l, min );
  rk_penRad2DegJointDis( l, max );
  for( i=0; i<rkLinkJointDOF(l); i++ ){
    printf( "enter value %d/%d [%.10g-%.10g]> ", i, rkLinkJointDOF(l), min[i], max[i] );
    zFDouble( stdin, &dis[i] );
  }
  rk_penDeg2RadJointDis( l, dis );
  rkLinkJointSetDis( l, dis );
  rkChainUpdateFK( &chain );
}

void rk_penResetJointDis(void)
{
  rkChainNeutralize( &chain );
}

void rk_penSetLinkPos(void)
{
  rkIKAttr attr;
  rkIKCell *cell;
  rkLink *l, *lp;
  double p[3];
  zVec dis;

  if( !( l = rk_penLink() ) ) return;
  printf( "[link:%s] frame\n", zName(l) );
  zVec3DPrint( rkLinkWldPos(l) );
  rk_penPos( &p[0], &p[1], &p[2] );
  dis = zVecAlloc( rkChainJointSize(&chain) );
  for( lp=l; lp!=rkChainRoot(&chain); lp=rkLinkParent(lp) )
    if( rkLinkJointDOF(lp) > 0 ){
      printf( "register joint [%s].\n", zName(lp) );
      rkChainRegisterIKJointID( &chain, lp - rkChainRoot(&chain), 0.001 );
    }
  attr.id = l - rkChainRoot(&chain);
  printf( "IK of link [%s].\n", rkChainLinkName(&chain,attr.id) );
  zVec3DZero( &attr.attention_point );
  cell = rkChainRegisterIKCellWldPos( &chain, NULL, 0, &attr, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT );
  rkIKCellSetRef( cell, p[0], p[1], p[2] );

  rkChainIK( &chain, dis, zTOL, 0 );
  rkChainFK( &chain, dis );
  rkChainDestroyIK( &chain );
  zVecFree( dis );
  zFrame3DPrint( rkLinkWldFrame(l) );
}

void rk_penSetLinkFrame(void)
{
  rkIKAttr attr;
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
  for( lp=l; lp!=rkChainRoot(&chain); lp=rkLinkParent(lp) )
    if( rkLinkJointDOF(lp) > 0 ){
      printf( "register joint [%s].\n", zName(lp) );
      rkChainRegisterIKJointID( &chain, lp - rkChainRoot(&chain), 0.001 );
    }
  attr.id = l - rkChainRoot(&chain);
  printf( "IK of link [%s].\n", rkChainLinkName(&chain,attr.id) );
  zVec3DZero( &attr.attention_point );
  cell_pos = rkChainRegisterIKCellWldPos( &chain, NULL, 0, &attr, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT );
  cell_att = rkChainRegisterIKCellWldAtt( &chain, NULL, 0, &attr, RK_IK_ATTR_MASK_ID );
  rkIKCellSetRef( cell_pos, p[0], p[1], p[2] );
  rkIKCellSetRef( cell_att, a[0], a[1], a[2] );

  rkChainIK( &chain, dis, zTOL, 0 );
  rkChainFK( &chain, dis );
  rkChainDestroyIK( &chain );
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
#define FILENAMESIZ 512
#define SUFSIZ 10
  static int count = 0; /* for capture count */
  char xwdcommand[BUFSIZ];
  char xwdfile[FILENAMESIZ], imgfile[FILENAMESIZ];
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

int rk_penChangeDir(char *pathname, char *dirname, char *filename, char *cwd, size_t size)
{
  if( !getcwd( cwd, size ) ){
    ZRUNERROR( "astray from directory" );
    return -1;
  }
  zGetDirFilename( pathname, dirname, filename, size );
  if( *dirname ){
    if( chdir( dirname ) < 0 ){
      ZRUNERROR( "cannot change directory to %s", dirname );
      return -1;
    }
  }
  return 0;
}

int rk_penReturnDir(char *cwd)
{
  if( chdir( cwd ) < 0 ){
    ZRUNERROR( "cannot change directory to %s", cwd );
    return -1;
  }
  return 0;
}

rkChain *rk_penReadChainFile(rkChain *chain, char *pathname)
{
  char dirname[BUFSIZ], filename[BUFSIZ], cwd[BUFSIZ];

  rk_penChangeDir( pathname, dirname, filename, cwd, BUFSIZ );
  chain = rkChainReadFile( chain, filename );
  rk_penReturnDir( cwd );
  return chain;
}

zMShape3D *rk_penReadMShapeFile(zMShape3D *ms, char *pathname)
{
  char dirname[BUFSIZ], filename[BUFSIZ], cwd[BUFSIZ];

  rk_penChangeDir( pathname, dirname, filename, cwd, BUFSIZ );
  ms = zMShape3DReadZTK( ms, filename );
  rk_penReturnDir( cwd );
  return ms;
}

void rk_penInit(void)
{
  zRGB rgb;
  rkglChainAttr attr;
  zMShape3D envshape;
  zSphere3D bball;
  double vv_width, vv_near, vv_far;

  rkglChainAttrInit( &attr );
  if( opt[OPT_DRAW_WIREFRAME].flag ) attr.disptype = RKGL_WIREFRAME;
  if( opt[OPT_DRAW_BB].flag )        attr.disptype = RKGL_BB;
  if( opt[OPT_DRAW_BONE].flag ){
    attr.disptype = RKGL_STICK;
    attr.bone_r = atof( opt[OPT_DRAW_BONE].arg );
  }
  if( opt[OPT_DRAW_COORD].flag ) attr.disptype = RKGL_FRAME;
  if( opt[OPT_DRAW_ELLIPS].flag ){
    attr.disptype = RKGL_ELLIPS;
    attr.ellips_mag = atof( opt[OPT_DRAW_ELLIPS].arg );
  }
  if( !rk_penReadChainFile( &chain, opt[OPT_MODELFILE].arg ) ||
      !rkglChainLoad( &gr, &chain, &attr, &light ) )
    exit( 1 );

  if( opt[OPT_ENVFILE].flag ){
    if( !rk_penReadMShapeFile( &envshape, opt[OPT_ENVFILE].arg ) ){
      ZOPENERROR( opt[OPT_ENVFILE].arg );
      rk_penUsage();
      exit( 1 );
    }
    if( attr.disptype == RKGL_STICK || attr.disptype == RKGL_ELLIPS )
      attr.disptype = RKGL_FACE;
    env = rkglEntryMShape( &envshape, attr.disptype, &light );
    zMShape3DDestroy( &envshape );
    if( env < 0 ) exit( 1 );
  }
  if( opt[OPT_INITFILE].flag &&
      !rkChainInitReadZTK( &chain, opt[OPT_INITFILE].arg ) )
    exit( 1 );

  zRGBDecodeStr( &rgb, opt[OPT_BG].arg );
  rkglBGSet( &cam, rgb.r, rgb.g, rgb.b );
  rkglVPCreate( &cam, 0, 0, atoi( opt[OPT_WIDTH].arg ), atoi( opt[OPT_HEIGHT].arg ) );
  if( opt[OPT_AUTO].flag && rkChainBoundingBall( &chain, &bball ) ){
    rkglCALookAt( &cam,
      zSphere3DCenter(&bball)->c.x+zSphere3DRadius(&bball)*18, zSphere3DCenter(&bball)->c.y, zSphere3DCenter(&bball)->c.z,
      zSphere3DCenter(&bball)->c.x, zSphere3DCenter(&bball)->c.y, zSphere3DCenter(&bball)->c.z,
      0, 0, 1 );
    vv_width = zSphere3DRadius(&bball) / 8;
    vv_near = zSphere3DRadius(&bball);
    vv_far = 1000*zSphere3DRadius(&bball);
  } else{
    if( opt[OPT_PAN].flag || opt[OPT_TILT].flag || opt[OPT_ROLL].flag )
      rkglCASet( &cam,
        atof( opt[OPT_OX].arg ), atof( opt[OPT_OY].arg ), atof( opt[OPT_OZ].arg ),
        atof( opt[OPT_PAN].arg ),  atof( opt[OPT_TILT].arg ), atof( opt[OPT_ROLL].arg ) );
    else
      rkglCALookAt( &cam,
        atof( opt[OPT_OX].arg ), atof( opt[OPT_OY].arg ), atof( opt[OPT_OZ].arg ),
        0, 0, 0, 0, 0, 1 );
    vv_width = 0.2;
    vv_near = 1;
    vv_far = 200;
  }
  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0 );
  rkglLightMove( &light, atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg) );
  rkglShadowInit( &shadow, 1024, 1024, 2, 0.2, 0.1 );
  rkglTextureEnable();
  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
  if( opt[OPT_FOG].flag ) glEnable( GL_FOG );

  rkglSetDefaultCallbackParam( &cam, vv_width, vv_near, vv_far, 0.02, 5.0 );
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
  { "show structure", rk_penShowConnectivity },
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
#else
#include <roki_gl/roki_gl.h>

int main(int argc, char *argv[])
{
  ROKI_GL_LIB_MANDATORY( "GLUT", argv[0] );
  return 0;
}
#endif /* __ROKI_GL_USE_GLUT */
