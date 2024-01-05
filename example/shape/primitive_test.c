#include <roki_gl/roki_glut.h>

zOpticalInfo blue;
zOpticalInfo green;
zOpticalInfo red;
zOpticalInfo yellow;
zOpticalInfo orange;

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

  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
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

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 1, 10 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case '8': rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case '*': rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case '9': rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case 'w': dispswitch = 1 - dispswitch; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c1, c2;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 3, 0, 0 );
  zEllips3DCreateAlign( &el, &c1, 2, 3, 1, 16 );

  zOpticalInfoCreateSimple( &orange, 0.8, 0.4, 0, NULL );
  zVec3DCreate( &c1,-2, 0,-2 );
  zVec3DCreate( &c2,-3, 0, 2 );
  zECyl3DCreate( &ec, &c1, &c2, 3, 1, ZVEC3DY, 16 );

  zOpticalInfoCreateSimple( &blue, 0, 0, 1.0, NULL );
  zVec3DCreate( &c1, 1, -3,-2 );
  zVec3DCreate( &c2,-1, -5, 2 );
  zCapsule3DCreate( &capsule, &c1, &c2, 1.5, 0 );
  zVec3DCreate( &c1, 0.0,  3, 0 );
  zSphere3DCreate( &hemisphere, &c1, 2, 0 );
  zVec3DCreate( &hemisphere_dir, 1, 0.5, 2 );

  zOpticalInfoCreateSimple( &green, 0.0, 0.8, 0, NULL );
  zVec3DCreate( &c1, 2,-3,-2 );
  zVec3DCreate( &c2, 2,-3, 2 );
  zCyl3DCreate( &tube, &c1, &c2, 1, 16 );

  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
