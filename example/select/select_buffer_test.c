#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

void put_box(double x, double y, double z, float r, float g, float b)
{
  zRGB color;

  glPushMatrix();
  glTranslated( x, y, z );
  zRGBSet( &color, r, g, b );
  rkglMaterialRGBA( &color, 0.5 );
  glutSolidCube( 0.8 );
  glPopMatrix();
}

void draw_scene(void)
{
  glPushMatrix();
  glTranslated( -5, 0, 0 );

  glLoadName( 0 ); put_box( 0, 0, 0, 0.0, 0.0, 1.0 );
  glLoadName( 1 ); put_box( 1, 0, 0, 1.0, 0.0, 1.0 );
  glLoadName( 2 );
  glPushName( 3 ); put_box( 2, 0, 0, 0.0, 1.0, 0.0 );
  glPushName( 4 ); put_box( 3, 0, 0, 1.0, 1.0, 0.0 );
  glLoadName( 5 ); put_box( 4, 0, 0, 1.0, 0.0, 0.0 );
  glPopName();
  glPopName();
  glLoadName( 6 ); put_box( 5, 0, 0, 0.0, 1.0, 1.0 );

  glPopMatrix();
}

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );
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
      rkglSelect( &sb, &cam, draw_scene, x, y, 1, 1 );
      rkglSelectionPrint( &sb );
      if( rkglSelectionFindNearest( &sb ) ){
        printf( "[nearest] " );
        rkglSelectionPrintName( &sb );
        printf( "\n" );
      }
      printf( "depth = %g\n", rkglGetDepth( &cam, x, y ) );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    exit(0);
  default: ;
  }
}

void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 5.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 );
  rkglCameraFitPerspective( &cam, 30.0, 1, 20 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10.0, 0.0, 10.0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
