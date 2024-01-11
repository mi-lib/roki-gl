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

void display(void)
{
  static double r = 0;
  int i, j;

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  gluLookAt( 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glRotated( r, 0, 1, 0 );

  rkglClear();
  glBegin( GL_QUADS );
  for( i=0; i<6; i++ ){
    glColor3dv( color[i] );
    for( j=0; j<4; j++ )
      glVertex3dv( vertex[face[i][j]] );
  }
  glEnd();
  glutSwapBuffers();
  if( ( r+=0.1 ) >= 360 ) r = 0;
}

void idle(void)
{
  glutPostRedisplay();
}

void resize(int w, int h)
{
  glClearColor( 1, 1, 1, 1 );
  glViewport( 0, 0, w, h );
  glScissor( 0, 0, w, h );
  rkglClear();
  rkglBGSet( &cam, 0.3, 0.3, 0.3 );
  rkglVPCreate( &cam, 10, 10, w-20, h-20 );
  rkglFrustumScaleH( &cam, 1.0/320, 1.0, 10.0 );
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
