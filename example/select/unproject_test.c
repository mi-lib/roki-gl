#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_select.h>

rkglCamera cam;
rkglLight light;

zVec3D pxy;

void display(void)
{
  static GLfloat yellow[] = { 1.0, 1.0, 0.2, 1.0 };
  static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };

  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  glMaterialfv( GL_FRONT, GL_DIFFUSE, yellow );
  glutSolidDodecahedron();

  glMaterialfv( GL_FRONT, GL_DIFFUSE, red );
  glPushMatrix();
  rkglTranslate( &pxy );
  glutSolidSphere( 0.05, 20, 20 );
  glPopMatrix();

  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      printf( "pressed at (%d,%d)\n", x, y );
      rkglPickAndUnproject( &cam, x, y, &pxy );
      zVec3DWrite( &pxy );
      rkglProject( &cam, &pxy, &x, &y );
      printf( "projected to (%d,%d)\n", x, y );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 30.0, (double)w / (double)h, 0.5, 20.0 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033' /* ESC */:
    exit( 0 );
  default: ;
  }
}

void init(void)
{
  rkglBGSet( &cam, 0.0, 0.0, 0.4 );
  rkglCALookAt( &cam, 5.0, 4.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 4.0, 8.0, 6.0 );
}

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  init();
  glutMainLoop();
  return 0;
}
