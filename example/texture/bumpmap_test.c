#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glut.h>
#include <zx11/zximage.h>

rkglCamera cam;
rkglLight light;

zMShape3D ms;
zTexture bump;

static void init(char *filename)
{
  rkglSetCallbackParamGLUT( &cam, 2.0, 1, 100, 1.0, 5.0 );
  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 3, 0, 3, 0, -45, 0 );
  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 20, 0, 10 );

  rkglTextureBumpEnable();
  zMShape3DReadZTK( &ms, "bump_test.ztk" );
  glDisable( GL_CULL_FACE );
}

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglMShape( &ms, RKGL_FACE, &light );
  glPopMatrix();
  glutSwapBuffers();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutVisibilityFunc( rkglVisFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  init( argc > 1 ? argv[1] : "bump.bmp" );
  glutMainLoop();
  return 0;
}
