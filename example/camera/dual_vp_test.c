#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>

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

rkglCamera cam1, cam2;

void draw(rkglCamera *c, double r)
{
  int i, j;
  double x, y;

  rkglVPLoad( c );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  y = 1.0 / 640 * rkglVPHeight(c);
  x = y * rkglVPAspect(c);
  /* keep size */
  glFrustum( -x, x, -y, y, 1.0, 10.0 );

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
  glClearColor( 1, 1, 1, 1 );
  glViewport( 0, 0, w, h );
  rkglClear();
  rkglBGSet( &cam1, 0.2, 0, 0 );
  rkglVPCreate( &cam1, 10,    10, w-20, h/2-15 );
  rkglBGSet( &cam2, 0, 0.2, 0 );
  rkglVPCreate( &cam2, 10, h/2+5, w-20, h/2-15 );
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
