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
  rkglFrustumFit2VP( rkgl_default_cam, w, h, rkgl_default_vv_width, rkgl_default_vv_near, rkgl_default_vv_far );
}

void rkglIdleFuncGLUT(void)
{
  glutPostRedisplay();
}

void rkglKeyFuncGLUT(unsigned char key, int x, int y)
{
  switch( key ){
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
  glutPostRedisplay();
}

void rkglSpecialFuncGLUT(int key, int x, int y)
{
  int c;

  c = glutGetModifiers() & GLUT_ACTIVE_CTRL;
  switch( key ){
  case GLUT_KEY_UP:    rkglKeyCARotateUp(   rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_DOWN:  rkglKeyCARotateDown( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_LEFT:  rkglKeyCARotateLeft(  rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  case GLUT_KEY_RIGHT: rkglKeyCARotateRight( rkgl_default_cam, rkgl_default_key_delta_angle, c ); break;
  default: ;
  }
  glutPostRedisplay();
}

void rkglMouseFuncGLUT(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLUT_DOWN, x, y, glutGetModifiers() );
  switch( rkgl_mouse_button ){
  case GLUT_WHEEL_UP:   rkglCAZoomIn(  rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  case GLUT_WHEEL_DOWN: rkglCAZoomOut( rkgl_default_cam, rkgl_default_key_delta_trans ); break;
  default: ;
  }
}

void rkglMouseDragFuncGLUT(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( rkgl_default_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLUT_LEFT_BUTTON:   rkglMouseDragCARotate(    rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_RIGHT_BUTTON:  rkglMouseDragCATranslate( rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_MIDDLE_BUTTON: rkglMouseDragCAZoom(      rkgl_default_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glutPostRedisplay();
}

void rkglVisFuncGLUT(int visible)
{
  glutIdleFunc( visible == GLUT_VISIBLE ? rkglIdleFuncGLUT : NULL );
}
