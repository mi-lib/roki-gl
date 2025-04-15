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

  rkglCameraLoadViewframe( &cam );
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
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/160, 1, 10 );
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

  case 'j': rkglCameraLockonAndRotate( &cam, 1, 5, 0, 0 ); break;
  case 'J': rkglCameraLockonAndRotate( &cam, 1,-5, 0, 0 ); break;
  case 'k': rkglCameraLockonAndRotate( &cam, 1, 0, 5, 0 ); break;
  case 'K': rkglCameraLockonAndRotate( &cam, 1, 0,-5, 0 ); break;
  case 'l': rkglCameraLockonAndRotate( &cam, 1, 0, 0, 5 ); break;
  case 'L': rkglCameraLockonAndRotate( &cam, 1, 0, 0,-5 ); break;
  case 'n': rkglCameraRelMove( &cam, 0.05, 0, 0 ); break;
  case 'N': rkglCameraRelMove( &cam,-0.05, 0, 0 ); break;
  case 'm': rkglCameraRelMove( &cam, 0, 0.05, 0 ); break;
  case 'M': rkglCameraRelMove( &cam, 0,-0.05, 0 ); break;
  case ',': rkglCameraRelMove( &cam, 0, 0, 0.05 ); break;
  case '<': rkglCameraRelMove( &cam, 0, 0,-0.05 ); break;
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
