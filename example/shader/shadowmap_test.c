#include <roki_gl/roki_glut.h>

int room_id;

rkglCamera cam;
rkglLight light;
rkglShadow shadow;

void draw(void)
{
  static zVec3D cs0 = { { 0.0, 0.0, 4.0 } };
  static zVec3D cb0 = { { 0.0, 0.0, 3.0 } };
  static double theta_s = 0, theta_b = 0;
  zOpticalInfo oi;
  zSphere3D sphere;
  zBox3D box;

  zSphere3DCreate( &sphere, &cs0, 1.5, 16 );
  zVec3DCreate( zSphere3DCenter(&sphere), cs0.c.x + 3.0 * cos(theta_s), cs0.c.y + 3.0 * sin(theta_s), cs0.c.z );
  zOpticalInfoCreateSimple( &oi, 0.0, 1.0, 0.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  theta_s += zDeg2Rad(1.0);

  zBox3DCreateAlign( &box, &cb0, 3.0, 3.0, 6.0 );
  zVec3DCreate( zBox3DCenter(&box), cb0.c.x - 1.0 * cos(theta_b), cb0.c.y - 1.0 * sin(theta_b), cb0.c.z );
  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 0.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  rkglBox( &box, RKGL_FACE );
  theta_b += zDeg2Rad(1.0);

  glCallList( room_id );
}

void display(void)
{
  rkglShadowDrawGLSL( &shadow, &cam, &light, draw );
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    rkglShadowDeleteGLSL( &shadow );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D pc0, pc1, pc2;
  zOpticalInfo oi, oi2;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 1.0, 1.0 );
  rkglCameraFitPerspective( &cam, 90, 1, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.3, 0.3, 0.3, 1.0, 1.0, 1.0, 0, 0, 0 );
  rkglLightMove( &light, 5, -8, 20 );
  rkglLightSetAttenuationConst( &light, 1.0 );

  rkglCameraSetViewframe( &cam, 8, 0, 8, 0, -30, 0 );

  room_id = rkglBeginList();
  zOpticalInfoCreateSimple( &oi,  0.0, 0.0, 1.0, NULL );
  zOpticalInfoCreateSimple( &oi2, 1.0, 0.0, 0.0, NULL );
  zVec3DCreate( &pc0, -30, -30, -3 );
  zVec3DCreate( &pc1,  30, -30, -3 );
  zVec3DCreate( &pc2, -30,  30, -3 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  glEndList();

  rkglShadowInitGLSL( &shadow, 256, 256, 50.0, 0.2, 0.2 );
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
