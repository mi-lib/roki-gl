/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 * contributer: 2023- Daishi Kaneta
 *
 * roki_glfw - GLFW wrapper (on OpenGL FrameWork).
 */

#include <signal.h>
#include <roki_gl/roki_glfw.h>

int rkglInitGLFW(int *argc, char **argv)
{
  if( glfwInit() == GL_FALSE ){
    ZRUNERROR( "Failed to initialize GLFW in glfwInit()" );
    return -1;
  }
  rkgl_mouse_button = -1;
  return 0;
}

void rkglWindowOpenGLFW(GLFWwindow* window, int x, int y)
{
  /* Create window by calling glfwCreateWindow() before calling this function. */
  glfwSetWindowPos( window, x, y );
  glfwShowWindow( window );
  glfwMakeContextCurrent( window );

#ifdef __ROKI_GL_USE_GLEW
  rkglInitGLEW();
#endif /* __ROKI_GL_USE_GLEW */
  rkglEnableDefault();
}

GLFWwindow *rkglWindowCreateAndOpenGLFW(int x, int y, int width, int height, const char *title)
{
  GLFWwindow *window;

  if( !( window = glfwCreateWindow( width, height, title, NULL, NULL ) ) ){
    ZRUNERROR( "Failed to create a window" );
    glfwTerminate();
    return NULL;
  }
  rkglWindowOpenGLFW( window, x, y );
  return window;
}

/* default callback functions */

void rkglReshapeFuncGLFW(GLFWwindow* window, int w, int h)
{
  rkglCameraFitFrustumToViewport( rkgl_default_cam, w, h, rkgl_default_vv_width, rkgl_default_vv_near, rkgl_default_vv_far );
}

void rkglCharFuncGLFW(GLFWwindow* window, unsigned int codepoint)
{
  switch( codepoint ){
  case 'h': rkglCameraRelMoveLeft(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'l': rkglCameraRelMoveRight( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'k': rkglCameraRelMoveUp(    rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'j': rkglCameraRelMoveDown(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'z': rkglCameraZoomIn(       rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'Z': rkglCameraZoomOut(      rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glfwPostEmptyEvent();
}

void rkglKeyFuncGLFW(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  int c;

  if( action == GLFW_PRESS){
    c = ( mods & GLFW_MOD_CONTROL );
    switch( key ){
    case GLFW_KEY_UP:    rkglKeyCameraRotateUp(    rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_DOWN:  rkglKeyCameraRotateDown(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_LEFT:  rkglKeyCameraRotateLeft(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_RIGHT: rkglKeyCameraRotateRight( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    default: ;
    }
    glfwPostEmptyEvent();
  }
}

void rkglMouseFuncGLFW(GLFWwindow* window, int button, int state, int mods)
{
  int ctrl_key;

  /* get modifier */
  ctrl_key = ( glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) & GLFW_PRESS ) ? GLFW_KEY_LEFT_CONTROL : 0;
  rkglMouseStoreButtonMod( button, state, GLFW_PRESS, ctrl_key );
}

void rkglMouseWheelFuncGLFW(GLFWwindow* window, double xoffset, double yoffset)
{
  /* if yoffset == 0, nothing happens. */
  if ( yoffset < 0 ){
    rkglCameraZoomIn(  rkgl_default_cam, rkgl_default_key_delta_trans );
  } else
  if ( yoffset > 0 ){
    rkglCameraZoomOut( rkgl_default_cam, rkgl_default_key_delta_trans );
  }
}

void rkglMouseDragFuncGLFW(GLFWwindow* window, double x, double y)
{
  double dx, dy;
  int px, py;

  px = floor( x );
  py = floor( y );
  rkglMouseDragGetIncrementer( rkgl_default_cam, px, py, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLFW_MOUSE_BUTTON_LEFT:   rkglMouseDragCameraRotate(    rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_RIGHT:  rkglMouseDragCameraTranslate( rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_MIDDLE: rkglMouseDragCameraZoom(      rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  default: ;
  }
  rkglMouseStoreXY( px, py );
  glfwPostEmptyEvent();
}

void rkglVisFuncGLFW(GLFWwindow* window)
{
  /* glfw doesn't have a function corresponding to glutIdleFunc() & callback */
  if( glfwGetWindowAttrib( window, GLFW_VISIBLE ) == GLFW_TRUE )
    glfwPostEmptyEvent();
}
