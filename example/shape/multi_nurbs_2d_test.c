#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zOpticalInfo oi;

#define NURBS_NUM 3
zNURBS3D nurbs[NURBS_NUM];
int selected_cp = -1;
int selected_curve = -1;

rkglSelectionBuffer sb;

#define NAME_NURBS 100
#define NAME_OTHER 200

#define NUM_CP   6
#define SIZE_CP 10.0

void draw_scene(void)
{
  int nid;
  zRGB rgb;

  glPushMatrix();
  glLoadName( NAME_OTHER );
  rkglFrame( ZFRAME3DIDENT, 1.0, 2 );
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  for( nid=0; nid<NURBS_NUM; nid++ ){
    zRGBSet( &rgb, 1.0 * ( ~nid & 1 ) , 1.0 * ( ~nid & 2 ), 1.0 * ( ~nid & 4 ) );
    glLoadName( NAME_NURBS + nid );
    glLineWidth( 3 );
    rkglNURBSCurve( &nurbs[nid], &rgb );
    zRGBSet( &rgb, 0.5, 1.0, 0.5 );
    glLineWidth( 1 );
    rkglNURBSCurveCP( &nurbs[nid], SIZE_CP, &rgb );
  }
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
  int nid, i;

  for( nid=0; nid<NURBS_NUM; nid++ ){
    for( i=0; i<zNURBS3D1CPNum(&nurbs[nid]); i++ ){
      zVec3DCreate( zNURBS3D1CP(&nurbs[nid],i), zRandF(-1.0,1.0), zRandF(-1.0,1.0), zRandF(-1.0,1.0) );
    }
  }
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );
}

void keyboard(unsigned char key, int x, int y)
{
  int nid;
  switch( key ){
  case 'q':
    for( nid=0; nid<NURBS_NUM; nid++ )
      zNURBS3DDestroy( &nurbs[nid] );
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
  int i, selected_curve_tmp;
  bool is_selected;

  rkglSelectionRewind( sb );
  selected_cp = -1;
  selected_curve = -1;
  is_selected = false;
  for( i=0; i<sb->hits; i++ ){
    selected_curve_tmp = rkglSelectionName(sb,0) - NAME_NURBS;
    if( selected_curve_tmp >= 0 && selected_curve_tmp < NURBS_NUM ){
      if( rkglSelectionName(sb,1) >= 0 && rkglSelectionName(sb,1) < zNURBS3D1CPNum(&nurbs[selected_curve_tmp]) ){
        selected_curve = selected_curve_tmp;
        selected_cp = rkglSelectionName(sb,1);
        is_selected = true;
        break;
      }
    }
    rkglSelectionNext( sb );
  }
  return is_selected;
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionInit( &sb );
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, SIZE_CP, SIZE_CP );
      if( find_cp( &sb ) )
        eprintf( "Selected NURBS curve [%d] control point [%d]\n", selected_curve, selected_cp );
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
  int nid;
  zVec3D p;

  if( sb.hits == 0 ){
    rkglMouseDragFuncGLUT( x, y );
    return;
  }
  for( nid=0; nid<NURBS_NUM; nid++ ){
    if( selected_cp >= 0 && selected_cp < zNURBS3D1CPNum(&nurbs[nid]) ){
      rkglUnproject( &cam, x, y, rkglSelectionZnearDepth(&sb), &p );
      zVec3DCopy( &p, zNURBS3D1CP(&nurbs[selected_curve],selected_cp) );
    }
  }
}

void reshape(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 45.0, (GLdouble)w/(GLdouble)h, 1.0, 10.0 );
}

void init(void)
{
  int nid;

  zRandInit();
  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.0, 0.0, 0.0 );
  rkglCALookAt( &cam, 3, 0, 1, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );

  for( nid=0; nid<NURBS_NUM; nid++ ){
    zNURBS3D1Alloc( &nurbs[nid], NUM_CP, 3 );
    zNURBS3D1SetSliceNum( &nurbs[nid], 50 );
  }
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
