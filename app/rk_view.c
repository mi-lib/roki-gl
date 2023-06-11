/* rk_view - 3D geometry viewer */

#include <unistd.h>
#include <roki_gl/roki_gl.h>
#include <roki_gl/rkgl_glx.h>
#include <zx11/zximage.h>

#define RK_VIEW_TITLE "RK-VIEW"

enum{
  OPT_MODELFILE=0,
  OPT_PAN, OPT_TILT, OPT_ROLL, OPT_OX, OPT_OY, OPT_OZ, OPT_AUTO,
  OPT_WIDTH, OPT_HEIGHT,
  OPT_SCALE,
  OPT_WIREFRAME,
  OPT_BG,
  OPT_LX, OPT_LY, OPT_LZ,
  OPT_SMOOTH, OPT_SHADOW,
  OPT_CAPTURE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "model", NULL, "<.ztk file>", "geometric model file", NULL, false },
  { "pan", NULL, "<pan value>", "set camera pan angle", (char *)"0", false },
  { "tilt", NULL, "<tilt value>", "set camera tilt angle", (char *)"0", false },
  { "roll", NULL, "<roll value>", "set camera roll angle", (char *)"0", false },
  { "x", NULL, "<value>", "camera position in x axis", (char *)"5", false },
  { "y", NULL, "<value>", "camera position in y axis", (char *)"0", false },
  { "z", NULL, "<value>", "camera position in z axis", (char *)"0", false },
  { "auto", NULL, NULL, "automatic allocation of camera", NULL, false },
  { "width", NULL, "<width>", "set window width", (char *)"500", false },
  { "height", NULL, "<height>", "set window height", (char *)"500", false },
  { "scale", NULL, "<scale>", "set scale factor", (char *)"1.0", false },
  { "wireframe", NULL, NULL, "draw objects as wireframe models", NULL, false },
  { "bg", NULL, "<RGB#hex>", "set background color", (char *)"#010101", false },
  { "lx", NULL, "<value>", "light position in x axis", (char *)"3", false },
  { "ly", NULL, "<value>", "light position in y axis", (char *)"0", false },
  { "lz", NULL, "<value>", "light position in z axis", (char *)"3", false },
  { "smooth", NULL, NULL, "enable antialias", NULL, false },
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

/* view volume */
zSphere3D bball;
double vv_width, vv_near, vv_far;

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

void rk_viewReadPH(zMShape3D *ms, char *sfx)
{
  FILE *fp;

  zMShape3DInit( ms );
  zArrayAlloc( &ms->optic, zOpticalInfo, 1 );
  zArrayAlloc( &ms->shape, zShape3D, 1 );
  if( zMShape3DOpticNum(ms) != 1 || zMShape3DShapeNum(ms) != 1 ){
    ZALLOCERROR();
    exit( EXIT_FAILURE );
  }
  zOpticalInfoInit( zMShape3DOptic(ms,0) );
  if( strcmp( sfx, "dae" ) == 0 ){
    if( !zShape3DFReadDAE( zMShape3DShape(ms,0), opt[OPT_MODELFILE].arg ) ) exit( EXIT_FAILURE );
    return;
  }
  if( !( fp = fopen( opt[OPT_MODELFILE].arg, "rt" ) ) ){
    ZOPENERROR( opt[OPT_MODELFILE].arg );
    rk_viewUsage();
  }
  if( strcmp( sfx, "stl" ) == 0 ){
    if( !zShape3DFReadSTL( fp, zMShape3DShape(ms,0) ) ) exit( EXIT_FAILURE );
  } else
  if( strcmp( sfx, "ply" ) == 0 ){
    if( !zShape3DFReadPLY( fp, zMShape3DShape(ms,0) ) ) exit( EXIT_FAILURE );
  } else
  if( strcmp( sfx, "obj" ) == 0 ){
    if( !zShape3DFReadOBJ( fp, zMShape3DShape(ms,0) ) ) exit( EXIT_FAILURE );
  } else{
    ZRUNERROR( "unknown format %s", sfx );
    rk_viewUsage();
  }
  fclose( fp );
}

void rk_viewReadModel(void)
{
  char *sfx;
  double scale;
  zMShape3D ms;
  register int i;

  sfx = zGetSuffix( opt[OPT_MODELFILE].arg );
  if( !sfx || strcmp( sfx, "ztk" ) == 0 ){
    if( !zMShape3DReadZTK( &ms, opt[OPT_MODELFILE].arg ) ){
      ZOPENERROR( opt[OPT_MODELFILE].arg );
      rk_viewUsage();
    }
  } else
    rk_viewReadPH( &ms, sfx );

  if( opt[OPT_SCALE].flag ){
    scale = atof( opt[OPT_SCALE].arg );
    for( i=0; i<zMShape3DShapeNum(&ms); i++ )
      zPH3DScale( zShape3DPH(zMShape3DShape(&ms,i)), scale );
  }
  if( opt[OPT_AUTO].flag )
    if( !zMShape3DBBall( &ms, &bball ) ) exit( 1 );
  model = rkglMShapeEntry( &ms,
    opt[OPT_WIREFRAME].flag ? RKGL_WIREFRAME : RKGL_FACE, &light );
  zMShape3DDestroy( &ms );
  if( model < 0 ) exit( 1 );
}

void rk_viewResetCamera(void)
{
  if( opt[OPT_AUTO].flag ){
    rkglCALookAt( &cam,
      zSphere3DCenter(&bball)->c.x+zSphere3DRadius(&bball)*18, zSphere3DCenter(&bball)->c.y, zSphere3DCenter(&bball)->c.z,
      zSphere3DCenter(&bball)->c.x, zSphere3DCenter(&bball)->c.y, zSphere3DCenter(&bball)->c.z,
      0, 0, 1 );
    vv_width = zSphere3DRadius(&bball) / 8;
    vv_near = zSphere3DRadius(&bball);
    vv_far = 1000*zSphere3DRadius(&bball);
  } else{
    rkglCASet( &cam,
      atof(opt[OPT_OX].arg), atof(opt[OPT_OY].arg), atof(opt[OPT_OZ].arg),
      atof(opt[OPT_PAN].arg), atof(opt[OPT_TILT].arg), atof(opt[OPT_ROLL].arg) );
    vv_width = 0.2;
    vv_near = 1;
    vv_far = 200;
  }
}

void rk_viewInit(void)
{
  zRGB rgb;

  win = rkglWindowCreateGLX( NULL, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg), RK_VIEW_TITLE );
  rkglWindowKeyEnableGLX( win );
  rkglWindowMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  zRGBDec( &rgb, opt[OPT_BG].arg );
  rkglBGSet( &cam, rgb.r, rgb.g, rgb.b );
  rkglVPCreate( &cam, 0, 0, atoi(opt[OPT_WIDTH].arg), atoi(opt[OPT_HEIGHT].arg) );
  rkglVPCreate( &cam, 0, 0, atoi( opt[OPT_WIDTH].arg ), atoi( opt[OPT_HEIGHT].arg ) );

  rk_viewReadModel();
  rk_viewResetCamera();

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0 );
  rkglLightMove( &light, atof(opt[OPT_LX].arg), atof(opt[OPT_LY].arg), atof(opt[OPT_LZ].arg) );
  rkglShadowInit( &shadow, 512, 512, 1.5, 0.2, 0.1 );
  rkglTextureEnable();

  if( opt[OPT_SMOOTH].flag ) glEnable( GL_LINE_SMOOTH );
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
  rkglVPCreate( &cam, 0, 0, reg.width, reg.height );
  x = vv_width / 2;
  y = x / rkglVPAspect(&cam);
  rkglFrustum( &cam, -x, x, -y, y, vv_near, vv_far );
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
    } else
      rk_viewResetCamera();
    break;
  case XK_c:
    rk_viewCapture();
    break;
  case XK_r:
    rk_viewResetCamera();
    break;
  case XK_q:
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
  rkglInitGLEW();
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
