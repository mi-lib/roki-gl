#include <roki_gl/roki_glut.h>

zOpticalInfo opt;
zShape3D shape;

rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCameraLoadViewframe( &cam );
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
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/160, 1, 100 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'r': if( opt.diffuse.r > 0.0 ) opt.diffuse.r -= 0.01; break;
  case 'R': if( opt.diffuse.r < 1.0 ) opt.diffuse.r += 0.01; break;
  case 'g': if( opt.diffuse.g > 0.0 ) opt.diffuse.g -= 0.01; break;
  case 'G': if( opt.diffuse.g < 1.0 ) opt.diffuse.g += 0.01; break;
  case 'b': if( opt.diffuse.b > 0.0 ) opt.diffuse.b -= 0.01; break;
  case 'B': if( opt.diffuse.b < 1.0 ) opt.diffuse.b += 0.01; break;
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    zShape3DDestroy( &shape );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 6, 0, 3, 0, -30, 0 );

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
