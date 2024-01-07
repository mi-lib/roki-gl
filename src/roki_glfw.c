/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) ***
 *
 * roki_glfw - GLFW wrapper (on OpenGL FrameWork).
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/roki_glfw.h>

int rkglInitGLFW(int *argc, char **argv)
{
  if( glfwInit() == GL_FALSE ){
    ZRUNERROR("Failed glfwInit()");
    return -1;
  }
  return 0;
}

int rkglWindowCreateGLFW(GLFWwindow* window, int x, int y, int w, int h, const char *title)
{
  /* Execute window = glfwCreateWindow(w, h, title, NULL, NULL); before calling this function */
  if ( window == NULL ){
    ZRUNERROR("Failed glfwCreateWindow()");
    glfwTerminate();
    return -1;
  }
  glfwSetWindowPos( window, x, y );
  glfwShowWindow( window );
  glfwMakeContextCurrent( window );

#ifdef __ROKI_GL_USE_GLEW
  rkglInitGLEW();
#endif /* __ROKI_GL_USE_GLEW */
  rkglEnableDefault();

  return 0;
}


/* default callback functions */

static rkglCamera *_glfw_cam;
static double _glfw_vv_width;
static double _glfw_vv_near;
static double _glfw_vv_far;
static double _glfw_dl;
static double _glfw_da;

void rkglSetCallbackParamGLFW(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl, double da)
{
  _glfw_cam = c;
  _glfw_vv_width = vv_width;
  _glfw_vv_near = vv_near;
  _glfw_vv_far = vv_far;
  _glfw_dl = dl;
  _glfw_da = da;
}

void rkglReshapeFuncGLFW(int w, int h)
{
  double x, y;

  rkglVPCreate( _glfw_cam, 0, 0, w, h );
  x = 0.5 * _glfw_vv_width;
  y = x / rkglVPAspect(_glfw_cam);
  rkglFrustum( _glfw_cam, -x, x, -y, y, _glfw_vv_near, _glfw_vv_far );
}

void rkglIdleFuncGLFW(void)
{
  glfwPostEmptyEvent();
}

void rkglKeyFuncGLFW(unsigned char key, int x, int y)
{
  switch( key ){
  case 'h': rkglCARelMoveLeft(  _glfw_cam, _glfw_dl ); break;
  case 'l': rkglCARelMoveRight( _glfw_cam, _glfw_dl ); break;
  case 'k': rkglCARelMoveUp(    _glfw_cam, _glfw_dl ); break;
  case 'j': rkglCARelMoveDown(  _glfw_cam, _glfw_dl ); break;
  case 'z': rkglCAZoomIn(       _glfw_cam, _glfw_dl ); break;
  case 'Z': rkglCAZoomOut(      _glfw_cam, _glfw_dl ); break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glfwPostEmptyEvent();
}

void rkglSpecialFuncGLFW(GLFWwindow* window, int key, int x, int y)
{
  int c;

  c = glfwGetKey( window, GLFW_KEY_LEFT_CONTROL) & GLFW_PRESS;
  switch( key ){
  case GLFW_KEY_UP:    rkglKeyCARotateUp(    _glfw_cam, _glfw_da, c ); break;
  case GLFW_KEY_DOWN:  rkglKeyCARotateDown(  _glfw_cam, _glfw_da, c ); break;
  case GLFW_KEY_LEFT:  rkglKeyCARotateLeft(  _glfw_cam, _glfw_da, c ); break;
  case GLFW_KEY_RIGHT: rkglKeyCARotateRight( _glfw_cam, _glfw_da, c ); break;
  default: ;
  }
  glfwPostEmptyEvent();
}

void rkglMouseFuncGLFW(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLFW_PRESS, x, y, GLFW_KEY_LEFT_CONTROL );
}

void rkglMouseWheelFuncGLFW(GLFWwindow* window, double xoffset, double yoffset)
{
  if ( yoffset < 0 ) {
    rkglCAZoomIn( _glfw_cam, _glfw_dl );
  }
  if ( yoffset > 0 ) {
    rkglCAZoomOut( _glfw_cam, _glfw_dl );
  }
}

void rkglMouseDragFuncGLFW(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( _glfw_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLFW_MOUSE_BUTTON_LEFT:   rkglMouseDragCARotate(    _glfw_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_RIGHT:  rkglMouseDragCATranslate( _glfw_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_MIDDLE: rkglMouseDragCAZoom(      _glfw_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glfwPostEmptyEvent();
}

void rkglVisFuncGLFW(GLFWwindow* window)
{
  if( glfwGetWindowAttrib( window, GLFW_VISIBLE ) == GLFW_TRUE )
    glfwPostEmptyEvent();
}
