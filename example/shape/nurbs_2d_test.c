#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zOpticalInfo oi;
zNURBS3D nurbs;

#define NUM_CP 6
#define VMAX 0.0001
zVec3D vel[NUM_CP];

void init_curve(void)
{
  int i;

  for( i=0; i<zNURBS3D1CPNum(&nurbs); i++ ){
    zVec3DCreate( zNURBS3D1CP(&nurbs,i), zRandF(-1.0,1.0), zRandF(-1.0,1.0), zRandF(-1.0,1.0) );
    zVec3DCreate( &vel[i], zRandF(-VMAX,VMAX), zRandF(-VMAX,VMAX), zRandF(-VMAX,VMAX) );
  }
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );
}

void update_curve(void)
{
  static int count = 0;
  int i;

  if( ++count > 1000 ){
    for( i=0; i<NUM_CP; i++ ){
      zVec3DCreate( &vel[i], zRandF(-VMAX,VMAX), zRandF(-VMAX,VMAX), zRandF(-VMAX,VMAX) );
    }
    count = 0;
  }
  for( i=0; i<zNURBS3D1CPNum(&nurbs); i++ ){
    zVec3DAddDRC( zNURBS3D1CP(&nurbs,i), &vel[i] );
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q':
    zNURBS3DDestroy( &nurbs );
    exit( 1 );
    break;
  case 'i':
    init_curve();
    glutPostRedisplay();
    break;
  default: ;
  }
}

void draw_scene(void)
{
  zRGB rgb;

  glPushMatrix();
  rkglFrame( ZFRAME3DIDENT, 1.0, 2 );
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  glLineWidth( 3 );
  rkglNURBSCurve( &nurbs, &rgb );
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  glLineWidth( 1 );
  rkglNURBSCurveCP( &nurbs, 10.0, &rgb );
  glPopMatrix();
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  update_curve();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 45.0, (GLdouble)w/(GLdouble)h, 1.0, 10.0 );
}

void init(void)
{
  zRandInit();
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.0, 0.0, 0.0 );
  rkglCALookAt( &cam, 3, 0, 1, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );

  zNURBS3D1Alloc( &nurbs, NUM_CP, 3 );
  zNURBS3D1SetSliceNum( &nurbs, 50 );
  init_curve();
}

int main(int argc, char **argv)
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
