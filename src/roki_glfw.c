/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) ***
 *
 * roki_glfw - GLFW wrapper (on OpenGL FrameWork).
 */

#include <signal.h>
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

void rkglReshapeFuncGLFW(GLFWwindow* window, int w, int h)
{
  rkglFrustumFit2VP( rkgl_default_cam, w, h, rkgl_default_vv_width, rkgl_default_vv_near, rkgl_default_vv_far );
}

void rkglCharFuncGLFW(GLFWwindow* window, unsigned int codepoint)
{
  switch( codepoint ){
  case 'h': rkglCARelMoveLeft(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'l': rkglCARelMoveRight( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'k': rkglCARelMoveUp(    rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'j': rkglCARelMoveDown(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'z': rkglCAZoomIn(       rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'Z': rkglCAZoomOut(      rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glfwPostEmptyEvent();
}

void rkglKeyFuncGLFW(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if( action == GLFW_PRESS){
    int c;

    c = ( mods & GLFW_MOD_CONTROL );
    switch( key ){
    case GLFW_KEY_UP:    rkglKeyCARotateUp(    rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_DOWN:  rkglKeyCARotateDown(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_LEFT:  rkglKeyCARotateLeft(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    case GLFW_KEY_RIGHT: rkglKeyCARotateRight( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
    default: ;
    }
    glfwPostEmptyEvent();
  }
}

void rkglMouseFuncGLFW(GLFWwindow* window, int button, int state, int x, int y)
{
  int ctrl_key;

  /* get modifier */
  ctrl_key = ( glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) & GLFW_PRESS ) ? GLFW_KEY_LEFT_CONTROL : 0;
  rkglMouseStoreInput( button, state, GLFW_PRESS, x, y, ctrl_key );
}

void rkglMouseWheelFuncGLFW(GLFWwindow* window, double xoffset, double yoffset)
{
  /* if yoffset == 0, nothing */
  if ( yoffset < 0 ){
    rkglCAZoomIn( rkgl_default_cam, rkgl_default_key_delta_trans );
  } else if ( yoffset > 0 ){
    rkglCAZoomOut( rkgl_default_cam, rkgl_default_key_delta_trans );
  }
}

void rkglMouseDragFuncGLFW(GLFWwindow* window, int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( rkgl_default_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLFW_MOUSE_BUTTON_LEFT:   rkglMouseDragCARotate(    rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_RIGHT:  rkglMouseDragCATranslate( rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_MIDDLE: rkglMouseDragCAZoom(      rkgl_default_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glfwPostEmptyEvent();
}

void rkglVisFuncGLFW(GLFWwindow* window)
{
  /* glfw doesn't have a function corresponding to glutIdleFunc() & callback */
  if( glfwGetWindowAttrib( window, GLFW_VISIBLE ) == GLFW_TRUE )
    glfwPostEmptyEvent();
}
