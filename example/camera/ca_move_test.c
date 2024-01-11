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

  rkglCALoad( &cam );
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
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScaleH( &cam, 1.0/160, 1, 10 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'j': rkglCAPTR( &cam, 5, 0, 0 ); break;
  case 'J': rkglCAPTR( &cam,-5, 0, 0 ); break;
  case 'k': rkglCAPTR( &cam, 0, 5, 0 ); break;
  case 'K': rkglCAPTR( &cam, 0,-5, 0 ); break;
  case 'l': rkglCAPTR( &cam, 0, 0, 5 ); break;
  case 'L': rkglCAPTR( &cam, 0, 0,-5 ); break;
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case 'n': rkglCAMove( &cam, 0.05, 0, 0 ); break;
  case 'N': rkglCAMove( &cam,-0.05, 0, 0 ); break;
  case 'm': rkglCAMove( &cam, 0, 0.05, 0 ); break;
  case 'M': rkglCAMove( &cam, 0,-0.05, 0 ); break;
  case ',': rkglCAMove( &cam, 0, 0, 0.05 ); break;
  case '<': rkglCAMove( &cam, 0, 0,-0.05 ); break;
  case '8': rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case '*': rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case '9': rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  glCullFace( GL_FRONT );
  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );
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
