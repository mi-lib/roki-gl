/* with a courtesy to Mr. Daishi Kaneta. */
#include <roki_gl/roki_glut.h>

rkglFrameHandle fh;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;

/* FrameHandle shape property */
static const double g_LENGTH = 2.0;
static const double g_MAGNITUDE = 1.0;

void draw_scene(void)
{
  rkglFrameHandleDraw( &fh );
}

void display(void)
{
  rkglCameraPut( &g_cam );
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
      if( rkglSelectNearest( &sb, &g_cam, draw_scene, x, y, 1, 1 ) ){
        rkglFrameHandleAnchor( &fh, &sb, &g_cam, x, y );
      }
    } else
    if( state == GLUT_UP ){
      rkglFrameHandleUnselect( &fh );
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

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglCameraInit( &g_cam );
  rkglCameraSetBackground( &g_cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &g_cam, 10, 0, 2, 0, -20, 0 );
  rkglCameraFitPerspective( &g_cam, 30.0, 1, 100 );
  rkglSetDefaultCamera( &g_cam );
  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &g_light, 8, 0, 8 );
  rkglFrameHandleCreate( &fh, 0, g_LENGTH, g_MAGNITUDE );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );
  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
