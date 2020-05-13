/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glx - X11/OpenGL wrapper
 */

#include <signal.h>
#include <math.h>
#include <roki-gl/rkgl_glx.h>
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

  zxInit();
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
  XStoreName( zxdisplay, win, title );
  rkglWindowActivateGLX( win );
  rkglEnableDefault();
  return win;
}

void rkglWindowAddEventGLX(Window win, long event)
{
  XWindowAttributes attr;

  XGetWindowAttributes( zxdisplay, win, &attr );
  XSelectInput( zxdisplay, win, attr.all_event_masks | attr.your_event_mask | event );
}

/* default callback functions */

void rkglReshapeGLX(rkglCamera *cam, int w, int h, double vvwidth, double vvnear, double vvfar)
{
  double x, y;

  rkglVPCreate( cam, 0, 0, w, h );
  x = 0.5 * vvwidth;
  y = x / rkglVPAspect(cam);
  rkglFrustum( cam, -x, x, -y, y, vvnear, vvfar );
}

int rkglKeyFuncGLX(rkglCamera *cam, double dl, double da)
{
  KeySym key;
  switch( ( key = zxKeySymbol() ) ){
  case XK_h:     rkglCARelMoveLeft(  cam, dl ); break;
  case XK_l:     rkglCARelMoveRight( cam, dl ); break;
  case XK_k:     rkglCARelMoveUp(    cam, dl ); break;
  case XK_j:     rkglCARelMoveDown(  cam, dl ); break;
  case XK_z: zxModkeyShiftIsOn() ?
                 rkglCAZoomOut( cam, dl ) : rkglCAZoomIn( cam, dl );  break;
  case XK_Up:    rkglKeyCARotateUp(    cam, da, zxModkeyCtrlIsOn() ); break;
  case XK_Down:  rkglKeyCARotateDown(  cam, da, zxModkeyCtrlIsOn() ); break;
  case XK_Left:  rkglKeyCARotateLeft(  cam, da, zxModkeyCtrlIsOn() ); break;
  case XK_Right: rkglKeyCARotateRight( cam, da, zxModkeyCtrlIsOn() ); break;
  case XK_Q: case XK_q: case XK_Escape:
    rkglExitGLX();
    return -1;
  default:
    if( !zxModkeyOn( key ) );
  }
  return 0;
}

void rkglMouseFuncGLX(rkglCamera *cam, int event, double dl)
{
  rkglMouseStoreInput( zxMouseButton, event, ButtonPress, zxMouseX, zxMouseY, zxModkey() );
  switch( rkgl_mouse_button ){
  case Button4: rkglCAZoomIn( cam, dl ); break;
  case Button5: rkglCAZoomOut(  cam, dl ); break;
  default: ;
  }
}

void rkglMouseDragFuncGLX(rkglCamera *cam)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( cam, zxMouseX, zxMouseY, &dx, &dy );
  switch( rkgl_mouse_button ){
  case Button1: rkglMouseDragCARotate(    cam, dx, dy, ZX_MODKEY_CTRL ); break;
  case Button3: rkglMouseDragCATranslate( cam, dx, dy, ZX_MODKEY_CTRL ); break;
  case Button2: rkglMouseDragCAZoom(      cam, dx, dy, ZX_MODKEY_CTRL ); break;
  default: ;
  }
  rkglMouseStoreXY( zxMouseX, zxMouseY );
}
