#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zMShape3D ms;

void display(void)
{
  rkglClear();
  rkglCameraLoadViewframe( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglMShape( &ms, RKGL_FACE, &light );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  rkglSetDefaultCallbackParam( &cam, 2.0, 2, 60, 1.0, 5.0 );

  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 10, 0, 5, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 3, 10 );

  /* following calls are mandatory. */
  rkglTextureEnable();
  zMShape3DReadZTK( &ms, "lena_cube.ztk" );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutVisibilityFunc( rkglVisFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
