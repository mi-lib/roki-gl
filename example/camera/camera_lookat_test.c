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

double eyex, eyey, eyez, centerx, centery, centerz;

void display(void)
{
  int i, j;

  rkglCameraPut( &cam );
  glPushMatrix();
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
  case 'h': eyey -= 0.5; break;
  case 'j': eyez -= 0.5; break;
  case 'k': eyez += 0.5; break;
  case 'l': eyey += 0.5; break;
  case 'H': centery -= 0.5; break;
  case 'J': centerz -= 0.5; break;
  case 'K': centerz += 0.5; break;
  case 'L': centery += 0.5; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  rkglCameraLookAt( &cam, eyex, eyey, eyez, centerx, centery, centerz, 0, 0, 1 );
}

void init(void)
{
  glCullFace( GL_FRONT );
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  eyex = 6; eyey = 0; eyez = 3;
  centerx = centery = centerz = 0;
  rkglCameraLookAt( &cam, eyex, eyey, eyez, centerx, centery, centerz, 0, 0, 1 );
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
