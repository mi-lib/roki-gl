#include <roki_gl/roki_glut.h>
#include <zx11/zximage.h>

rkglCamera cam;
rkglLight light;

zMShape3D ms;
zTexture bump;

static void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 6, 0, 3, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30.0, 1, 100 );
  rkglSetDefaultCamera( &cam );
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
  rkglCameraPut( &cam );
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
  init();
  glutMainLoop();
  return 0;
}
