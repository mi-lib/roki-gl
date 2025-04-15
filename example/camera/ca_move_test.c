#include <roki_gl/roki_glut.h>

GLdouble vertex[][3] = {
  { 0, 0,-2 },
  { 1, 0,-2 },
  { 1, 1,-2 },
  { 0, 1,-2 },
  { 0, 0, 2 },
  { 1, 0, 2 },
  { 1, 1, 2 },
  { 0, 1, 2 },
};

int face[][4] = {
  { 0, 1, 3, 2 },
  { 1, 5, 2, 6 },
  { 5, 4, 6, 7 },
  { 4, 0, 7, 3 },
  { 4, 5, 0, 1 },
  { 3, 2, 7, 6 },
};

GLdouble color[][4] = {
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 },
  { 1, 1, 0 },
  { 1, 0, 1 },
  { 0, 1, 1 },
};

rkglCamera cam;

double r = 0;

void display(void)
{
  int i, j;

  rkglCameraLoadViewframe( &cam );
  glPushMatrix();
  glRotated( r, 0, 1, 0 );
  rkglClear();
  glBegin( GL_TRIANGLE_STRIP );
  for( i=0; i<6; i++ ){
    glColor3dv( color[i] );
    for( j=0; j<4; j++ )
      glVertex3dv( vertex[face[i][j]] );
  }
  glEnd();
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/160, 1, 30 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'j': rkglCameraSetPanTiltRoll( &cam, 5, 0, 0 ); break;
  case 'J': rkglCameraSetPanTiltRoll( &cam,-5, 0, 0 ); break;
  case 'k': rkglCameraSetPanTiltRoll( &cam, 0, 5, 0 ); break;
  case 'K': rkglCameraSetPanTiltRoll( &cam, 0,-5, 0 ); break;
  case 'l': rkglCameraSetPanTiltRoll( &cam, 0, 0, 5 ); break;
  case 'L': rkglCameraSetPanTiltRoll( &cam, 0, 0,-5 ); break;
  case 'u': rkglCameraLockonAndSetPanTiltRoll( &cam, 5, 0, 0 ); break;
  case 'U': rkglCameraLockonAndSetPanTiltRoll( &cam,-5, 0, 0 ); break;
  case 'i': rkglCameraLockonAndSetPanTiltRoll( &cam, 0, 5, 0 ); break;
  case 'I': rkglCameraLockonAndSetPanTiltRoll( &cam, 0,-5, 0 ); break;
  case 'o': rkglCameraLockonAndSetPanTiltRoll( &cam, 0, 0, 5 ); break;
  case 'O': rkglCameraLockonAndSetPanTiltRoll( &cam, 0, 0,-5 ); break;
  case 'n': rkglCameraMove( &cam, 0.05, 0, 0 ); break;
  case 'N': rkglCameraMove( &cam,-0.05, 0, 0 ); break;
  case 'm': rkglCameraMove( &cam, 0, 0.05, 0 ); break;
  case 'M': rkglCameraMove( &cam, 0,-0.05, 0 ); break;
  case ',': rkglCameraMove( &cam, 0, 0, 0.05 ); break;
  case '<': rkglCameraMove( &cam, 0, 0,-0.05 ); break;
  case '8': rkglCameraRelMove( &cam, 0.05, 0, 0 ); break;
  case '*': rkglCameraRelMove( &cam,-0.05, 0, 0 ); break;
  case '9': rkglCameraRelMove( &cam, 0, 0.05, 0 ); break;
  case '(': rkglCameraRelMove( &cam, 0,-0.05, 0 ); break;
  case '0': rkglCameraRelMove( &cam, 0, 0, 0.05 ); break;
  case ')': rkglCameraRelMove( &cam, 0, 0,-0.05 ); break;
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  glCullFace( GL_FRONT );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 6, 0, 3, 0, -30, 0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 240, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
