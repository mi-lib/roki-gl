/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glut - GLUT (OpenGL utility toolkit) wrapper.
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/rkgl_glut.h>

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

static rkglCamera *_glut_cam;
static double _glut_vv_width;
static double _glut_vv_near;
static double _glut_vv_far;
static double _glut_dl;
static double _glut_da;

void rkglSetCallbackParamGLUT(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl, double da)
{
  _glut_cam = c;
  _glut_vv_width = vv_width;
  _glut_vv_near = vv_near;
  _glut_vv_far = vv_far;
  _glut_dl = dl;
  _glut_da = da;
}

void rkglReshapeFuncGLUT(int w, int h)
{
  double x, y;

  rkglVPCreate( _glut_cam, 0, 0, w, h );
  x = 0.5 * _glut_vv_width;
  y = x / rkglVPAspect(_glut_cam);
  rkglFrustum( _glut_cam, -x, x, -y, y, _glut_vv_near, _glut_vv_far );
}

void rkglIdleFuncGLUT(void)
{
  glutPostRedisplay();
}

void rkglKeyFuncGLUT(unsigned char key, int x, int y)
{
  switch( key ){
  case 'h': rkglCARelMoveLeft(  _glut_cam, _glut_dl ); break;
  case 'l': rkglCARelMoveRight( _glut_cam, _glut_dl ); break;
  case 'k': rkglCARelMoveUp(    _glut_cam, _glut_dl ); break;
  case 'j': rkglCARelMoveDown(  _glut_cam, _glut_dl ); break;
  case 'z': rkglCAZoomIn(       _glut_cam, _glut_dl ); break;
  case 'Z': rkglCAZoomOut(      _glut_cam, _glut_dl ); break;
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
  case GLUT_KEY_UP:    rkglKeyCARotateUp(   _glut_cam, _glut_da, c ); break;
  case GLUT_KEY_DOWN:  rkglKeyCARotateDown( _glut_cam, _glut_da, c ); break;
  case GLUT_KEY_LEFT:  rkglKeyCARotateLeft(  _glut_cam, _glut_da, c ); break;
  case GLUT_KEY_RIGHT: rkglKeyCARotateRight( _glut_cam, _glut_da, c ); break;
  default: ;
  }
  glutPostRedisplay();
}

void rkglMouseFuncGLUT(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLUT_DOWN, x, y, glutGetModifiers() );
  switch( rkgl_mouse_button ){
  case GLUT_WHEEL_UP:   rkglCAZoomIn( _glut_cam, _glut_dl ); break;
  case GLUT_WHEEL_DOWN: rkglCAZoomOut(  _glut_cam, _glut_dl ); break;
  default: ;
  }
}

void rkglMouseDragFuncGLUT(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( _glut_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLUT_LEFT_BUTTON:   rkglMouseDragCARotate(    _glut_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_RIGHT_BUTTON:  rkglMouseDragCATranslate( _glut_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_MIDDLE_BUTTON: rkglMouseDragCAZoom(      _glut_cam, dx, dy, GLUT_ACTIVE_CTRL ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glutPostRedisplay();
}

void rkglVisFuncGLUT(int visible)
{
  glutIdleFunc( visible == GLUT_VISIBLE ? rkglIdleFuncGLUT : NULL );
}
