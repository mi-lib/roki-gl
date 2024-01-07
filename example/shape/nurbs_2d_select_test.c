#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zOpticalInfo oi;
zNURBS3D nurbs;

rkglSelectionBuffer sb;

#define NUM_CP   6
#define SIZE_CP 10.0

void draw_scene(void)
{
  zRGB rgb;

  glPushMatrix();
  glLoadName( 200 );
  rkglFrame( ZFRAME3DIDENT, 1.0, 2 );
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  glLoadName( 100 );
  glLineWidth( 3 );
  rkglNURBSCurve( &nurbs, &rgb );
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  glLineWidth( 1 );
  rkglNURBSCurveCP( &nurbs, SIZE_CP, &rgb );
  glPopMatrix();
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

void init_curve(void)
{
  int i;

  for( i=0; i<zNURBS3D1CPNum(&nurbs); i++ ){
    zVec3DCreate( zNURBS3D1CP(&nurbs,i), zRandF(-1.0,1.0), zRandF(-1.0,1.0), zRandF(-1.0,1.0) );
  }
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );
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

void mouse(int button, int state, int x, int y)
{
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, SIZE_CP, SIZE_CP );
      rkglSelectionPrint( &sb );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
  if( sb.hits == 0 )
    rkglMouseFuncGLUT( button, state, x, y );
}

void motion(int x, int y)
{
  if( sb.hits == 0 )
    rkglMouseDragFuncGLUT( x, y );
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
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
