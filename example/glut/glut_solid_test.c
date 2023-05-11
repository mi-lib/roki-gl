#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_shape.h>
#include <signal.h>
#include <GL/freeglut_ext.h>

enum{
  TEST_CUBE = 0,
  TEST_SPHERE,
  TEST_CONE,
  TEST_TORUS,
  TEST_DODECAHEDRON,
  TEST_OCTAHEDRON,
  TEST_TETRAHEDRON,
  TEST_ICOSAHEDRON,
  TEST_TEAPOT,
  TEST_CYLINDER,
  TEST_RHOMBICDODECAHEDRON,
  TEST_SIERPINSKISPONGE,
  TEST_NUM,
};

bool sw[TEST_NUM];
rkglCamera cam;
rkglLight light;

void keyfunc(unsigned char key, int x, int y)
{
  int val=-1;

  switch( key ){
  case 'a': val = TEST_CUBE;                break;
  case 'b': val = TEST_SPHERE;              break;
  case 'c': val = TEST_CONE;                break;
  case 'd': val = TEST_TORUS;               break;
  case 'e': val = TEST_DODECAHEDRON;        break;
  case 'f': val = TEST_OCTAHEDRON;          break;
  case 'g': val = TEST_TETRAHEDRON;         break;
  case 'h': val = TEST_ICOSAHEDRON;         break;
  case 'i': val = TEST_TEAPOT;              break;
  case 'j': val = TEST_CYLINDER;            break;
  case 'k': val = TEST_RHOMBICDODECAHEDRON; break;
  case 'l': val = TEST_SIERPINSKISPONGE;    break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  if( val>=0 && val<TEST_NUM ) sw[val] = 1 - sw[val];
  glutPostRedisplay();
}

void display(void)
{
  GLdouble offset[] = { 0.0, 0.0, 0.0 };
  zOpticalInfo oi;

  zOpticalInfoCreateSimple( &oi, 0.2, 0.4, 0.9, NULL );
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();

  rkglMaterial( &oi );
  if( sw[TEST_CUBE] )         glutSolidCube( 2 );
  if( sw[TEST_SPHERE] )       glutSolidSphere( 2, 100, 100 );
  if( sw[TEST_CONE] )         glutSolidCone( -1, 2, 100, 1 );
  if( sw[TEST_TORUS] )        glutSolidTorus( 0.5, 1, 100, 100 );
  if( sw[TEST_DODECAHEDRON] ) glutSolidDodecahedron();
  if( sw[TEST_OCTAHEDRON] )   glutSolidOctahedron();
  if( sw[TEST_TETRAHEDRON] )  glutSolidTetrahedron();
  if( sw[TEST_ICOSAHEDRON] )  glutSolidIcosahedron();
  if( sw[TEST_TEAPOT] )       glutSolidTeapot( 1 );
  if( sw[TEST_CYLINDER] )     glutSolidCylinder( 0.5, 2, 100, 1 );
  if( sw[TEST_RHOMBICDODECAHEDRON] ) glutSolidRhombicDodecahedron();
  if( sw[TEST_SIERPINSKISPONGE] )    glutSolidSierpinskiSponge( 5, offset, 3 );

  glPopMatrix();
  glutSwapBuffers();
}

void init()
{
  rkglSetCallbackParamGLUT( &cam, 1.0, 1, 20, 1.0, 5.0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 0, 0, 0, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 10, 10 );

  memset( sw, 0, sizeof(sw) );
  zRandInit();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 500, 500, argv[0] );

  glutDisplayFunc( display );
  glutVisibilityFunc( rkglVisFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyfunc );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();

  printf( "a: cube\n" );
  printf( "b: sphere\n" );
  printf( "c: cone\n" );
  printf( "d: torus\n" );
  printf( "e: dodecahedron\n" );
  printf( "f: octahedron\n" );
  printf( "g: tetrahedron\n" );
  printf( "h: icosahedron\n" );
  printf( "i: teapot\n" );
  printf( "j: cylinder\n" );
  printf( "k: rhombicdodecahedron\n" );
  printf( "l: sierpinskisponge\n" );

  glutMainLoop();
  return 0;
}
