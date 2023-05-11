#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

zOpticalInfo opt;
zShape3D shape;

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
  rkglShape( &shape, NULL, RKGL_FACE, &light );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 1, 100 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'r': if( opt.dif.r > 0.0 ) opt.dif.r -= 0.01; break;
  case 'R': if( opt.dif.r < 1.0 ) opt.dif.r += 0.01; break;
  case 'g': if( opt.dif.g > 0.0 ) opt.dif.g -= 0.01; break;
  case 'G': if( opt.dif.g < 1.0 ) opt.dif.g += 0.01; break;
  case 'b': if( opt.dif.b > 0.0 ) opt.dif.b -= 0.01; break;
  case 'B': if( opt.dif.b < 1.0 ) opt.dif.b += 0.01; break;
  case ' ':
    r += 10; break;
  case 'q': case 'Q': case '\033':
    zShape3DDestroy( &shape );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &opt, 0.8, 0, 0, NULL );
  zShape3DInit( &shape );
  zShape3DBoxCreateAlign( &shape, ZVEC3DZERO, 5, 3, 4 );
  zShape3DSetOptic( &shape, &opt );
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
