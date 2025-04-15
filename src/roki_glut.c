/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * roki_glut - GLUT wrapper (on OpenGL utility toolkit).
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/roki_glut.h>

void rkglInitGLUT(int *argc, char **argv)
{
  glutInit( argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
}

int rkglWindowCreateGLUT(int x, int y, int w, int h, const char *title)
{
  int id;

  glutInitWindowPosition( x, y );
  glutInitWindowSize( w, h );
  id = glutCreateWindow( title );
#ifdef __ROKI_GL_USE_GLEW
  rkglInitGLEW();
#endif /* __ROKI_GL_USE_GLEW */
  rkglEnableDefault();
  return id;
}

/* default callback functions */

void rkglReshapeFuncGLUT(int w, int h)
{
#if 0
  rkglVVFrustumFit2VP( rkgl_default_cam, w, h, rkgl_default_vv_width, rkgl_default_vv_near, rkgl_default_vv_far );
#else
  rkglCameraFitFrustumToViewport( rkgl_default_cam, w, h, rkgl_default_vv_width, rkgl_default_vv_near, rkgl_default_vv_far );
#endif
}

void rkglIdleFuncGLUT(void)
{
  glutPostRedisplay();
}

void rkglKeyFuncGLUT(unsigned char key, int x, int y)
{
  switch( key ){
#if 0
  case 'h': rkglCARelMoveLeft(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'l': rkglCARelMoveRight( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'k': rkglCARelMoveUp(    rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'j': rkglCARelMoveDown(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'z': rkglCAZoomIn(       rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'Z': rkglCAZoomOut(      rkgl_default_cam, rkgl_default_key_delta_trans ); break;
#else
  case 'h': rkglCameraRelMoveLeft(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'l': rkglCameraRelMoveRight( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'k': rkglCameraRelMoveUp(    rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'j': rkglCameraRelMoveDown(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'z': rkglCameraZoomIn(       rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case 'Z': rkglCameraZoomOut(      rkgl_default_cam, rkgl_default_key_delta_trans ); break;
#endif
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

void rkglSpecialFuncGLUT(int key, int x, int y)
{
  int c;

  c = glutGetModifiers() & GLUT_ACTIVE_CTRL;
  switch( key ){
#if 0
  case GLUT_KEY_UP:    rkglKeyCARotateUp(   rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_DOWN:  rkglKeyCARotateDown( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_LEFT:  rkglKeyCARotateLeft(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_RIGHT: rkglKeyCARotateRight( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
#else
  case GLUT_KEY_UP:    rkglKeyCameraRotateUp(   rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_DOWN:  rkglKeyCameraRotateDown( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_LEFT:  rkglKeyCameraRotateLeft(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_RIGHT: rkglKeyCameraRotateRight( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
#endif
  default: ;
  }
  glutPostRedisplay();
}

void rkglMouseFuncGLUT(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLUT_DOWN, x, y, glutGetModifiers() );
  switch( rkgl_mouse_button ){
#if 0
  case GLUT_WHEEL_UP:   rkglCAZoomIn(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case GLUT_WHEEL_DOWN: rkglCAZoomOut( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
#else
  case GLUT_WHEEL_UP:   rkglCameraZoomIn(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case GLUT_WHEEL_DOWN: rkglCameraZoomOut( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
#endif
  default: ;
  }
}

void rkglMouseDragFuncGLUT(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( rkgl_default_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
#if 0
  case GLUT_LEFT_BUTTON:   rkglMouseDragCARotate(    rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_RIGHT_BUTTON:  rkglMouseDragCATranslate( rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_MIDDLE_BUTTON: rkglMouseDragCAZoom(      rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
#else
  case GLUT_LEFT_BUTTON:   rkglMouseDragCameraRotate(    rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_RIGHT_BUTTON:  rkglMouseDragCameraTranslate( rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_MIDDLE_BUTTON: rkglMouseDragCameraZoom(      rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
#endif
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glutPostRedisplay();
}

void rkglVisFuncGLUT(int visible)
{
  glutIdleFunc( visible == GLUT_VISIBLE ? rkglIdleFuncGLUT : NULL );
}
