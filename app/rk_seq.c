/* rk_seq - afterimage of a sequence of robot motion */

#if defined(__ROKI_GL_USE_GLX)
#include <zm/zm_seq.h>
#include <zx11/zximage.h>
#include <roki_gl/roki_glx.h>

#define RK_SEQ_TITLE "RK-SEQ"

enum{
  OPT_MODELFILE=0, OPT_ZVSFILE, OPT_ZKCSFILE, OPT_ENVFILE,
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SHADOW, OPT_SHADOW_SIZE, OPT_SHADOW_AREA, OPT_SHADOW_BLUR,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.ztk/.urdf file>", "kinematic chain model file", NULL, false },
  { "zvs", NULL, "<.zvs file>", "joint displacement sequence file", NULL, false },
  { "zkcs", NULL, "<.zkcs file>", "full configuration sequence file", NULL, false },
  { "env", NULL, "<.ztk file>", "environment shape model file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"2", false },
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
  { "shadowblur", NULL, "<value>", "edge blur of shadow", (char *)"0.1", false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

static char *seqfilebase;

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
  zStrAddrList arglist;
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
  zStrAddrListDestroy( &arglist );
  return true;
}

bool rk_seqLoadSequence(void)
{
  static const char *seq_stdin = "rk_seq.input";

  if( opt[OPT_ZKCSFILE].flag ){
    if( !zSeqScanFile( &seq, opt[OPT_ZKCSFILE].arg ) )
      return false;
    if( !( seqfilebase = zStrClone( opt[OPT_ZKCSFILE].arg ) ) )
      return false;
    zGetBasenameDRC( seqfilebase );
  } else{
    if( opt[OPT_ZVSFILE].flag ){
      if( !zSeqScanFile( &seq, opt[OPT_ZVSFILE].arg ) )
        return false;
    } else{
      if( !zSeqFScan( stdin, &seq ) ) return false;
      opt[OPT_ZVSFILE].arg = (char *)seq_stdin;
    }
    if( !( seqfilebase = zStrClone( opt[OPT_ZVSFILE].arg ) ) )
      return false;
    zGetBasenameDRC( seqfilebase );
  }
  if( !( poselist = zIndexCreate( zListSize(&seq) ) ) ){
    zSeqFree( &seq );
    return false;
  }
  return true;
}

void rk_seqListEntry(void)
{
  int i=0;
  zSeqCell *cp;
  void (* kf)(rkChain*,zVec);

  kf = opt[OPT_ZKCSFILE].flag ? rkChainSetConf : rkChainFK;
  while( !zListIsEmpty( &seq ) ){
    cp = zSeqDequeue( &seq );
    zIndexSetElem( poselist, i++, rkglBeginList() );
    kf( &chain, cp->data.v );
    rkglChainDraw( &gc );
    glEndList();
    zSeqCellFree( cp );
  }
}

void rk_seqDraw(void)
{
  int i;

  if( env ) glCallList( env );
  for( i=0; i<zArraySize(poselist); i++ )
    glCallList( zIndexElem(poselist,i) );
}

void rk_seqDisplay(void)
{
  rkglWindowActivateGLX( win );
  if( opt[OPT_SHADOW].flag ){
    /* shadow-map rendering */
    rkglShadowDraw( &shadow, &cam, &light, rk_seqDraw );
  } else{
    /* non-shadowed rendering */
    rkglClear();
    rkglCameraPut( &cam );
    rkglLightPut( &light );
    rk_seqDraw();
  }
  rkglWindowSwapBuffersGLX( win );
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
  rkglWindowKeyEnableGLX( win );
  rkglWindowMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  zRGBDecodeStr( &rgb, opt[OPT_BG].arg );
  rkglCameraSetBackground( &cam, rgb.r, rgb.g, rgb.b );
  rkglCameraSetViewport( &cam, 0, 0,
    atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg) );
  rkglCameraSetViewframe( &cam,
    atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
    atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );
  rkglSetDefaultCamera( &cam, 30.0, 1.0, 200 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0 );
  rkglLightMove( &light, atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg) );
  rkglShadowInit( &shadow, atoi(opt[OPT_SHADOW_SIZE].arg), atoi(opt[OPT_SHADOW_SIZE].arg), atof(opt[OPT_SHADOW_AREA].arg), 0.2, atof(opt[OPT_SHADOW_BLUR].arg) );
  rkglTextureEnable();

  rkglChainAttrInit( &attr );
  if( !rkChainReadFile( &chain, opt[OPT_MODELFILE].arg ) ||
      !rkglChainLoad( &gc, &chain, &attr, &light ) ){
    ZOPENERROR( opt[OPT_MODELFILE].arg );
    rk_seqUsage();
    exit( 1 );
  }
  if( opt[OPT_ENVFILE].flag ){
    if( !zMShape3DReadZTK( &envshape, opt[OPT_ENVFILE].arg ) ){
      ZOPENERROR( opt[OPT_ENVFILE].arg );
      rk_seqUsage();
      exit( 1 );
    }
    env = rkglEntryMShape( &envshape, attr.disptype, &light );
    zMShape3DDestroy( &envshape );
    if( env < 0 ) exit( 1 );
  }

  rk_seqLoadSequence();
  rk_seqListEntry();
}

void rk_seqExit(void)
{
  int i;

  free( seqfilebase );
  for( i=0; i<zArraySize(poselist); i++ )
    glDeleteLists( zIndexElem(poselist,i), 1 );
  zIndexFree( poselist );
  rkglChainUnload( &gc );
  rkChainDestroy( &chain );
  rkglWindowCloseGLX( win );
  rkglExitGLX();
}

void rk_seqCapture(void)
{
#define SEQ_BUFSIZ 512
  zxRegion reg;
  zxImage img;
  static char imgfile[SEQ_BUFSIZ];

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

  zxGetGeometry( win, &reg );
  rkglCameraSetViewport( &cam, 0, 0, reg.width, reg.height );
  rkglDefaultCameraSetPerspective();
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
  int event;

  switch( ( event = zxDequeueEvent() ) ){
  case Expose:
  case ConfigureNotify: rk_seqReshape();              break;
  case ButtonPress:
  case ButtonRelease:   rkglMouseFuncGLX( &cam, event ); break;
  case MotionNotify:    rkglMouseDragFuncGLX( &cam ); break;
  case KeyPress:        if( rk_seqKeyPress() >= 0 )   break; return -1;
  case KeyRelease:      zxModkeyOff( zxKeySymbol() ); break;
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
#else
#include <roki_gl/roki_gl.h>

int main(int argc, char *argv[])
{
  ROKI_GL_LIB_MANDATORY( "GLX", argv[0] );
  return 0;
}
#endif /* __ROKI_GL_USE_GLX */
