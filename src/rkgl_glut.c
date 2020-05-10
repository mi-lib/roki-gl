/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glut - GLUT (OpenGL utility toolkit) wrapper
 */

#include <signal.h>
#include <math.h>
#include <roki-gl/rkgl_glut.h>

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
  rkglEnableDefault();
  return id;
}

/* default callback functions */

static rkglCamera *_glut_cam;
static double _glut_vv_width;
static double _glut_vv_near;
static double _glut_vv_far;
static double _glut_dl_key;
static double _glut_da_key;

void rkglSetCallbackParamGLUT(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl_key, double da_key)
{
  _glut_cam = c;
  _glut_vv_width = vv_width;
  _glut_vv_near = vv_near;
  _glut_vv_far = vv_far;
  _glut_dl_key = dl_key;
  _glut_da_key = da_key;
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
  case 'h': rkglCARelMove( _glut_cam, 0,-_glut_dl_key, 0 ); break;
  case 'l': rkglCARelMove( _glut_cam, 0, _glut_dl_key, 0 ); break;
  case 'k': rkglCARelMove( _glut_cam, 0, 0, _glut_dl_key ); break;
  case 'j': rkglCARelMove( _glut_cam, 0, 0,-_glut_dl_key ); break;
  case 'z': rkglCARelMove( _glut_cam,-_glut_dl_key, 0, 0 ); break;
  case 'Z': rkglCARelMove( _glut_cam, _glut_dl_key, 0, 0 ); break;
  case 'h'-0x60: rkglCAMove( _glut_cam, 0,-_glut_dl_key, 0 ); break;
  case 'l'-0x60: rkglCAMove( _glut_cam, 0, _glut_dl_key, 0 ); break;
  case 'k'-0x60: rkglCAMove( _glut_cam, 0, 0, _glut_dl_key ); break;
  case 'j'-0x60: rkglCAMove( _glut_cam, 0, 0,-_glut_dl_key ); break;
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
  case GLUT_KEY_UP:
    c ? rkglCALockonPTR( _glut_cam, 0,-_glut_da_key, 0 ) :
        rkglCAPTR( _glut_cam, 0, _glut_da_key, 0 );
    break;
  case GLUT_KEY_DOWN:
    c ? rkglCALockonPTR( _glut_cam, 0, _glut_da_key, 0 ) :
        rkglCAPTR( _glut_cam, 0,-_glut_da_key, 0 );
    break;
  case GLUT_KEY_LEFT:
    c ? rkglCALockonPTR( _glut_cam,-_glut_da_key, 0, 0 ) :
        rkglCAPTR( _glut_cam, _glut_da_key, 0, 0 );
    break;
  case GLUT_KEY_RIGHT:
    c ? rkglCALockonPTR( _glut_cam, _glut_da_key, 0, 0 ) :
        rkglCAPTR( _glut_cam,-_glut_da_key, 0, 0 );
    break;
  default: ;
  }
  glutPostRedisplay();
}

static int _glut_mouse_button, _glut_mouse_x, _glut_mouse_y;
static int _glut_key_mod;

void rkglSaveInputStatusGLUT(int button, int state, int x, int y)
{
  _glut_mouse_button = state == GLUT_DOWN ? button : -1;
  _glut_mouse_x = x;
  _glut_mouse_y = y;
  _glut_key_mod = glutGetModifiers();
}

void rkglMouseFuncGLUT(int button, int state, int x, int y)
{
  rkglSaveInputStatusGLUT( button, state, x, y );
}

void rkglMouseDragFuncGLUT(int x, int y)
{
  int c;
  double dx, dy, r;

  dx = (double)( x - _glut_mouse_x ) / _glut_cam->vp[3];
  dy =-(double)( y - _glut_mouse_y ) / _glut_cam->vp[2];
  c = _glut_key_mod & GLUT_ACTIVE_CTRL;
  switch( _glut_mouse_button ){
  case GLUT_LEFT_BUTTON:
    r = 180 * sqrt( dx*dx + dy*dy );
    c ? rkglCALockonRotate( _glut_cam, r, -dy, dx, 0 ) :
        rkglCARotate( _glut_cam, r, -dy, dx, 0 );
    break;
  case GLUT_RIGHT_BUTTON:
    c ? rkglCARelMove( _glut_cam, 0, dx, dy ) :
        rkglCAMove( _glut_cam, 0, dx, dy );
    break;
  case GLUT_MIDDLE_BUTTON:
    c ? rkglCARelMove( _glut_cam, -dy, 0, 0 ) :
        rkglCAMove( _glut_cam, -dy, 0, 0 );
    break;
  default: ;
  }
  _glut_mouse_x = x;
  _glut_mouse_y = y;
  glutPostRedisplay();
}

void rkglVisFuncGLUT(int visible)
{
  glutIdleFunc( visible == GLUT_VISIBLE ? rkglIdleFuncGLUT : NULL );
}
