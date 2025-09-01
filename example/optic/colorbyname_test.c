#include <roki_gl/roki_glut.h>

rkglCamera cam;

void display(void)
{
  const GLdouble vertex[][3] = {
    { 0,-1,-1 },
    { 0, 1,-1 },
    { 0, 1, 1 },
    { 0,-1, 1 },
  };

  rkglCameraPut( &cam );
  glPushMatrix();
  rkglClear();
  glBegin( GL_TRIANGLE_STRIP );
  glVertex3dv( vertex[0] );
  glVertex3dv( vertex[3] );
  glVertex3dv( vertex[1] );
  glVertex3dv( vertex[2] );
  glEnd();
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case '0': rkglRGBByStr( "black" );   break;
  case 'r': rkglRGBByStr( "red" );     break;
  case 'g': rkglRGBByStr( "green" );   break;
  case 'b': rkglRGBByStr( "blue" );    break;
  case 'y': rkglRGBByStr( "yellow" );  break;
  case 'c': rkglRGBByStr( "cyan" );    break;
  case 'm': rkglRGBByStr( "magenta" ); break;
  case 'w': rkglRGBByStr( "white" );   break;
  case '-': rkglRGBIntensify(-0.1,-0.1,-0.1 ); break;
  case '+': rkglRGBIntensify( 0.1, 0.1, 0.1 ); break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 240, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  glCullFace( GL_FRONT );
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 10, 0, 0, 0, 0, 0 );
  rkglCameraFitPerspective( &cam, 30.0, 1, 20 );
  rkglSetDefaultCamera( &cam );
  rkglRGBByStr( "black" );
  glutMainLoop();
  return 0;
}
