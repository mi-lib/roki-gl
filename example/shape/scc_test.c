#include <roki_gl/roki_glut.h>

zOpticalInfo red, cyan, yellow;
zSphere3D sphere[3];
zCyl3D cylinder[3];
zCone3D cone[3];

rkglCamera cam;
rkglLight light;

ubyte dispswitch = 1;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  rkglMaterial( &red );
  rkglSphere( &sphere[0], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglSphere( &sphere[1], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglSphere( &sphere[2], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglMaterial( &cyan );
  rkglCyl( &cylinder[0], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglCyl( &cylinder[1], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglCyl( &cylinder[2], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglMaterial( &yellow );
  rkglCone( &cone[0], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglCone( &cone[1], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  rkglCone( &cone[2], RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) );
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'w': dispswitch = 1 - dispswitch; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c1, c2;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 15, 0, 5, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 100 );
  rkglSetDefaultCamera( &cam );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  /* course */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1,-2, 0, 0 );
  zSphere3DCreate( &sphere[0], &c1, 1, 8 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1,-2, 2,-1.5 );
  zVec3DCreate( &c2,-2, 1, 1.5 );
  zCyl3DCreate( &cylinder[0], &c1, &c2, 0.5, 8 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1,-2,-2,-2 );
  zVec3DCreate( &c2,-2,-1, 1 );
  zCone3DCreate( &cone[0], &c1, &c2, 1, 8 );

  /* middle */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 0, 0, 0 );
  zSphere3DCreate( &sphere[1], &c1, 1, 16 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 0, 2,-1.5 );
  zVec3DCreate( &c2, 0, 1, 1.5 );
  zCyl3DCreate( &cylinder[1], &c1, &c2, 0.5, 16 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 0,-2,-2 );
  zVec3DCreate( &c2, 0,-1, 1 );
  zCone3DCreate( &cone[1], &c1, &c2, 1, 16 );

  /* fine */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 2, 0, 0 );
  zSphere3DCreate( &sphere[2], &c1, 1, 0 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 2, 2,-1.5 );
  zVec3DCreate( &c2, 2, 1, 1.5 );
  zCyl3DCreate( &cylinder[2], &c1, &c2, 0.5, 0 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 2,-2,-2 );
  zVec3DCreate( &c2, 2,-1, 1 );
  zCone3DCreate( &cone[2], &c1, &c2, 1, 0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
