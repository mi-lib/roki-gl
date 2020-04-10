#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

int torus;

rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 1, 0, 0 );
  rkglClear();
  glCallList( torus );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 3, 10 );
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
  zVec3D c, d;
  zOpticalInfo white;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &white, 1.6, 1.0, 0.6, NULL );
  zVec3DCreate( &c, 1, 0, 0 );
  zVec3DCreate( &d, 0, 1, 1 );
  torus = rkglBeginList();
  rkglMaterial( &white );
  rkglTorus( &c, &d, 1.0, 0.5, 32, 16, RKGL_FACE );
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
