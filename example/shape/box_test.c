#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

zOpticalInfo red, blue;
zBox3D box1, box2;

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
  rkglMaterial( &red );
  rkglBox( &box1, RKGL_FACE );
  rkglMaterial( &blue );
  rkglBox( &box2, RKGL_FACE );
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
  case ' ':
    r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c1, c2;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zOpticalInfoCreateSimple( &blue, 0, 0, 0.8, NULL );
  zVec3DCreate( &c1, 0, 1, 0 );
  zVec3DCreate( &c2, 0,-1,-1 );
  zBox3DCreateAlign( &box1, &c1, 4, 0.5, 1.5 );
  zBox3DCreateAlign( &box2, &c2, 4, 1.0, 1.0 );
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
