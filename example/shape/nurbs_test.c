#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zOpticalInfo oi;
zNURBS3D nurbs;
int selected_cp_row = -1;
int selected_cp_col = -1;

rkglSelectionBuffer sb;

#define NAME_NURBS 100
#define NAME_OTHER 200

#define NUM_CP_ROW   6
#define NUM_CP_COL   3
#define SIZE_CP     10.0

bool show_ctl = false;
bool show_wf = false;

void draw_scene(void)
{
  zRGB rgb;

  glPushMatrix();
  rkglMaterial( &oi );
  glLoadName( NAME_OTHER );
  glLineWidth( 2 );
  rkglNURBS( &nurbs, RKGL_FACE | ( show_wf ? 0 : RKGL_WIREFRAME ) );
  glLoadName( NAME_NURBS );
  if( show_ctl ){
    zRGBSet( &rgb, 0.5, 1.0, 0.5 );
    glLineWidth( 1 );
    rkglNURBSCP( &nurbs, SIZE_CP, &rgb );
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

void init_surface(void)
{
  int i, j;

  for( i=0; i<zNURBS3DCPNum(&nurbs,0); i++ )
    for( j=0; j<zNURBS3DCPNum(&nurbs,1); j++ ){
      zNURBS3DCP(&nurbs,i,j)->e[0] = 2 * ( (double)i/zNURBS3DCPNum(&nurbs,0) - 0.5 );
      zNURBS3DCP(&nurbs,i,j)->e[1] = 2 * ( (double)j/zNURBS3DCPNum(&nurbs,1) - 0.5 );
      zNURBS3DCP(&nurbs,i,j)->e[2] =
        ( i == 0 || i == zNURBS3DCPNum(&nurbs,0)-1 || j == 0 || j == zNURBS3DCPNum(&nurbs,1)-1 ) ? 0 : zRandF( -1.0, 1.0 );
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
    init_surface();
    glutPostRedisplay();
    break;
  case 'w':
    show_wf = 1 - show_wf;
    glutPostRedisplay();
    break;
  case 'p':
    show_ctl = 1 - show_ctl;
    glutPostRedisplay();
    break;
  default: ;
  }
}

void find_cp(rkglSelectionBuffer *sb)
{
  int i;

  rkglSelectionRewind( sb );
  selected_cp_row = selected_cp_col = -1;
  for( i=0; i<rkglSelectionHits(sb); i++ ){
    if( rkglSelectionName(sb,0) == NAME_NURBS &&
        rkglSelectionName(sb,1) >= 0 && rkglSelectionName(sb,1) < zNURBS3DCPNum(&nurbs,0) &&
        rkglSelectionName(sb,2) >= 0 && rkglSelectionName(sb,2) < zNURBS3DCPNum(&nurbs,1) ){
      selected_cp_row = rkglSelectionName(sb,1);
      selected_cp_col = rkglSelectionName(sb,2);
      break;
    }
    rkglSelectionNext( sb );
  }
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionInit( &sb );
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, SIZE_CP, SIZE_CP );
      find_cp( &sb );
      if( selected_cp_row >=0 && selected_cp_col >= 0 )
        eprintf( "Selected control point [%d][%d]\n", selected_cp_row, selected_cp_col );
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
  if( selected_cp_row >= 0 && selected_cp_row < zNURBS3DCPNum(&nurbs,0) &&
      selected_cp_col >= 0 && selected_cp_col < zNURBS3DCPNum(&nurbs,1) ){
    rkglUnproject( &cam, x, y, rkglSelectionZnearDepth(&sb), &p );
    zVec3DCopy( &p, zNURBS3DCP(&nurbs,selected_cp_row,selected_cp_col) );
  }
}

void init(void)
{
  zRandInit();
  rkglCameraSetBackground( &cam, 0.0, 0.0, 0.0 );
  rkglCameraLookAt( &cam, 3, 0, 1, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 45, 1, 20 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );

  zNURBS3DAlloc( &nurbs, 6, 6, 3, 3 );
  zNURBS3DSetSlice( &nurbs, 50, 50 );
  init_surface();
}

int main(int argc, char **argv)
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
