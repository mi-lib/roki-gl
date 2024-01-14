#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

#define NUM_CHAIN 5
rkChain chain[NUM_CHAIN];
rkglChain gr[NUM_CHAIN];

void draw_scene(void)
{
  int i;

  for( i=0; i<NUM_CHAIN; i++ ){
    rkglChainSetName( &gr[i], i );
    rkglChainDraw( &gr[i] );
  }
}

void display(void)
{
  rkglCALoad( &cam );
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
  rkglChainLinkReset( &gr[selected_chain], selected_link );
  selected_chain = -1;
  selected_link = -1;
}

void select_link(rkglSelectionBuffer *sb)
{
  zOpticalInfo oi_alt;
  int i;

  reset_link();
  for( i=0; i<NUM_CHAIN; i++ ){
    if( ( selected_link = rkglChainLinkFindSelected( &gr[i], sb ) ) >= 0 ){
      selected_chain = i;
      break;
    }
  }
  if( selected_chain < 0 ) return;
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  gr[selected_chain].attr.disptype = RKGL_FACE;
  rkglChainLinkAlt( &gr[selected_chain], selected_link, &oi_alt, &gr[selected_chain].attr, &light );
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

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScaleH( &cam, 1.0/1000, 0.5, 10 );
}

void keyboard(unsigned char key, int x, int y)
{
  int i;

  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    for( i=0; i<NUM_CHAIN; i++ ){
      rkglChainUnload( &gr[i] );
      rkChainDestroy( &chain[i] );
    }
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglChainAttr attr;
  int i;

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 1, 1, 1, 45, -30, 0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkglChainAttrInit( &attr );
  for( i=0; i<NUM_CHAIN; i++ ){
    rkChainReadZTK( &chain[i], "../model/puma.ztk" );
    rkglChainLoad( &gr[i], &chain[i], &attr, &light );
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
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
