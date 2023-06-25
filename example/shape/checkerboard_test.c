#include <roki_gl/roki_glut.h>

zOpticalInfo red, white;
int checker_id;

rkglCamera cam;
rkglLight light;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  glCallList( checker_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 1, 10 );
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
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D pc0, pc1, pc2;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );
  zOpticalInfoCreateSimple( &white, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &pc0, -3.5, -5, 0 );
  zVec3DCreate( &pc1,  3.5, -5, 0 );
  zVec3DCreate( &pc2, -3.5,  5, 0 );
  checker_id = rkglBeginList();
  rkglCheckerBoard( &pc0, &pc1, &pc2, 7, 10, &red, &white );
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
