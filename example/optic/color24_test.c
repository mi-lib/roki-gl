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

unsigned color[] = {
  0xff0000,
  0x00ff00,
  0x0000ff,
  0xffff00,
  0xff00ff,
  0x00ffff,
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
    rkglColor24( color[i] );
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
  case 't': color[0] -=0x010000; break;
  case 'y': color[1] -=0x000100; break;
  case 'u': color[2] -=0x000001; break;
  case 'i': color[3] -=0x010100; break;
  case 'o': color[4] -=0x010001; break;
  case 'p': color[5] -=0x000101; break;
  case 'T': color[0] +=0x010000; break;
  case 'Y': color[1] +=0x000100; break;
  case 'U': color[2] +=0x000001; break;
  case 'I': color[3] +=0x010100; break;
  case 'O': color[4] +=0x010001; break;
  case 'P': color[5] +=0x000101; break;

  case 'j': rkglCALockonRotate( &cam, 1, 5, 0, 0 ); break;
  case 'J': rkglCALockonRotate( &cam, 1,-5, 0, 0 ); break;
  case 'k': rkglCALockonRotate( &cam, 1, 0, 5, 0 ); break;
  case 'K': rkglCALockonRotate( &cam, 1, 0,-5, 0 ); break;
  case 'l': rkglCALockonRotate( &cam, 1, 0, 0, 5 ); break;
  case 'L': rkglCALockonRotate( &cam, 1, 0, 0,-5 ); break;
  case 'n': rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case 'N': rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case 'm': rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case 'M': rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case ',': rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case '<': rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
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
