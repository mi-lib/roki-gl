#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

#define NUM_CHAIN 5
rkChain chain[NUM_CHAIN];
rkglChain gc[NUM_CHAIN];

void draw_scene(void)
{
  int i;

  for( i=0; i<NUM_CHAIN; i++ ){
    rkglChainSetName( &gc[i], i );
    rkglChainDraw( &gc[i] );
  }
}

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static int selected_chain = -1;
static int selected_link = -1;

void reset_link(void)
{
  if( selected_chain < 0 || selected_link < 0 ) return;
  rkglChainResetLinkOptic( &gc[selected_chain], selected_link );
  selected_chain = -1;
  selected_link = -1;
}

void select_link(rkglSelectionBuffer *sb)
{
  zOpticalInfo oi_alt;
  int i;

  reset_link();
  for( i=0; i<NUM_CHAIN; i++ ){
    if( ( selected_link = rkglChainLinkFindSelected( &gc[i], sb ) ) >= 0 ){
      selected_chain = i;
      break;
    }
  }
  if( selected_chain < 0 ) return;
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  gc[selected_chain].attr.disptype = RKGL_FACE;
  rkglChainAlternateLinkOptic( &gc[selected_chain], selected_link, &oi_alt, &light );
}

void move_link(double angle)
{
  double dis;

  if( selected_chain < 0 || selected_link < 0 ) return;
  rkChainLinkJointGetDis( &chain[selected_chain], selected_link, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &chain[selected_chain], selected_link, &dis );
  rkChainUpdateFK( &chain[selected_chain] );
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionBuffer sb;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      if( rkglSelectNearest( &sb, &cam, draw_scene, x, y, 1, 1 ) ){
        select_link( &sb );
      } else{
        reset_link();
      }
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if( state == GLUT_DOWN )
      reset_link();
    break;
  default: ;
  }
}

void keyboard(unsigned char key, int x, int y)
{
  int i;

  switch( key ){
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    for( i=0; i<NUM_CHAIN; i++ ){
      rkglChainUnload( &gc[i] );
      rkChainDestroy( &chain[i] );
    }
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  int i;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 1, 1, 1, 45, -30, 0 );
  rkglCameraFitPerspective( &cam, 30.0, 1.0, 20.0 );
  rkglSetDefaultCamera( &cam );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  for( i=0; i<NUM_CHAIN; i++ ){
    rkChainReadZTK( &chain[i], "../model/puma.ztk" );
    rkglChainLoad( &gc[i], &chain[i], NULL, &light );
    zVec3DCreate( rkChainLinkOrgPos(&chain[i],0), 0, 0.3*i-0.9, 0 );
    rkChainUpdateFK( &chain[i] );
  }
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutKeyboardFunc( keyboard );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
