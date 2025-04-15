#include <roki_gl/roki_glut.h>

GLdouble vertex[][3] = {
  { 0, 0, 0 },
  { 1, 0, 0 },
  { 1, 1, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 },
  { 1, 0, 1 },
  { 1, 1, 1 },
  { 0, 1, 1 },
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

rkglCamera cam1, cam2;

void draw(rkglCamera *c, double r)
{
  int i, j;

  rkglCameraLoadViewport( c );
  rkglCameraScaleFrustumHeight( c, 1.0/320, 1.0, 30.0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  gluLookAt( 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glRotated( r, 0, 1, 0 );
  rkglClear();
  glBegin( GL_TRIANGLE_STRIP );
  for( i=0; i<6; i++ ){
    glColor3dv( color[i] );
    for( j=0; j<4; j++ )
      glVertex3dv( vertex[face[i][j]] );
  }
  glEnd();
}

void display(void)
{
  static double r = 0;

  draw( &cam1, r );
  draw( &cam2, r+90 );
  glutSwapBuffers();
  if( ( r+=0.1 ) >= 360 ) r = 0;
}

void idle(void)
{
  glutPostRedisplay();
}

void resize(int w, int h)
{
  glViewport( 0, 0, w, h );
  glScissor( 0, 0, w, h );
  glClearColor( 1, 1, 1, 1 );
  rkglClear();
  rkglCameraSetBackground( &cam1, 0.2, 0, 0 );
  rkglCameraSetViewport( &cam1, 10,    10, w-20, h/2-15 );
  rkglCameraSetBackground( &cam2, 0, 0.2, 0 );
  rkglCameraSetViewport( &cam2, 10, h/2+5, w-20, h/2-15 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033': exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  glCullFace( GL_FRONT );
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
