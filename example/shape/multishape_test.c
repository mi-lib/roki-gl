#include <roki_gl/roki_glut.h>

int ms_id;
zMultiShape3D ms;
rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 0, 1 );
  rkglClear();
  glCallList( ms_id );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.6, 0.6, 0.6 );
  rkglCameraSetViewframe( &cam, 1, 0, 0.7, 0, -30, 0 );
  rkglCameraFitPerspective( &cam, 30, 0.1, 10.0 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  zMultiShape3DReadZTK( &ms, "../model/scc.ztk" );
  ms_id = rkglEntryMultiShape( &ms, RKGL_FACE, &light );
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
