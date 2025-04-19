#include <roki_gl/roki_glut.h>

rkglCameraArray cameraarray;
rkglCamera *cam;
rkglLight light;

GLint obj_id;

void display(void)
{
  rkglCameraPut( cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  glCallList( obj_id );
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c0, c1, c2;
  zOpticalInfo red, cyan, yellow, white;
  zSphere3D sphere;
  zCyl3D cylinder;
  zCone3D cone;

  /* camera */
  rkglCameraArrayReadZTK( &cameraarray, "camera.ztk" );
  cam = zArrayElem( &cameraarray, 0 );
  rkglCameraArrayFPrintZTK( stdout, &cameraarray );
  rkglSetDefaultCamera( cam );
  /* light */
  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );
  /* color */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zOpticalInfoCreateSimple( &white, 1.0, 1.0, 1.0, NULL );
  /* sphere */
  zVec3DCreate( &c1, 0, 0, 1 );
  zSphere3DCreate( &sphere, &c1, 1, 16 );
  /* cylinder */
  zVec3DCreate( &c1, 0, 2,-0.5 );
  zVec3DCreate( &c2, 0, 1, 2.5 );
  zCyl3DCreate( &cylinder, &c1, &c2, 0.5, 16 );
  /* cone */
  zVec3DCreate( &c1, 0,-2.5,-0.5 );
  zVec3DCreate( &c2, 0,-0.5, 2 );
  zCone3DCreate( &cone, &c1, &c2, 1, 16 );
  /* objects */
  glLineWidth( 2 );
  obj_id = rkglBeginList();
  rkglMaterial( &red );
  rkglSphere( &sphere, RKGL_FACE | RKGL_WIREFRAME );
  rkglMaterial( &cyan );
  rkglCyl( &cylinder, RKGL_FACE | RKGL_WIREFRAME );
  rkglMaterial( &yellow );
  rkglCone( &cone, RKGL_FACE | RKGL_WIREFRAME );
  /* checkerboard */
  zVec3DCreate( &c0, -20.0, -50, -1 );
  zVec3DCreate( &c1,  10.0, -50, -1 );
  zVec3DCreate( &c2, -20.0,  50, -1 );
  rkglCheckerBoard( &c0, &c1, &c2, 30, 50, &red, &white );
  glEndList();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
