#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

#define NAME_NURBS 100
#define NAME_OTHER 200

#define NUM_NURBS 4
#define NUM_CP    6
#define SIZE_CP 10.0

zNURBS3D nurbs[NUM_NURBS];
int selected_curve = -1;
int selected_cp = -1;

rkglSelectionBuffer sb;

void draw_scene(void)
{
  zRGB rgb;
  int i;

  glPushMatrix();
  glLoadName( NAME_OTHER );
  rkglFrame( ZFRAME3DIDENT, 1.0, 2 );
  for( i=0; i<NUM_NURBS; i++ ){
    zRGBSet( &rgb, 1.0 * ( ~i & 1 ) , 1.0 * ( ~i & 2 ), 1.0 * ( ~i & 4 ) );
    glLoadName( NAME_NURBS + i );
    glLineWidth( 3 );
    rkglNURBSCurve( &nurbs[i], &rgb );
    zRGBSet( &rgb, 0.5, 1.0, 0.5 );
    glLineWidth( 1 );
    rkglNURBSCurveCP( &nurbs[i], SIZE_CP, &rgb );
  }
  glPopMatrix();
}

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

void init_curve(void)
{
  int i, j;

  for( i=0; i<NUM_NURBS; i++ ){
    for( j=0; j<zNURBS3D1CPNum(&nurbs[i]); j++ ){
      zVec3DCreate( zNURBS3D1CP(&nurbs[i],j), zRandF(-1.0,1.0), zRandF(-1.0,1.0), zRandF(-1.0,1.0) );
    }
  }
}

void keyboard(unsigned char key, int x, int y)
{
  int i;

  switch( key ){
  case 'q':
    for( i=0; i<NUM_NURBS; i++ )
      zNURBS3DDestroy( &nurbs[i] );
    exit( 1 );
    break;
  case 'i':
    init_curve();
    glutPostRedisplay();
    break;
  default: ;
  }
}

bool find_cp(rkglSelectionBuffer *sb)
{
  int i, selected_curve_tmp = -1;

  rkglSelectionRewind( sb );
  selected_curve = -1;
  selected_cp = -1;
  for( i=0; i<rkglSelectionHits(sb); i++ ){
    selected_curve_tmp = rkglSelectionName(sb,0) - NAME_NURBS;
    if( selected_curve_tmp >= 0 && selected_curve_tmp < NUM_NURBS ){
      if( rkglSelectionName(sb,1) >= 0 && rkglSelectionName(sb,1) < zNURBS3D1CPNum(&nurbs[selected_curve_tmp]) ){
        selected_curve = selected_curve_tmp;
        selected_cp = rkglSelectionName(sb,1);
        break;
      }
    }
    rkglSelectionNext( sb );
  }
  return selected_curve >=0 && selected_cp >= 0;
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionInit( &sb );
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, SIZE_CP, SIZE_CP );
      if( find_cp( &sb ) )
        eprintf( "Selected curve #%d & control point [%d]\n", selected_curve, selected_cp );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
  if( rkglSelectionNoHits(&sb) )
    rkglMouseFuncGLUT( button, state, x, y );
}

void motion(int x, int y)
{
  zVec3D p;

  if( rkglSelectionNoHits(&sb) ){
    rkglMouseDragFuncGLUT( x, y );
    return;
  }
  if( selected_curve >= 0 && selected_cp >= 0 ){
    rkglUnproject( &cam, x, y, rkglSelectionZnearDepth(&sb), &p );
    zVec3DCopy( &p, zNURBS3D1CP(&nurbs[selected_curve],selected_cp) );
  }
}

void init(void)
{
  int i;

  zRandInit();
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.0, 0.0, 0.0 );
  rkglCameraLookAt( &cam, 5, 0, 1, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 45, 0.5, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );

  for( i=0; i<NUM_NURBS; i++ ){
    zNURBS3D1Alloc( &nurbs[i], NUM_CP, 3 );
    zNURBS3D1SetSlice( &nurbs[i], 50 );
  }
  init_curve();
}

int main(int argc, char **argv)
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 960, 680, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutKeyboardFunc( keyboard );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
