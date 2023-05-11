#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_chain.h>
#include <roki-gl/rkgl_select.h>

rkglCamera cam;
rkglLight light;

rkChain chain[2];
rkglChain gr[2];

void draw_scene(void)
{
  rkglChainNamedDraw( &gr[0], 0 );
  rkglChainNamedDraw( &gr[1], 1 );
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
  if( selected_chain < 0 || selected_link < 0 )  return;
  rkglChainLinkReset( &gr[selected_chain], selected_link );
  selected_chain = -1;
  selected_link = -1;
}

void select_link(GLuint selbuf[], int hits)
{
  GLuint *ns;
  zOpticalInfo oi_alt;

  reset_link();
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ) return;
  selected_chain = ns[3];
  selected_link  = ns[4];
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
  GLuint selbuf[BUFSIZ];

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN )
      select_link( selbuf, rkglPick( &cam, draw_scene, selbuf, BUFSIZ, x, y, 1, 1 ) );
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
  rkglFrustumScale( &cam, 1.0/1000, 0.5, 10 );
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
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr[0] );
    rkglChainUnload( &gr[1] );
    rkChainDestroy( &chain[0] );
    rkChainDestroy( &chain[1] );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglChainAttr attr;

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 1, 1, 1, 45, -30, 0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkglChainAttrInit( &attr );
  rkChainReadZTK( &chain[0], "../model/puma.ztk" );
  rkglChainLoad( &gr[0], &chain[0], &attr, &light );
  rkChainReadZTK( &chain[1], "../model/puma.ztk" );
  rkglChainLoad( &gr[1], &chain[1], &attr, &light );

  zVec3DCreate( rkChainLinkOrgPos(&chain[1],0), 0, 0.2, 0 );
  rkChainUpdateFK( &chain[1] );
}

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  init();
  glutMainLoop();
  return 0;
}
