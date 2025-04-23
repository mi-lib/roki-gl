/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * roki_glx - GLX wrapper (on X11/OpenGL).
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/roki_glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>

GLXContext rkgl_ctx;
static XVisualInfo *_rkgl_vi_glx;
static Colormap _rkgl_cmap_glx;

static XVisualInfo *_rkglChooseVisualOneGLX(int attr[]);
static int _rkglChooseVisualGLX(int attr[]);

XVisualInfo *_rkglChooseVisualOneGLX(int attr[])
{
  return ( _rkgl_vi_glx = glXChooseVisual( zxdisplay, DefaultScreen(zxdisplay), attr ) );
}

int _rkglChooseVisualGLX(int attr[])
{
  if( !_rkglChooseVisualOneGLX( attr ) ){ /* first trial */
    attr[11] = None; /* give up double buffer */
    if( !_rkglChooseVisualOneGLX( attr ) ){ /* second trial */
      attr[10] = 0; /* give up alpha channel */
      if( !_rkglChooseVisualOneGLX( attr ) ){ /* third trial */
        ZRUNERROR( "cannot get visual info" );
        return -1;
      }
    }
  }
  return 0;
}

int rkglInitGLX(void)
{
  static int attr[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 12,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_ALPHA_SIZE, 1,
    GLX_DOUBLEBUFFER,
    None,
  };

  if( !zxdisplay ) zxInit();
  if( !glXQueryExtension( zxdisplay, NULL, NULL ) ){
    ZRUNERROR( "GLX extension not available" );
    return -1;
  }
  if( _rkglChooseVisualGLX( attr ) < 0 ){
    ZRUNERROR( "cannot get visual info" );
    return -1;
  }
  if( !( _rkgl_cmap_glx = XCreateColormap( zxdisplay, zxrootwindow, _rkgl_vi_glx->visual, AllocNone ) ) ){
    ZRUNERROR( "cannot create colormap" );
    return -1;
  }
  if( !( rkgl_ctx = glXCreateContext( zxdisplay, _rkgl_vi_glx, None, GL_TRUE ) ) ){
    ZRUNERROR( "cannot create X11/OpenGL rendering context" );
    return -1;
  }
  return 0;
}

void rkglExitGLX(void)
{
  XFreeColormap( zxdisplay, _rkgl_cmap_glx );
  glXDestroyContext( zxdisplay, rkgl_ctx );
  zxExit();
}

Window rkglWindowCreateGLX(zxWindow *parent, int x, int y, int w, int h, const char *title)
{
  Window win;
  XSetWindowAttributes attr;

  zxDefaultSetWindowAttributes( &attr );
  attr.colormap = _rkgl_cmap_glx;
  win = XCreateWindow( zxdisplay,
    parent ? zxWindowBody(parent) : zxrootwindow,
    x, y, w, h, 0, _rkgl_vi_glx->depth,
    InputOutput, _rkgl_vi_glx->visual,
    CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWBackingStore | CWOverrideRedirect,
    &attr );
  if( title )
    XStoreName( zxdisplay, win, title );
  rkglWindowActivateGLX( win );
  rkglEnableDefault();
#ifdef __ROKI_GL_USE_GLEW
  rkglInitGLEW();
#endif /* __ROKI_GL_USE_GLEW */
  return win;
}

void rkglWindowAddEventGLX(Window win, long event)
{
  XWindowAttributes attr;

  XGetWindowAttributes( zxdisplay, win, &attr );
  XSelectInput( zxdisplay, win, attr.all_event_masks | attr.your_event_mask | event );
}

/* default callback functions */

void rkglReshapeGLX(rkglCamera *cam, int w, int h)
{
  rkglCameraSetViewport( cam, 0, 0, w, h );
  rkglCameraPerspective( cam );
}

int rkglKeyPressFuncGLX(rkglCamera *cam)
{
  KeySym key;
  switch( ( key = zxKeySymbol() ) ){
  case XK_H: case XK_h: rkglKeyCameraMoveLeft(  cam ); break;
  case XK_L: case XK_l: rkglKeyCameraMoveRight( cam ); break;
  case XK_K: case XK_k: rkglKeyCameraMoveUp(    cam ); break;
  case XK_J: case XK_j: rkglKeyCameraMoveDown(  cam ); break;
  case XK_z:            rkglKeyCameraZoomIn(    cam ); break;
  case XK_Z:            rkglKeyCameraZoomOut(   cam ); break;
  case XK_Up:           rkglKeyCameraTiltUp(    cam ); break;
  case XK_Down:         rkglKeyCameraTiltDown(  cam ); break;
  case XK_Left:         rkglKeyCameraPanLeft(   cam ); break;
  case XK_Right:        rkglKeyCameraPanRight(  cam ); break;
  case XK_Q: case XK_q: case XK_Escape:
    rkglExitGLX();
    return -1;
  default:
    if( !zxModkeyOn( key ) ) break;
  }
  return 0;
}

int rkglKeyReleaseFuncGLX(rkglCamera *cam)
{
  return zxModkeyOff( zxKeySymbol() ) ? 1 : 0;
}

void rkglMouseFuncGLX(rkglCamera *cam, int event)
{
  rkglMouseStoreInput( zxMouseButton, event, ButtonPress, zxMouseX, zxMouseY, zxModkey() );
  switch( rkgl_mouse_button ){
  case Button4: rkglKeyCameraZoomIn(  cam ); break;
  case Button5: rkglKeyCameraZoomOut( cam ); break;
  default: ;
  }
}

void rkglMouseDragFuncGLX(rkglCamera *cam)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( cam, zxMouseX, zxMouseY, &dx, &dy );
  switch( rkgl_mouse_button ){
  case Button1: rkglMouseDragCameraRotate( cam, dx, dy, ZX_MODKEY_CTRL ); break;
  case Button3: rkglMouseDragCameraTranslate( cam, dx, dy ); break;
  case Button2: rkglMouseDragCameraZoom( cam, dx, dy ); break;
  default: ;
  }
  rkglMouseStoreXY( zxMouseX, zxMouseY );
}

/* buffer-to-image conversion */

static void _rkglReadBufferImage(zxImage *img, GLuint type, int bpp, int os1, int os2)
{
  GLint view[4];
  int i;
  uint j, k;
  ubyte *buf;

  glGetIntegerv( GL_VIEWPORT, view );
  buf = (ubyte *)malloc( sizeof(ubyte) * view[2] * view[3] * bpp );
  rkglReadBuffer( type, view[0], view[1], view[2], view[3], buf );
  zxImageAllocDefault( img, view[2], view[3] );
  for( i=img->height-1; i>=0; i-- )
    for( j=0; j<img->width; j++ ){
      k = bpp * ( i * img->width + j );
      zxImageCellFromRGB( img, j, img->height-i-1, buf[k], buf[k+os1], buf[k+os2] );
    }
  free( buf );
}

void rkglReadRGBImage(zxImage *img)
{
  _rkglReadBufferImage( img, GL_RGB, 3, 1, 2 );
}

void rkglReadDepthImage(zxImage *img)
{
  _rkglReadBufferImage( img, GL_DEPTH_COMPONENT, 1, 0, 0 );
}

void rkglReadRGBImageGLX(Window win, zxImage *img)
{
  zxRegion reg;

  zxGetGeometry( win, &reg );
  zxImageAllocDefault( img, reg.width, reg.height );
  zxImageFromPixmap( img, win, reg.width, reg.height );
}
