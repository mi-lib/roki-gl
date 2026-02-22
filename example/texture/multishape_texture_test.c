#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zMultiShape3D ms;

void display(void)
{
  rkglClear();
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglMultiShape( &ms, RKGL_FACE, &light );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 10, 0, 3, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30.0, 1, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 3, 10 );

  /* following calls are mandatory. */
  rkglTextureEnable();
  zMultiShape3DReadZTK( &ms, "lena_cube.ztk" );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );
  glutDisplayFunc( display );
  init();
  glutMainLoop();
  return 0;
}
