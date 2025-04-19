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
  rkglCameraSetViewport( rkgl_default_camera, 0, 0, w, h );
  rkglCameraPerspective( rkgl_default_camera );
}

void rkglIdleFuncGLUT(void)
{
  glutPostRedisplay();
}

void rkglKeyFuncGLUT(unsigned char key, int x, int y)
{
  switch( key ){
  case 'h': rkglKeyDefaultCameraMoveLeft();  break;
  case 'l': rkglKeyDefaultCameraMoveRight(); break;
  case 'k': rkglKeyDefaultCameraMoveUp();    break;
  case 'j': rkglKeyDefaultCameraMoveDown();  break;
  case 'z': rkglKeyDefaultCameraZoomIn();    break;
  case 'Z': rkglKeyDefaultCameraZoomOut();   break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

void rkglSpecialFuncGLUT(int key, int x, int y)
{
  switch( key ){
  case GLUT_KEY_UP:    rkglKeyDefaultCameraTiltUp();   break;
  case GLUT_KEY_DOWN:  rkglKeyDefaultCameraTiltDown(); break;
  case GLUT_KEY_LEFT:  rkglKeyDefaultCameraPanLeft();  break;
  case GLUT_KEY_RIGHT: rkglKeyDefaultCameraPanRight(); break;
  default: ;
  }
  glutPostRedisplay();
}

void rkglMouseFuncGLUT(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLUT_DOWN, x, y, glutGetModifiers() );
  switch( rkgl_mouse_button ){
  case GLUT_WHEEL_UP:   rkglKeyDefaultCameraZoomIn();  break;
  case GLUT_WHEEL_DOWN: rkglKeyDefaultCameraZoomOut(); break;
  default: ;
  }
}

void rkglMouseDragFuncGLUT(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( rkgl_default_camera, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLUT_LEFT_BUTTON:   rkglMouseDragDefaultCameraRotate( dx, dy, GLUT_ACTIVE_CTRL ); break;
  case GLUT_RIGHT_BUTTON:  rkglMouseDragDefaultCameraTranslate( dx, dy ); break;
  case GLUT_MIDDLE_BUTTON: rkglMouseDragDefaultCameraZoom( dx, dy ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glutPostRedisplay();
}

void rkglVisFuncGLUT(int visible)
{
  glutIdleFunc( visible == GLUT_VISIBLE ? rkglIdleFuncGLUT : NULL );
}
