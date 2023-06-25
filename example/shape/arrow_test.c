#include <roki_gl/roki_glut.h>

int a_id;
rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 0, 1 );
  rkglClear();
  glCallList( a_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 0.005, 1, 20 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u':
    rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U':
    rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i':
    rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I':
    rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o':
    rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O':
    rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case '8':
    rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case '*':
    rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case '9':
    rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case '(':
    rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case '0':
    rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case ')':
    rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case ' ':
    r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D bot, vec;
  zOpticalInfo oi;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.3, 0.3, 0.3 );
  rkglCASet( &cam, 5, 0, 0, 0, 0, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 8, 0, 8 );

  zVec3DCreate( &bot, 0, 0, 0 );
  zVec3DCreate( &vec, 2.0, 2.0, 2.0 );
  zOpticalInfoCreateSimple( &oi, 0.9, 0.9, 0.3, NULL );
  a_id = rkglBeginList();
    rkglMaterial( &oi );
    rkglArrow( &bot, &vec, 1.0 );
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
