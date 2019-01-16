#include <roki/rokigl.h>
#include <roki/rkgl_glx.h>
#include <zx11/zximage_png.h>
#include <roki/rkgl_nurbs3d.h>
#include <terra/terra.h>

Window win;

rkglCamera cam;
rkglLight light;

zTerra terra;
rkglNURBS3D glns;
zNURBS3DSeq nseq;
zNURBS3DSeqListCell *nseq_now;

int cnt;

void rk_nurbsViewCapture(void)
{
  zxRegion reg;
  zxImage img;
  static char imgfile[BUFSIZ];
  static int count = 0;

  sprintf( imgfile, "%04d.png", count++ );
  zxGetGeometry( win, &reg );
  zxImageAllocDefault( &img, reg.width, reg.height );
  zxImageFromPixmap( &img, win, img.width, img.height );
  zxImageWritePNGFile( &img, imgfile );
  zxImageDestroy( &img );
}

/**********************************************************/

void rk_nurbsViewDraw(void)
{
  rkglNURBS3DDraw( &glns );
}

void rk_nurbsNext(void)
{
  if( nseq_now != zListTail( &nseq ) ){
    nseq_now = zListCellPrev( nseq_now );
    cnt++;
  }
  eprintf("cnt: %d\n", cnt);
  rkglNURBS3DSetNURBS( &glns, &nseq_now->data.ns );
}

void rk_nurbsPrev(void)
{
  if( nseq_now != zListHead( &nseq ) ){
    nseq_now = zListCellNext( nseq_now );
    cnt--;
  }
  eprintf("cnt: %d\n", cnt);
  rkglNURBS3DSetNURBS( &glns, &nseq_now->data.ns );
}

void rk_nurbsViewDisplay(void)
{
  rkglActivateGLX( win );
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rk_nurbsViewDraw();
  rkglSwapBuffersGLX( win );
  rkglFlushGLX();
}

void rk_nurbsViewInit(int argc, char *argv[])
{
  zRGB rgb;

  win = rkglWindowCreateGLX( NULL, 0, 0, 500, 500, NULL );
  rkglKeyEnableGLX( win );
  rkglMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  zRGBDec( &rgb, "#505050" );
  rkglBGSet( &cam, rgb.r, rgb.g, rgb.b );
  rkglVPCreate( &cam, 0, 0, 500, 500 );
  rkglCASet( &cam, 5, 0, 0, 0, 0, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.5, 0.5, 0.5, 0.8, 0.8, 0.8, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 3, 0, 3 );

  rkglNURBS3DCreate( &glns );
  zTerraReadFile( &terra, argv[1] );

  rkglNURBS3DLoad( &glns, zTerraNURBS3D(&terra) );
  rkglNURBS3DSetOpticalInfo( &glns, zTerraOpticalInfo(&terra) );

  zNURBS3DSeqBReadFile( &nseq, argv[2] );
  nseq_now = zListHead( &nseq );

  cnt = 0;
}

void rk_nurbsViewExit(void)
{
  rkglWindowCloseGLX( win );
  rkglCloseGLX();
  rkglNURBS3DDestroy( &glns );
}

/**********************************************************/
void rk_nurbsViewReshape(void)
{
  zxRegion reg;
  double x, y;

  zxGetGeometry( win, &reg );
  x = 0.1;
  rkglVPCreate( &cam, 0, 0, reg.width, reg.height );
  y = x / rkglVPAspect(&cam);
  rkglFrustum( &cam, -x, x, -y, y, 1, 20 );
}

enum{ RK_NURBSVIEW_CAM_ROT, RK_NURBSVIEW_CAM_PAN, RK_NURBSVIEW_CAM_ZOOM };

static int mouse_button = -1;
static int mousex, mousey;
static byte cammode = RK_NURBSVIEW_CAM_ROT;

void rk_nurbsViewStoreMouseInfo(byte mode)
{
  mouse_button = zxMouseButton;
  mousex = zxMouseX;
  mousey = zxMouseY;
  cammode = mode;
}

void rk_nurbsViewMousePress(void)
{
  switch( zxMouseButton ){
  case Button1: rk_nurbsViewStoreMouseInfo( RK_NURBSVIEW_CAM_ROT );  break;
  case Button3: rk_nurbsViewStoreMouseInfo( RK_NURBSVIEW_CAM_PAN );  break;
  case Button2: rk_nurbsViewStoreMouseInfo( RK_NURBSVIEW_CAM_ZOOM ); break;
  case Button4: rkglCARelMove( &cam,-0.1, 0, 0 );          break;
  case Button5: rkglCARelMove( &cam, 0.1, 0, 0 );          break;
  default: ;
  }
}

void rk_nurbsViewMouseRelease(void)
{
  mouse_button = -1;
}

void rk_nurbsViewMouseDrag(void)
{
  double dx, dy, r;

  if( mouse_button == -1 ) return;
  dx = (double)( zxMouseX - mousex ) / cam.vp[3];
  dy =-(double)( zxMouseY - mousey ) / cam.vp[2];
  switch( cammode ){
  case RK_NURBSVIEW_CAM_ROT:
    r = 180 * sqrt( dx*dx + dy*dy );
    zxModkeyCtrlIsOn() ?
      rkglCARotate( &cam, r, -dy, dx, 0 ) :
      rkglCALockonRotate( &cam, r, -dy, dx, 0 );
    break;
  case RK_NURBSVIEW_CAM_PAN:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, 0, dx, dy ) :
      rkglCARelMove( &cam, 0, dx, dy );
    break;
  case RK_NURBSVIEW_CAM_ZOOM:
    zxModkeyCtrlIsOn() ?
      rkglCAMove( &cam, -dy, 0, 0 ) :
      rkglCARelMove( &cam, -2*dy, 0, 0 );
    break;
  default: ;
  }
  mousex = zxMouseX;
  mousey = zxMouseY;
}

int rk_nurbsViewKeyPress(void)
{
  static bool from_light = false;
  static bool face = false;
  static bool poly = false;

  zxModkeyOn( zxKeySymbol() );
  switch( zxKeySymbol() ){
  case XK_l: /* toggle viewpoint to light/camera */
    if( ( from_light = 1 - from_light ) ){
      rkglCALookAt( &cam, 3, 0, 3, 0, 0, 0, -1, 0, 1 );
    } else{
      rkglCASet( &cam, 5, 0, 0, 0, 0, 0 );
    }
    break;
  case XK_n:
    rk_nurbsNext();
    break;
  case XK_p:
    rk_nurbsPrev();
    break;
  case XK_c:
    rk_nurbsViewCapture();
    break;
  case XK_b:
    face = face ? false : true;
    rkglNURBS3DOnDrawBothFaces( &glns, face );
    break;
  case XK_m:
    poly = poly ? false : true;
    if( poly )
      rkglNURBS3DOnDrawPolygon( &glns );
    else
      rkglNURBS3DOnDrawFill( &glns );
    break;
  case XK_q:
    eprintf( "quit.\n" );
    return -1;
  }
  return 0;
}

int rk_nurbsViewEvent(void)
{
  switch( zxDequeueEvent() ){
  case Expose:
  case ConfigureNotify: rk_nurbsViewReshape();             break;
  case ButtonPress:     rk_nurbsViewMousePress();          break;
  case MotionNotify:    rk_nurbsViewMouseDrag();           break;
  case ButtonRelease:   rk_nurbsViewMouseRelease();        break;
  case KeyPress:
    if( rk_nurbsViewKeyPress() < 0 ) return -1;            break;
  case KeyRelease:      zxModkeyOff( zxKeySymbol() ); break;
  default: ;
  }
  return 0;
}

void rk_nurbsViewLoop(void)
{
#define RK_NURBSVIEW_LOOP_SKIP 5
  int count = 0;

  /* initial draw */
  rk_nurbsViewReshape();
  rk_nurbsViewDisplay();
  while( rk_nurbsViewEvent() == 0 ){
    if( count++ > RK_NURBSVIEW_LOOP_SKIP ){
      rk_nurbsViewDisplay();
      count = 0;
    }
  }
}

int main(int argc, char *argv[])
{
  if( argc != 3 ) return 0;
  rkglInitGLX();
  rk_nurbsViewInit( argc, argv );
  rk_nurbsViewLoop();
  rk_nurbsViewExit();
  return 0;
}

