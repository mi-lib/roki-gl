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

void rkglCloseGLX(void)
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
  win =
    XCreateWindow( zxdisplay,
      parent ? zxWindowBody(parent) : zxrootwindow,
      x, y, w, h, 0, _rkgl_vi_glx->depth,
      InputOutput, _rkgl_vi_glx->visual,
      CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWBackingStore | CWOverrideRedirect,
      &attr );
  XStoreName( zxdisplay, win, title );
  rkglActivateGLX( win );
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

int rkglKeyFuncGLX(rkglCamera *cam, KeySym key, int x, int y, double dl, double da)
{
  switch( key ){
  case XK_h: zxModkeyShiftIsOn() ?
    rkglCAMove( cam, 0,-dl, 0 ) : rkglCARelMove( cam, 0,-dl, 0 ); break;
  case XK_l: zxModkeyShiftIsOn() ?
    rkglCAMove( cam, 0, dl, 0 ) : rkglCARelMove( cam, 0, dl, 0 ); break;
  case XK_k: zxModkeyShiftIsOn() ?
    rkglCAMove( cam, 0, 0, dl ) : rkglCARelMove( cam, 0, 0, dl ); break;
  case XK_j: zxModkeyShiftIsOn() ?
    rkglCAMove( cam, 0, 0,-dl ) : rkglCARelMove( cam, 0, 0,-dl ); break;
  case XK_z: zxModkeyShiftIsOn() ?
    rkglCARelMove( cam, dl, 0, 0 ) : rkglCARelMove( cam,-dl, 0, 0 ); break;
  case XK_Up: zxModkeyCtrlIsOn() ?
    rkglCALockonPTR( cam, 0,-da, 0 ) : rkglCAPTR( cam, 0, da, 0 ); break;
  case XK_Down: zxModkeyCtrlIsOn() ?
    rkglCALockonPTR( cam, 0, da, 0 ) : rkglCAPTR( cam, 0,-da, 0 ); break;
  case XK_Left: zxModkeyCtrlIsOn() ?
    rkglCALockonPTR( cam,-da, 0, 0 ) : rkglCAPTR( cam, da, 0, 0 ); break;
  case XK_Right: zxModkeyCtrlIsOn() ?
    rkglCALockonPTR( cam, da, 0, 0 ) : rkglCAPTR( cam,-da, 0, 0 ); break;
  case XK_Q: case XK_q: case XK_Escape:
    rkglCloseGLX();
    return -1;
  default:
    if( !zxModkeyOn( key ) );
  }
  return 0;
}

static int _glx_mouse_button, _glx_mouse_x, _glx_mouse_y;
static int _glx_key_mod;

void rkglMouseFuncGLX(rkglCamera *cam, int button, int state, int x, int y)
{
  _glx_mouse_button = state == ButtonPress ? button : -1;
  _glx_mouse_x = x;
  _glx_mouse_y = y;
  _glx_key_mod = zxModkey();

  switch( _glx_mouse_button ){
  case Button4:
    rkglCARelMove( cam, 0.1, 0, 0 );
    break;
  case Button5:
    rkglCARelMove( cam,-0.1, 0, 0 );
    break;
  }
}

int rkglMouseDragFuncGLX(rkglCamera *cam, int x, int y)
{
  double dx, dy, r;

  dx = (double)( x - _glx_mouse_x ) / cam->vp[3];
  dy =-(double)( y - _glx_mouse_y ) / cam->vp[2];
  switch( _glx_mouse_button ){
  case Button1:
    r = 180 * sqrt( dx*dx + dy*dy );
    _glx_key_mod & ZX_MODKEY_CTRL ?
      rkglCARotate( cam, r, -dy, dx, 0 ) :
      rkglCALockonRotate( cam, r, -dy, dx, 0 );
    break;
  case Button3:
    _glx_key_mod & ZX_MODKEY_CTRL ?
      rkglCAMove( cam, 0, dx, dy ) :
      rkglCARelMove( cam, 0, dx, dy );
    break;
  case Button2:
    _glx_key_mod & ZX_MODKEY_CTRL ?
      rkglCAMove( cam, -dy, 0, 0 ) :
      rkglCARelMove( cam, -dy, 0, 0 );
    break;
  default: ;
  }
  _glx_mouse_x = x;
  _glx_mouse_y = y;
  return _glx_mouse_button;
}
