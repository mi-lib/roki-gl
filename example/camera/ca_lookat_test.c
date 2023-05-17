#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>

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
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 },
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
double eyex, eyey, eyez, centerx, centery, centerz;

void display(void)
{
  int i, j;

  rkglCALoad( &cam );
  glPushMatrix();
  glRotated( r, 0, 1, 0 );
  rkglClear();
  glBegin( GL_QUADS );
  for( i=0; i<6; i++ ){
    glColor3dv( color[i] );
    for( j=0; j<4; j++ )
      glVertex3dv( vertex[face[i][j]] );
  }
  glEnd();
  glPopMatrix();
  glutSwapBuffers();
}

void idle(void)
{
  glutPostRedisplay();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 1, 10 );
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
  case ' ':
    r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  rkglCALookAt( &cam, eyex, eyey, eyez, centerx, centery, centerz, 0, 0, 1 );
}

void init(void)
{
  glCullFace( GL_FRONT );
  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  eyex = 6; eyey = 0; eyez = 3;
  centerx = centery = centerz = 0;
  rkglCALookAt( &cam, eyex, eyey, eyez, centerx, centery, centerz, 0, 0, 1 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 240, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
