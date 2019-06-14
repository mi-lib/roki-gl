/* rk_seq - afterimage of a sequence of robot motion */

#include <zm/zm_seq.h>
#include <roki-gl/roki-gl.h>
#include <roki-gl/rkgl_glx.h>
#include <zx11/zximage_png.h>

#define RK_SEQ_TITLE "RK-SEQ"

enum{
  OPT_MODELFILE=0, OPT_ZVSFILE, OPT_ZKCSFILE, OPT_ENVFILE,
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SHADOW, OPT_SHADOW_SIZE, OPT_SHADOW_AREA,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.zkc file>", "kinematic chain model file", NULL, false },
  { "zvs", NULL, "<.zvs file>", "joint displacement sequence file", NULL, false },
  { "zkcs", NULL, "<.zkcs file>", "full configuration sequence file", NULL, false },
  { "env", NULL, "<.z3d file>", "environment model file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "width", NULL, "<width>", "set window width", (char *)"500", false },
  { "height", NULL, "<height>", "set window height", (char *)"500", false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#505050", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "shadow", NULL, NULL, "enable shadow", NULL, false },
  { "shadowsize", NULL, "<value>", "shadow map size", (char *)"1024", false },
  { "shadowarea", NULL, "<value>", "radius of shadowing area", (char *)"2.0", false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

#define RK_SEQ_BUFSIZ 512

static char seqfilebase[RK_SEQ_BUFSIZ];

static rkChain chain;
static rkglChain gc;
static zSeq seq;
static int env = 0;
static zIndex poselist;
static Window win;

static rkglCamera cam;
static rkglLight light;
static rkglShadow shadow;

void rk_seqUsage(void)
{
  eprintf( "Usage: rk_seq <options>\n" );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  exit( 0 );
}

bool rk_seqCommandArgs(int argc, char *argv[])
{
  zStrList arglist;
  char *modelfile, *seqfile;

  if( argc <= 1 ) rk_seqUsage();
  zOptionRead( opt, argv, &arglist );
  zStrListGetPtr( &arglist, 2, &modelfile, &seqfile );
  if( opt[OPT_HELP].flag ) rk_seqUsage();
  if( modelfile ){
    opt[OPT_MODELFILE].flag = true;
    opt[OPT_MODELFILE].arg  = modelfile;
  }
  if( !opt[OPT_MODELFILE].flag ){
    ZRUNERROR( "kinematic chain model not assigned" );
    return false;
  }
  if( seqfile ){
    opt[OPT_ZVSFILE].flag = true;
    opt[OPT_ZVSFILE].arg  = seqfile;
  }
  zStrListDestroy( &arglist, false );
  return true;
}

bool rk_seqLoadSequence(void)
{
  static const char *seq_stdin = "rk_seq.input";

  if( opt[OPT_ZKCSFILE].flag ){
    if( !zSeqScanFile( &seq, opt[OPT_ZKCSFILE].arg ) )
      return false;
    zGetBasename( opt[OPT_ZKCSFILE].arg, seqfilebase, BUFSIZ );
  } else{
    if( opt[OPT_ZVSFILE].flag ){
      if( !zSeqScanFile( &seq, opt[OPT_ZVSFILE].arg ) )
        return false;
    } else{
      if( !zSeqFScan( stdin, &seq ) ) return false;
      opt[OPT_ZVSFILE].arg = (char *)seq_stdin;
    }
    zGetBasename( opt[OPT_ZVSFILE].arg, seqfilebase, BUFSIZ );
  }
  if( !( poselist = zIndexCreate( zListNum(&seq) ) ) ){
    zSeqFree( &seq );
    return false;
  }
  return true;
}

void rk_seqListEntry(void)
{
  register int i=0;
  zSeqListCell *cp;
  void (* kf)(rkChain*,zVec);

  kf = opt[OPT_ZKCSFILE].flag ? rkChainSetConf : rkChainFK;
  while( !zListIsEmpty( &seq ) ){
    cp = zSeqDequeue( &seq );
    zIndexSetElem( poselist, i++, rkglBeginList() );
    kf( &chain, cp->data.v );
    rkglChainDraw( &gc );
    glEndList();
    zSeqListCellFree( cp );
  }
}

void rk_seqDraw(void)
{
  register int i;

  if( env ) glCallList( env );
  for( i=0; i<zArraySize(poselist); i++ )
    glCallList( zIndexElem(poselist,i) );
}

void rk_seqDisplay(void)
{
  rkglActivateGLX( win );
  if( opt[OPT_SHADOW].flag ){
    /* shadow-map rendering */
    rkglShadowDraw( &shadow, &cam, &light, rk_seqDraw );
  } else{
    /* non-shadowed rendering */
    rkglClear();
    rkglCALoad( &cam );
    rkglLightPut( &light );
    rk_seqDraw();
  }
  rkglSwapBuffersGLX( win );
  rkglFlushGLX();
}

void rk_seqRedisplay(void)
{
#define RK_SEQ_DISPLAY_SKIP 15
  static int count = 0;

  if( count++ == RK_SEQ_DISPLAY_SKIP ){
    count = 0;
    rk_seqDisplay();
  }
}

void rk_seqInit(void)
{
  zRGB rgb;
  rkglChainAttr attr;
  zMShape3D envshape;

  win = rkglWindowCreateGLX( NULL, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg), RK_SEQ_TITLE );
  rkglKeyEnableGLX( win );
  rkglMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

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

  rkglChainAttrInit( &attr );
  if( !rkChainScanFile( &chain, opt[OPT_MODELFILE].arg ) ||
      !rkglChainLoad( &gc, &chain, &attr ) ){
    ZOPENERROR( opt[OPT_MODELFILE].arg );
    rk_seqUsage();
    exit( 1 );
  }
  if( opt[OPT_ENVFILE].flag ){
    if( !zMShape3DScanFile( &envshape, opt[OPT_ENVFILE].arg ) ){
      ZOPENERROR( opt[OPT_ENVFILE].arg );
      rk_seqUsage();
      exit( 1 );
    }
    env = rkglMShapeEntry( &envshape, attr.disptype );
    zMShape3DDestroy( &envshape );
    if( env < 0 ) exit( 1 );
  }

  rk_seqLoadSequence();
  rk_seqListEntry();
}

void rk_seqExit(void)
{
  register int i;

  for( i=0; i<zArraySize(poselist); i++ )
    glDeleteLists( zIndexElem(poselist,i), 1 );
  zIndexFree( poselist );
  rkglChainUnload( &gc );
  rkChainDestroy( &chain );
  rkglWindowCloseGLX( win );
  rkglCloseGLX();
}

void rk_seqCapture(void)
{
  zxRegion reg;
  zxImage img;
  static char imgfile[RK_SEQ_BUFSIZ];

  sprintf( imgfile, "%s_seq.png",  seqfilebase );
  zxGetGeometry( win, &reg );
  zxImageAllocDefault( &img, reg.width, reg.height );
  zxImageFromPixmap( &img, win, img.width, img.height );
  zxImageWritePNGFile( &img, imgfile );
  zxImageDestroy( &img );
}

/**********************************************************/
void rk_seqReshape(void)
{
  zxRegion reg;
  double x, y;

  zxGetGeometry( win, &reg );
  rkglVPCreate( &cam, 0, 0, reg.width, reg.height );
  x = 0.1;
  y = x / rkglVPAspect(&cam);
  rkglFrustum( &cam, -x, x, -y, y, 1, 20 );
}

enum{ RK_SEQ_CAM_ROT, RK_SEQ_CAM_PAN, RK_SEQ_CAM_ZOOM };

static int mouse_button = -1;
static int mousex, mousey;
static byte cammode = RK_SEQ_CAM_ROT;

void rk_seqStoreMouseInfo(byte mode)
{
  mouse_button = zxMouseButton;
  mousex = zxMouseX;
  mousey = zxMouseY;
  cammode = mode;
}

void rk_seqMousePress(void)
{
  switch( zxMouseButton ){
  case Button1: rk_seqStoreMouseInfo( RK_SEQ_CAM_ROT );  break;
  case Button3: rk_seqStoreMouseInfo( RK_SEQ_CAM_PAN );  break;
  case Button2: rk_seqStoreMouseInfo( RK_SEQ_CAM_ZOOM ); break;
  case Button4:
    rkglCARelMove( &cam,-0.1, 0, 0 ); rk_seqRedisplay(); break;
  case Button5:
    rkglCARelMove( &cam, 0.1, 0, 0 ); rk_seqRedisplay(); break;
  default: ;
  }
}

void rk_seqMouseRelease(void)
{
  mouse_button = -1;
}

void rk_seqMouseDrag(void)
{
  double dx, dy, r;

  if( mouse_button == -1 ) return;
  dx = (double)( zxMouseX - mousex ) / cam.vp[3];
  dy =-(double)( zxMouseY - mousey ) / cam.vp[2];
  switch( cammode ){
  case RK_SEQ_CAM_ROT:
    r = 180 * sqrt( dx*dx + dy*dy );
    zxModkeyCtrlIsOn() ?
      rkglCARotate( &cam, r, -dy, dx, 0 ) :
      rkglCALockonRotate( &cam, r, -dy, dx, 0 );
    rk_seqRedisplay();
    break;
  case RK_SEQ_CAM_PAN:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, 0, dx, dy ) :
      rkglCARelMove( &cam, 0, dx, dy );
    rk_seqRedisplay();
    break;
  case RK_SEQ_CAM_ZOOM:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, -dy, 0, 0 ) :
      rkglCARelMove( &cam, -2*dy, 0, 0 );
    rk_seqRedisplay();
    break;
  default: ;
  }
  mousex = zxMouseX;
  mousey = zxMouseY;
}

int rk_seqKeyPress(void)
{
  zxModkeyOn( zxKeySymbol() );
  switch( zxKeySymbol() ){
  case XK_c: rk_seqCapture(); break;
  case XK_q: return -1; /* quit */
  }
  return 0;
}

int rk_seqEvent(void)
{
  switch( zxDequeueEvent() ){
  case Expose:
  case ConfigureNotify: rk_seqReshape();            break;
  case ButtonPress:     rk_seqMousePress();         break;
  case MotionNotify:    rk_seqMouseDrag();          break;
  case ButtonRelease:   rk_seqMouseRelease();       break;
  case KeyPress:        if( rk_seqKeyPress() >= 0 ) break; return -1;
  case KeyRelease: zxModkeyOff( zxKeySymbol() );    break;
  default: ;
  }
  return 0;
}

void rk_seqAfterimage(void)
{
  rk_seqReshape();
  rk_seqDisplay();
  while( rk_seqEvent() >= 0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLX();
  if( !rk_seqCommandArgs( argc, argv+1 ) ) return 1;
  rk_seqInit();
  rk_seqAfterimage();
  rk_seqExit();
  return 0;
}
