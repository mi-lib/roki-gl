/* with a courtesy to Mr. Daishi Kaneta. */
#include <roki_gl/roki_glut.h>

rkglFrameHandle fh;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
static const GLdouble g_znear = -100.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.01;

/* FrameHandle shape property */
static const double g_LENGTH = 2.0;
static const double g_MAGNITUDE = 1.0;

void draw_scene(void)
{
  rkglFrameHandleDraw( &fh );
}

void display(void)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionBuffer sb;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &g_cam, draw_scene, x, y, 1, 1 );
      rkglFrameHandleSelect( &fh, &sb, &g_cam, x, y );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
  if( rkglFrameHandleIsUnselected( &fh ) )
    rkglMouseFuncGLUT( button, state, x, y );
}

void motion(int x, int y)
{
  if( !rkglFrameHandleMove( &fh, &g_cam, x, y ) )
    rkglMouseDragFuncGLUT( x, y );
}

void resize(int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &g_cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &g_cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &g_cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &g_cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &g_cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &g_cam, 0, 0,-5 ); break;
  case '8': g_scale += 0.001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '9': rkglCARelMove( &g_cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &g_cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &g_cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &g_cam, 0, 0,-0.05 ); break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetCallbackParamGLUT( &g_cam, 0, 0, 0, 0, 0 );
  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 5, 0, 2, 0, -20, 0 );
  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &g_light, 8, 0, 8 );
  /* frame handle */
  rkglFrameHandleCreate( &fh, 0, g_LENGTH, g_MAGNITUDE );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
