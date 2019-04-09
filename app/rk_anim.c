/* rk_anim - robot animation viewer */

#include <roki-gl/roki-gl.h>
#include <roki-gl/rkgl_glx.h>
#include <zx11/zximage_png.h>
#include <liw/liw_paction.h>

#define RK_ANIM_TITLE "RK-ANIM"

enum{
  OPT_TITLE=0,
  OPT_ENVFILE,
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ,
  OPT_WINX, OPT_WINY,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_WIREFRAME, OPT_BB, OPT_BONE, OPT_COM, OPT_ELLIPS,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_FOG,
  OPT_SHADOW, OPT_SHADOW_SIZE, OPT_SHADOW_AREA,
  OPT_FRAME, OPT_CAPTURE, OPT_SECPERFRAME, OPT_SKEW,
  OPT_RESIZE,
  OPT_NOTIMESTAMP,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "title", NULL, "<title name>", "title of sequence", (char *)"robot animation", false },
  { "env", NULL, "<.ztk file>", "environment model file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "winx", NULL, "<winx>", "set window init x position", (char *)"0", false },
  { "winy", NULL, "<winy>", "set window init y position", (char *)"0", false },
  { "width", NULL, "<width>", "set window width", (char *)"500", false },
  { "height", NULL, "<height>", "set window height", (char *)"500", false },
  { "wireframe", NULL, NULL, "draw kinematic chain as wireframe model", NULL, false },
  { "bb", NULL, NULL, "draw kinematic chain bounding box", NULL, false },
  { "bone", NULL, "<value>", "draw kinematic chain as bone model with specified radius", (char *)"0.006", false },
  { "com", NULL, NULL, "draw COM of each link and the whole", (char *)"0.012", false },
  { "ellips", NULL, NULL, "draw kinematic chain as inertial ellipsoid model", (char *)"0.1", false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#505050", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "smooth", NULL, NULL, "enable antialias", NULL, false },
  { "fog", NULL, NULL, "enable fog", NULL, false },
  { "shadow", NULL, NULL, "enable shadow", NULL, false },
  { "shadowsize", NULL, "<value>", "shadow map size", (char *)"1024", false },
  { "shadowarea", NULL, "<value>", "radius of shadowing area", (char *)"2.0", false },
  { "k", NULL, NULL, "wait key-in at each step", NULL, false },
  { "capture", NULL, "<suf>", "output image format suffix", (char *)"png", false },
  { "secperframe", NULL, "<period [msec]>", "second per frame", NULL, false },
  { "skew", NULL, "<multiplier>", "set time skew multiplier", (char *)"1.0", false },
  { "resize", NULL, NULL, "enable to resize within the parent window", NULL, false },
  { "notimestamp", NULL, NULL, "no timestanp for capture", NULL, false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

#define RK_ANIM_BUFSIZ 512
#define RK_ANIM_SIDE_SPACE 4
#define RK_ANIM_TOP_SPACE 4
#define RK_ANIM_BOTTOM_SPACE 36

#define RK_ANIM_SEC_PER_FRAME 0.033
static liwPAction pa;
static double sec_per_frame = RK_ANIM_SEC_PER_FRAME;
static double skew;
static double t_resume;
static bool seq_in_process = false;

static rkChain chain_env;
static rkglChain ge;
static int env = 0;

static rkglCamera cam;
static rkglLight light;
static rkglShadow shadow;
static bool from_light = false;

static zxWindow win;
static Window glwin;

static int win_width, win_height;

typedef struct{
  rkChain chain;
  rkglChain gc;
  zSeq seq;
  zSeqListCell *seq_now;
  char seqfilebase[RK_ANIM_BUFSIZ];
  void (* kf)(rkChain*,zVec);
  double t_now;
} rkAnimSet;
zListClass( rkAnimCellList, rkAnimCell, rkAnimSet );

static rkAnimCellList anim_cell_list;

/* ************************************************************************* */
/* chain */
bool rkAnimCellLoadChain(char chainfile[], rkglChainAttr *attr)
{
  rkAnimCell *cell;

  if( !( cell = zAlloc( rkAnimCell, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  if( !rkChainReadFile( &cell->data.chain, chainfile ) ||
      !rkglChainLoad( &cell->data.gc, &cell->data.chain, attr ) ){
    ZOPENERROR( chainfile );
    zFree( cell );
    return false;
  }
  zSeqInit( &cell->data.seq );
  cell->data.seq_now = NULL;
  cell->data.kf = rkChainFK;
  cell->data.t_now = 0;
  zListInsertHead( &anim_cell_list, cell );
  return true;
}

bool rkAnimCellLoadSeq(char *seqfile)
{
  static const char *seq_stdin = "rk_anim.input";
  rkAnimCell *cell;

  zListForEach( &anim_cell_list, cell )
    if( zListIsEmpty( &cell->data.seq ) ) break;
  if( cell == zListRoot(&anim_cell_list) ) return false;
  if( seqfile ){
    if( !zSeqReadFile( &cell->data.seq, seqfile ) ) return false;
    zGetBasename( seqfile, cell->data.seqfilebase, RK_ANIM_BUFSIZ );
  } else{
    if( !zSeqFRead( stdin, &cell->data.seq ) ) return false;
    strcpy( cell->data.seqfilebase, seq_stdin );
  }
  cell->data.seq_now = zListHead(&cell->data.seq);
  if( strcmp( zGetSuffix(seqfile), ZSEQ_SUFFIX ) != 0 )
    cell->data.kf = rkChainSetConf;
  cell->data.t_now = 0;
  return true;
}

void rkAnimCellDestroy(rkAnimCell *cell)
{
  rkglChainUnload( &cell->data.gc );
  rkChainDestroy( &cell->data.chain );
  zSeqFree( &cell->data.seq );
  zFree( cell );
}

void rkAnimCellListDestroy(void)
{
  rkAnimCell *cell;

  while( !zListIsEmpty( &anim_cell_list ) ){
    zListDeleteHead( &anim_cell_list, &cell );
    rkAnimCellDestroy( cell );
  }
}

/* ************************************************************************* */
void rkAnimUsage(void)
{
  eprintf( "Usage: rk_anim <.ztk file> <.zvs/.zkcs file> [options]\n" );
  eprintf( "<.ztk file>\tkinematic chain model file\n" );
  eprintf( "<.zvs file>\tjoint displacement sequence file\n" );
  eprintf( "<.zkcs file>\tfull configuration sequence file\n" );
  eprintf( "[options]\n" );
  zOptionHelp( opt );
  eprintf( "\n[operations]\n" );
  eprintf( "Space/p    pause and restart animation\n" );
  eprintf( "f          one frame forward (valid after pause)\n" );
  eprintf( "b          one frame backward (valid after pause)\n" );
  eprintf( "r          rewind\n" );
  eprintf( "c          capture the current frame\n" );
  eprintf( "q          quit\n" );
  eprintf( "left button & drag          camera rotation about focus\n" );
  eprintf( "left button & drag + CTRL   camera rotation\n" );
  eprintf( "right button & drag         camera translation about focus\n" );
  eprintf( "right button & drag + CTRL  camera translation\n" );
  eprintf( "middle button or wheel      camera zooming\n" );
  exit( 0 );
}

void rkAnimCreateChainAttr(rkglChainAttr *attr)
{
  rkglChainAttrInit( attr );
  if( opt[OPT_WIREFRAME].flag ) attr->disptype = RKGL_WIREFRAME;
  if( opt[OPT_BB].flag )        attr->disptype = RKGL_BB;
  if( opt[OPT_BONE].flag ){
    attr->disptype = RKGL_STICK;
    attr->bone_r = atof( opt[OPT_BONE].arg );
  }
  if( opt[OPT_COM].flag ){
    attr->disptype = RKGL_COM;
    attr->com_r = atof( opt[OPT_COM].arg );
  }
  if( opt[OPT_ELLIPS].flag ){
    attr->disptype = RKGL_ELLIPS;
    attr->ellips_mag = atof( opt[OPT_ELLIPS].arg );
  }
}

bool rkAnimCellListCreate(zStrList *arglist)
{
  zStrListCell *cell, *cp;
  rkglChainAttr attr;
  bool ret = true;

  zListInit( &anim_cell_list );
  rkAnimCreateChainAttr( &attr );
  zListForEach( arglist, cell ){
    if( strcmp( zGetSuffix( cell->data ), ZEDA_ZTK_SUFFIX ) == 0 ){
      cp = zListCellPrev(cell);
      zListPurge( arglist, cell );
      if( !rkAnimCellLoadChain( cell->data, &attr ) ) ret = false;
      zStrListCellFree( cell, false );
      cell = cp;
    }
  }
  if( !ret ) return false;
  if( zListNum(&anim_cell_list) == 1 && zListIsEmpty(arglist) )
    if( !rkAnimCellLoadSeq( NULL ) ) return false;
  if( zListNum(&anim_cell_list) != zListNum(arglist) ){
    ZRUNERROR( "sequence not specified" );
    return false;
  }
  zListForEach( arglist, cell )
    if( !rkAnimCellLoadSeq( cell->data ) ) return false;
  return true;
}

void rkAnimCapture(void)
{
  zxImage img;
  static char imgfile[RK_ANIM_BUFSIZ];

  sprintf( imgfile, "%s%0.3f.png", opt[OPT_TITLE].arg, t_resume );
  zxImageAllocDefault( &img, zxWindowWidth(&win), zxWindowHeight(&win) );
  zxImageFromPixmap( &img, zxCanvas(&win), img.width, img.height );
  zxImageWritePNGFile( &img, imgfile );
  zxImageDestroy( &img );
}

void rkAnimForward(int sig)
{
  rkAnimCell *cell;
  bool is_checked = false;
  double t_resume_tmp;

  seq_in_process = true;
  t_resume_tmp = t_resume + sec_per_frame;
  zListForEach( &anim_cell_list, cell ){
    if( cell->data.seq_now != zListTail(&cell->data.seq) ){
      is_checked = true;
      while( cell->data.t_now < t_resume_tmp &&
             cell->data.seq_now != zListTail(&cell->data.seq) ){
        cell->data.seq_now = zListCellPrev(cell->data.seq_now);
        cell->data.t_now += cell->data.seq_now->data.dt * skew;
      }
    }
  }
  if( is_checked )
    t_resume = t_resume_tmp;
  seq_in_process = false;
}

void rkAnimBackward(int sig)
{
  rkAnimCell *cell;
  bool is_checked = false;
  double t_resume_tmp;

  seq_in_process = true;
  t_resume_tmp = t_resume - sec_per_frame;
  zListForEach( &anim_cell_list, cell ){
    if( cell->data.seq_now != zListHead(&cell->data.seq) ){
      is_checked = true;
      while( cell->data.t_now > t_resume_tmp &&
             cell->data.seq_now != zListHead(&cell->data.seq) ){
        cell->data.seq_now = zListCellNext(cell->data.seq_now);
        cell->data.t_now -= cell->data.seq_now->data.dt * skew;
      }
    }
  }
  if( is_checked )
    t_resume = t_resume_tmp;
  seq_in_process = false;
}

void rkAnimRewind(int sig)
{
  rkAnimCell *cell;

  seq_in_process = true;
  zListForEach( &anim_cell_list, cell ){
    cell->data.t_now = 0;
    cell->data.seq_now = zListHead(&cell->data.seq);
  }
  t_resume = 0;
  seq_in_process = false;
}

/**********************************************************/

static zVec3D *_zMat3DToPTR(zMat3D *m, zVec3D *angle);
static void _rkAnimCamOptWrite(char *name, double val);
zVec3D *_zMat3DToPTR(zMat3D *m, zVec3D *angle)
{
  double azim, ca, sa;

  azim = atan2( -m->e[1][0], m->e[1][1] );
  zSinCos( azim, &sa, &ca );
  angle->e[0] = azim;
  angle->e[1] = atan2( m->e[2][0]*ca+m->e[2][1]*sa, m->e[0][0]*ca+m->e[0][1]*sa );
  angle->e[2] = atan2( m->e[1][2], -m->e[1][0]*sa+m->e[1][1]*ca );
  return angle;
}

void _rkAnimCamOptWrite(char *name, double val)
{
  printf( "-%s %s%f ", name, val<0.0?"-- ":"", val );
}

void rkAnimCamOptWrite(zVec3D *v, zVec3D *ptr)
{
  _rkAnimCamOptWrite( "x", v->e[0] );
  _rkAnimCamOptWrite( "y", v->e[1] );
  _rkAnimCamOptWrite( "z", v->e[2] );
  _rkAnimCamOptWrite( "pan",  zRad2Deg( ptr->e[0] ) );
  _rkAnimCamOptWrite( "tilt", zRad2Deg( ptr->e[1] ) );
  _rkAnimCamOptWrite( "roll", zRad2Deg( ptr->e[2] ) );
  fflush( stdout );
}

void rkAnimGetCamFrame()
{
  zMat3D m, m0;
  zVec3D v, ptr;

  rkglCALoad( &cam );
  zMat3DCreate( &m,
    cam.ca[0], cam.ca[1], cam.ca[2],
    cam.ca[4], cam.ca[5], cam.ca[6],
    cam.ca[8], cam.ca[9], cam.ca[10] );
  zMat3DCreate( &m0,
    0, 0, 1,
    1, 0, 0,
    0, 1, 0 );
  zVec3DCreate( &v, cam.ca[12], cam.ca[13], cam.ca[14] );

  /* position */
  zVec3DRevDRC( &v );
  zMulMat3DVec3DDRC( &m, &v );

  /* pan, tilt and roll angle */
  zMulMat3DMat3DTDRC( &m, &m0 );
  _zMat3DToPTR( &m, &ptr );

  rkAnimCamOptWrite( &v, &ptr );
}

/**********************************************************/

void rkAnimDraw(void)
{
  rkAnimCell *cell;

  zListForEach( &anim_cell_list, cell ){
    rkglChainDraw( &cell->data.gc );
    if( opt[OPT_COM].flag )
      rkglChainCOMDraw( &cell->data.gc, 0.03 );
  }
  if( env ) glCallList( env );
}

void rkAnimDisplay(void)
{
  rkglActivateGLX( glwin );
  if( opt[OPT_SHADOW].flag ){
    /* shadow-map rendering */
    rkglShadowDraw( &shadow, &cam, &light, rkAnimDraw );
  } else{
    /* non-shadowed rendering */
    rkglClear();
    rkglCALoad( &cam );
    rkglLightPut( &light );
    rkAnimDraw();
  }
  rkglSwapBuffersGLX( glwin );
  rkglFlushGLX();
}

void rkAnimRedisplay(void)
{
#define RK_ANIM_DISPLAY_SKIP 5
  static int count = 0;

  if( count++ == RK_ANIM_DISPLAY_SKIP ){
    count = 0;
    rkAnimDisplay();
  }
}

void rkAnimLoadEnv(void)
{
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
  if( opt[OPT_WIREFRAME].flag ) attr.disptype = RKGL_WIREFRAME;
  if( opt[OPT_BB].flag )        attr.disptype = RKGL_BB;

  if( !rkChainMShape3DReadFile( &chain_env, opt[OPT_ENVFILE].arg ) ){
    ZOPENERROR( opt[OPT_ENVFILE].arg );
    rkAnimUsage();
    exit( 1 );
  }
  if( !rkglChainLoad( &ge, &chain_env, &attr ) ) exit( 1 );

  env = rkglBeginList();
  rkglChainDraw( &ge );
  glEndList();
  if( env < 0 ) exit( 1 );
}

void rkAnimInit(void)
{
  zRGB rgb;
  short width, height;
  int winx, winy;

  winx = atoi( opt[OPT_WINX].arg );
  winy = atoi( opt[OPT_WINY].arg );
  width  = atoi( opt[OPT_WIDTH].arg );
  height = atoi( opt[OPT_HEIGHT].arg );
  if( opt[OPT_NOTIMESTAMP].flag ){
    zxWindowCreate( &win, winx, winy, width, height );
    win_width = width;
    win_height = height;
  } else {
    zxWindowCreate( &win, winx, winy, width+2*RK_ANIM_SIDE_SPACE, height+RK_ANIM_TOP_SPACE+RK_ANIM_BOTTOM_SPACE );
    win_width = width+2*RK_ANIM_SIDE_SPACE;
    win_height = height+RK_ANIM_TOP_SPACE+RK_ANIM_BOTTOM_SPACE;
  }
  zxWindowSetTitle( &win, RK_ANIM_TITLE );
  zxWindowOpen( &win );
  zxWindowSetBG( &win, (char *)"lightgray" );
  zxWindowSetFG( &win, (char *)"black" );
  zxWindowClear( &win );
  zxSetFont( &win, "-misc-fixed-medium-r-normal-*-40-*-*-*-*-*-*-*" );

  if( opt[OPT_NOTIMESTAMP].flag )
    glwin = rkglWindowCreateGLX( &win, 0, 0, width, height, NULL );
  else
    glwin = rkglWindowCreateGLX( &win, RK_ANIM_SIDE_SPACE, RK_ANIM_TOP_SPACE, width, height, NULL );
  rkglKeyEnableGLX( glwin );
  rkglMouseEnableGLX( glwin );
  rkglWindowOpenGLX( glwin );

  zRGBDec( &rgb, opt[OPT_BG].arg );
  rkglBGSet( &cam, rgb.r, rgb.g, rgb.b );
  rkglVPCreate( &cam, 0, 0,
    atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg) );
  rkglCASet( &cam,
    atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
    atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0, 0 );
  rkglLightSetPos( &light,
    atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg) );
  rkglShadowInit( &shadow, atoi(opt[OPT_SHADOW_SIZE].arg), atoi(opt[OPT_SHADOW_SIZE].arg), atof(opt[OPT_SHADOW_AREA].arg), 0.2 );

  if( opt[OPT_ENVFILE].flag ) rkAnimLoadEnv();
  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
  if( opt[OPT_FOG].flag ) rkglBGFog( &cam, 0.1 );
}

bool rkAnimCommandArgs(int argc, char *argv[])
{
  zStrList arglist;

  if( argc <= 1 ) rkAnimUsage();
  zOptionRead( opt, argv, &arglist );
  if( opt[OPT_HELP].flag ) rkAnimUsage();
  if( opt[OPT_SECPERFRAME].flag )
    sec_per_frame = atof( opt[OPT_SECPERFRAME].arg );
  skew = 1.0 / atof( opt[OPT_SKEW].arg );
  if( zListIsEmpty(&arglist) ){
    ZRUNERROR( "kinematic chain model not specified" );
    return false;
  }
  rkAnimInit();
  if( !rkAnimCellListCreate( &arglist ) ) return false;
  zStrListDestroy( &arglist, false );
  return true;
}

void rkAnimExit(void)
{
  rkAnimCellListDestroy();
  if( env ) rkChainDestroy( &chain_env );
  rkglWindowCloseGLX( glwin );
  rkglCloseGLX();
  zxWindowDestroy( &win );
}

/**********************************************************/
void rkAnimReshape(void)
{
  zxRegion reg;

  if( opt[OPT_RESIZE].flag ){
    zxWindowUpdateRegion( &win );
    if( zxWindowWidth(&win) != win_width || zxWindowHeight(&win) != win_height ){
      if( opt[OPT_NOTIMESTAMP].flag )
        rkglWindowResizeGLX( glwin, zxWindowWidth(&win), zxWindowHeight(&win) );
      else
        rkglWindowResizeGLX( glwin, zxWindowWidth(&win)-2*RK_ANIM_SIDE_SPACE, zxWindowHeight(&win)-RK_ANIM_TOP_SPACE-RK_ANIM_BOTTOM_SPACE );
    }
  }
  zxGetGeometry( glwin, &reg );
  rkglVPCreate( &cam, 0, 0, reg.width, reg.height );
  if( from_light ){
    double d;
    rkglVVInit();
    d = sqrt( zSqr(light.pos[0])+zSqr(light.pos[1])+zSqr(light.pos[2]) );
    gluPerspective( 2*zRad2Deg(asin(shadow.radius/d)),
      (GLdouble)shadow.width/(GLdouble)shadow.height,
      d > shadow.radius ? d-shadow.radius : d*0.9, d+shadow.radius );
  } else{
    double x, y;
    x = 0.1;
    y = x / rkglVPAspect(&cam);
    /* rkglFrustum( &cam, -x, x, -y, y, 1, 20 ); */
    rkglFrustum( &cam, -x, x, -y, y, 1, 200 );
  }
}

enum{ RK_ANIM_CAM_ROT, RK_ANIM_CAM_PAN, RK_ANIM_CAM_ZOOM };

static int mouse_button = -1;
static int mousex, mousey;
static byte cammode = RK_ANIM_CAM_ROT;

void rkAnimStoreMouseInfo(byte mode)
{
  mouse_button = zxMouseButton;
  mousex = zxMouseX;
  mousey = zxMouseY;
  cammode = mode;
}

void rkAnimMousePress(void)
{
  switch( zxMouseButton ){
  case Button1: rkAnimStoreMouseInfo( RK_ANIM_CAM_ROT );  break;
  case Button3: rkAnimStoreMouseInfo( RK_ANIM_CAM_PAN );  break;
  case Button2: rkAnimStoreMouseInfo( RK_ANIM_CAM_ZOOM ); break;
  case Button4:
    rkglCARelMove( &cam,-0.1, 0, 0 ); rkAnimDisplay();    break;
  case Button5:
    rkglCARelMove( &cam, 0.1, 0, 0 ); rkAnimDisplay();    break;
  default: ;
  }
}

void rkAnimMouseRelease(void)
{
  mouse_button = -1;
}

void rkAnimMouseDrag(void)
{
  double dx, dy, r;

  if( mouse_button == -1 ) return;
  dx = (double)( zxMouseX - mousex ) / cam.vp[3];
  dy =-(double)( zxMouseY - mousey ) / cam.vp[2];
  switch( cammode ){
  case RK_ANIM_CAM_ROT:
    r = 180 * sqrt( dx*dx + dy*dy );
    zxModkeyCtrlIsOn() ?
      rkglCARotate( &cam, r, -dy, dx, 0 ) :
      rkglCALockonRotate( &cam, r, -dy, dx, 0 );
    rkAnimRedisplay();
    break;
  case RK_ANIM_CAM_PAN:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, 0, dx, dy ) :
      rkglCARelMove( &cam, 0, dx, dy );
    rkAnimRedisplay();
    break;
  case RK_ANIM_CAM_ZOOM:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, -dy, 0, 0 ) :
      rkglCARelMove( &cam, -2*dy, 0, 0 );
    rkAnimRedisplay();
    break;
  default: ;
  }
  mousex = zxMouseX;
  mousey = zxMouseY;
}

int rkAnimKeyPress(void)
{
  zxModkeyOn( zxKeySymbol() );
  switch( zxKeySymbol() ){
  case XK_l: case XK_L: /* toggle viewpoint to light/camera */
    if( ( from_light = 1 - from_light ) ){
      rkglCALookAt( &cam,
        light.pos[0], light.pos[1], light.pos[2], 0, 0, 0, -1, 0, 1 );
    } else{
      rkglCASet( &cam,
        atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
        atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );
    }
    rkAnimReshape();
    rkAnimDisplay();
    break;
  case XK_p: case XK_P: case XK_space:
    pa.is_running ? liwPActionStop( &pa ) : liwPActionStart( &pa );
    break;
  case XK_f: case XK_F:
    rkAnimForward( 0 );
    break;
  case XK_b: case XK_B:
    rkAnimBackward( 0 );
    break;
  case XK_r: case XK_R:
    rkAnimRewind( 0 );
    break;
  case XK_c: case XK_C:
    rkAnimCapture();
    break;
  case XK_o: case XK_O:
    rkAnimGetCamFrame();
    break;
  case XK_q: case XK_Q:
    eprintf( "quit.\n" );
    return -1;
  }
  return 0;
}

int rkAnimEvent(void)
{
  switch( zxDequeueEvent() ){
  case Expose:
  case ConfigureNotify: rkAnimReshape();              break;
  case ButtonPress:     rkAnimMousePress();           break;
  case MotionNotify:    rkAnimMouseDrag();            break;
  case ButtonRelease:   rkAnimMouseRelease();         break;
  case KeyPress:        if( rkAnimKeyPress() >= 0 )   break; return -1;
  case KeyRelease:      zxModkeyOff( zxKeySymbol() ); break;
  default: ;
  }
  return 0;
}

void rkAnimDrawTimestamp(void)
{
  static char timestamp[RK_ANIM_BUFSIZ];
  zxRegion reg;

  sprintf( timestamp, "%0.3f [s]", t_resume / skew );
  zxTextArea( timestamp, 0, 0, &reg );
  zxWindowClear( &win );
  zxDrawString( &win, zxWindowWidth(&win)-reg.width-8, zxWindowHeight(&win)-8, timestamp );
}

void rkAnimFK(void)
{
  rkAnimCell *cell;

  zListForEach( &anim_cell_list, cell )
    cell->data.kf( &cell->data.chain, cell->data.seq_now->data.v );
}

bool rkAnimIsTerminated(void)
{
  rkAnimCell *cell;
  bool ret = true;

  zListForEach( &anim_cell_list, cell )
    if( cell->data.seq_now != zListTail(&cell->data.seq) )
      ret = false;
  return ret;
}

void rkAnimPlay(void)
{
  /* initial draw */
  rkAnimFK();
  rkAnimReshape();
  rkAnimDisplay();
  if( !opt[OPT_NOTIMESTAMP].flag )
    rkAnimDrawTimestamp();
  else
    zxWindowClear( &win );
  if( opt[OPT_CAPTURE].flag ){
    while( !rkAnimIsTerminated() ){
      rkAnimCapture();
      rkAnimForward( 0 );
      rkAnimFK();
      rkAnimDisplay();
      if( !opt[OPT_NOTIMESTAMP].flag )
        rkAnimDrawTimestamp();
      else
        zxWindowClear( &win );
    }
  } else{
    liwPActionCreate( &pa, sec2usec(sec_per_frame), rkAnimForward );
    if( !opt[OPT_FRAME].flag )
      liwPActionStart( &pa );
    while( 1 ){
      if( rkAnimEvent() < 0 ) return;
      if( seq_in_process ) continue;
      rkAnimFK();
      rkAnimRedisplay();
      rkAnimDrawTimestamp();
    }
  }
  eprintf( "animation terminated.\n" );
}

int main(int argc, char *argv[])
{
  rkglInitGLX();
  if( !rkAnimCommandArgs( argc, argv+1 ) ) return 1;
  rkAnimPlay();
  rkAnimExit();
  return 0;
}
