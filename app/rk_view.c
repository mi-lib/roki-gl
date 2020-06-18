/* rk_view - 3D geometry viewer */

#include <unistd.h>
#include <roki-gl/roki-gl.h>
#include <roki-gl/rkgl_glx.h>
#include <zx11/zximage.h>

#define RK_VIEW_TITLE "RK-VIEW"

enum{
  OPT_MODELFILE=0,
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_WIREFRAME,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_FOG, OPT_SHADOW,
  OPT_CAPTURE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.z3d file>", "geometric model file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "width", NULL, "<width>", "set window width", (char *)"500", false },
  { "height", NULL, "<height>", "set window height", (char *)"500", false },
  { "wireframe", NULL, NULL, "draw objects as wireframe models", NULL, false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#010101", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "smooth", NULL, NULL, "enable antialias", NULL, false },
  { "fog", NULL, NULL, "enable fog", NULL, false },
  { "shadow", NULL, NULL, "enable shadow", NULL, false },
  { "xwd", NULL, "<suf>", "output image format suffix", (char *)"png", false },
  { "help", NULL, NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

#define RK_VIEW_BUFSIZ 512

Window win;

rkglCamera cam;
rkglLight light;
rkglShadow shadow;
int model = -1;

void rk_viewUsage(void)
{
  eprintf( "Usage: rk_view <options>\n" );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  exit( 0 );
}

void rk_viewCapture(void)
{
  zxRegion reg;
  zxImage img;
  static char imgfile[RK_VIEW_BUFSIZ];
  static int count = 0;

  sprintf( imgfile, "%04d.%s", count++, opt[OPT_CAPTURE].arg );
  zxGetGeometry( win, &reg );
  zxImageAllocDefault( &img, reg.width, reg.height );
  zxImageFromPixmap( &img, win, img.width, img.height );
  zxImageWritePNGFile( &img, imgfile );
  zxImageDestroy( &img );
}

/**********************************************************/

void rk_viewDraw(void)
{
  glCallList( model );
}

void rk_viewDisplay(void)
{
  rkglWindowActivateGLX( win );
  if( opt[OPT_SHADOW].flag ){
    /* shadow-map rendering */
    rkglShadowDraw( &shadow, &cam, &light, rk_viewDraw );
  } else{
    /* non-shadowed rendering */
    rkglClear();
    rkglCALoad( &cam );
    rkglLightPut( &light );
    rk_viewDraw();
  }
  rkglWindowSwapBuffersGLX( win );
  rkglFlushGLX();
}

void rk_viewInit(void)
{
  zRGB rgb;
  zMShape3D ms;

  win = rkglWindowCreateGLX( NULL, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg), RK_VIEW_TITLE );
  rkglWindowKeyEnableGLX( win );
  rkglWindowMouseEnableGLX( win );
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
  rkglShadowInit( &shadow, 512, 512, 1.5, 0.2 );
  rkglTextureEnable();

  if( !zMShape3DReadZTK( &ms, opt[OPT_MODELFILE].arg ) ){
    ZOPENERROR( opt[OPT_MODELFILE].arg );
    rk_viewUsage();
    exit( 1 );
  }
  model = rkglMShapeEntry( &ms,
    opt[OPT_WIREFRAME].flag ? RKGL_WIREFRAME : RKGL_FACE, &light );
  zMShape3DDestroy( &ms );
  if( model < 0 ) exit( 1 );

  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
  if( opt[OPT_FOG].flag ) rkglBGFog( &cam, 0.1 );
}

bool rk_viewCommandArgs(int argc, char *argv[])
{
  zStrAddrList arglist;
  char *modelfile;

  if( argc <= 1 ) rk_viewUsage();
  zOptionRead( opt, argv, &arglist );
  zStrListGetPtr( &arglist, 1, &modelfile );
  if( opt[OPT_HELP].flag ) rk_viewUsage();
  if( modelfile ){
    opt[OPT_MODELFILE].flag = true;
    opt[OPT_MODELFILE].arg  = modelfile;
  }
  if( !opt[OPT_MODELFILE].flag ){
    ZRUNERROR( "model not assigned" );
    return false;
  }
  rk_viewInit();
  zStrAddrListDestroy( &arglist );
  return true;
}

void rk_viewExit(void)
{
  rkglWindowCloseGLX( win );
  rkglExitGLX();
}

/**********************************************************/
void rk_viewReshape(void)
{
  zxRegion reg;
  double x, y;

  zxGetGeometry( win, &reg );
  x = 0.1;
  rkglVPCreate( &cam, 0, 0, reg.width, reg.height );
  y = x / rkglVPAspect(&cam);
  rkglFrustum( &cam, -x, x, -y, y, 1, 20 );
}

int rk_viewKeyPress(void)
{
  static bool from_light = false;

  zxModkeyOn( zxKeySymbol() );
  switch( zxKeySymbol() ){
  case XK_l: /* toggle viewpoint to light/camera */
    if( ( from_light = 1 - from_light ) ){
      rkglCALookAt( &cam,
        atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg),
        0, 0, 0, -1, 0, 1 );
    } else{
      rkglCASet( &cam,
        atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
        atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );
    }
    break;
  case XK_c:
    rk_viewCapture();
    break;
  case XK_q:
    eprintf( "quit.\n" );
    return -1;
  }
  return 0;
}

int rk_viewEvent(void)
{
  int event;

  switch( ( event = zxDequeueEvent() ) ){
  case Expose:
  case ConfigureNotify: rk_viewReshape();             break;
  case ButtonPress:
  case ButtonRelease:   rkglMouseFuncGLX( &cam, event, 1.0 ); break;
  case MotionNotify:    rkglMouseDragFuncGLX( &cam ); break;
  case KeyPress:        if( rk_viewKeyPress() >= 0 )  break; return -1;
  case KeyRelease:      zxModkeyOff( zxKeySymbol() ); break;
  default: ;
  }
  return 0;
}

void rk_viewLoop(void)
{
#define RK_VIEW_LOOP_SKIP 5
  int count = 0;

  /* initial draw */
  rk_viewReshape();
  rk_viewDisplay();
  while( rk_viewEvent() == 0 ){
    if( count++ > RK_VIEW_LOOP_SKIP ){
      rk_viewDisplay();
      count = 0;
    }
  }
}

int main(int argc, char *argv[])
{
  rkglInitGLX();
  if( !rk_viewCommandArgs( argc, argv+1 ) ) return 1;
  rk_viewLoop();
  rk_viewExit();
  return 0;
}
