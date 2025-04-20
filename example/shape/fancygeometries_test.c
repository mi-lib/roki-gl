#include <roki_gl/roki_glut.h>

int ax_id, ay_id, g_id, f_id, a_id;
int checker_id;

rkglCamera cam;
rkglLight light;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  glCallList( checker_id );
  glCallList( g_id );
  glCallList( ax_id );
  glCallList( ay_id );
  glCallList( f_id );
  glCallList( a_id );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  zVec3D pc0, pc1, pc2;
  zVec3D bot, vec;
  GLfloat rgba_white[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat rgba_red[4] = { 1.0, 0.0, 0.0, 1.0 };
  zOpticalInfo red, white, yellow;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 20, 0, 5, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );
  rkglFogExp( 0.6, 0.6, 0.6, 0.1 );

  /* checkerboard */
  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );
  zOpticalInfoCreateSimple( &white, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &pc0, -20.0, -50, -1 );
  zVec3DCreate( &pc1,  10.0, -50, -1 );
  zVec3DCreate( &pc2, -20.0,  50, -1 );
  checker_id = rkglBeginList();
  rkglCheckerBoard( &pc0, &pc1, &pc2, 30, 50, &red, &white );
  glEndList();
  /* gauge & axes */
  g_id = rkglBeginList();
  rkglGauge( zX, 3.01, zY, 4.51, 1.0, 0.2, rgba_white );
  glEndList();
  ax_id = rkglBeginList();
  rkglAxis( zX, 3.5, 2.0, rgba_red );
  glEndList();
  ay_id = rkglBeginList();
  rkglAxis( zY, 5.0, 2.0, rgba_red );
  glEndList();
  /* frame */
  f_id = rkglBeginList();
  rkglFrame( ZFRAME3DIDENT, 2, 3 );
  glEndList();
  /* arrow */
  zVec3DCreate( &bot, 0, 0, 0 );
  zVec3DCreate( &vec, -2.0, -1.0, 3.0 );
  zOpticalInfoCreateSimple( &yellow, 0.9, 0.9, 0.3, NULL );
  a_id = rkglBeginList();
    rkglMaterial( &yellow );
    rkglArrow( &bot, &vec, 1.0 );
  glEndList();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
