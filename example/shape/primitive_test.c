#include <roki_gl/roki_glut.h>

zOpticalInfo blue;
zOpticalInfo green;
zOpticalInfo red;
zOpticalInfo yellow;
zOpticalInfo orange;

zBox3D box;
zSphere3D hemisphere;
zCapsule3D capsule;
zCyl3D tube;
zVec3D hemisphere_dir;
zEllips3D el;
zECyl3D ec;

rkglCamera cam;
rkglLight light;

ubyte dispswitch = 1;
#define DISPSWITCH ( RKGL_FACE | ( dispswitch ? 0 : RKGL_WIREFRAME ) )

void display(void)
{
  zVec3D c, d;

  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  rkglRGBByStr( "white" );
  rkglMaterial( &red );
  rkglBox( &box, DISPSWITCH );
  rkglMaterial( &yellow );
  rkglEllips( &el, DISPSWITCH );
  rkglMaterial( &orange );
  rkglECyl( &ec, DISPSWITCH );
  rkglMaterial( &blue );
  rkglHemisphere( &hemisphere, &hemisphere_dir, DISPSWITCH );
  rkglCapsule( &capsule, DISPSWITCH );
  rkglMaterial( &green );
  rkglTube( &tube, DISPSWITCH );
  rkglMaterial( &red );
  zVec3DCreate( &c, 1, 0, 3 );
  zVec3DCreate( &d, 0, 1, 3 );
  rkglTorus( &c, &d, 2, 1, 32, 16, DISPSWITCH );
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
  rkglCameraSetViewframe( &cam, 10, 0, 0, 0, 0, 0 );
  rkglCameraFitPerspective( &cam, 90, 1, 100 );
  rkglSetDefaultCamera( &cam );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zOpticalInfoCreateSimple( &orange, 0.8, 0.4, 0, NULL );
  zOpticalInfoCreateSimple( &blue, 0, 0, 1.0, NULL );
  zOpticalInfoCreateSimple( &green, 0.0, 0.8, 0, NULL );
  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );

  zVec3DCreate( &c1, 0, 5, 0 );
  zBox3DCreateAlign( &box, &c1, 3, 1.5, 2.5 );

  zVec3DCreate( &c1, 3, 0, 0 );
  zEllips3DCreateAlign( &el, &c1, 2, 3, 1, 16 );

  zVec3DCreate( &c1,-2, 0,-2 );
  zVec3DCreate( &c2,-3, 0, 2 );
  zECyl3DCreate( &ec, &c1, &c2, 3, 1, ZVEC3DY, 16 );

  zVec3DCreate( &c1, 1, -3,-2 );
  zVec3DCreate( &c2,-1, -5, 2 );
  zCapsule3DCreate( &capsule, &c1, &c2, 1.5, 0 );
  zVec3DCreate( &c1, 0.0,  3, 0 );
  zSphere3DCreate( &hemisphere, &c1, 2, 0 );
  zVec3DCreate( &hemisphere_dir, 1, 0.5, 2 );

  zVec3DCreate( &c1, 2,-3,-2 );
  zVec3DCreate( &c2, 2,-3, 2 );
  zCyl3DCreate( &tube, &c1, &c2, 1, 16 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
