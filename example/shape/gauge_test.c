#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

int ax_id, ay_id, g_id;
rkglCamera cam;

double r = 0;

void display(void)
{
  rkglCALoad( &cam );

  glPushMatrix();
  glRotated( r, 0, 0, 1 );
  rkglClear();
  glCallList( g_id );
  glCallList( ax_id );
  glCallList( ay_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 0.002, 1, 20 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
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
  GLfloat white[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat red[4] = { 1.0, 0.0, 0.0, 1.0 };

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 5, 0, 2, 0, -20, 0 );

  g_id = rkglBeginList();
  rkglGauge( zX, 1.0, zY, 1.51, 1.0, 0.1, white );
  glEndList();
  ax_id = rkglBeginList();
  rkglAxis( zX, 1.2, 2.0, red );
  glEndList();
  ay_id = rkglBeginList();
  rkglAxis( zY, 1.7, 2.0, red );
  glEndList();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
