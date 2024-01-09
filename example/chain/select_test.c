#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

rkChain chain;
rkglChain gr;

#define NAME_CHAIN 0

void draw_scene(void)
{
  rkglChainSetName( &gr, NAME_CHAIN );
  rkglChainDraw( &gr );
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static int selected_link = -1;

void reset_link(void)
{
  if( selected_link >= 0 ){
    rkglChainLinkReset( &gr, selected_link );
    selected_link = -1;
  }
}

void select_link(rkglSelectionBuffer *sb)
{
  zOpticalInfo oi_alt;

  reset_link();
  selected_link = rkglChainLinkSelect( &gr, sb ); /* simple reference to link name */
  if( selected_link < 0 ) return;
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  gr.attr.disptype = RKGL_FACE;
  rkglChainLinkAlt( &gr, selected_link, &oi_alt, &gr.attr, &light );
}

void move_link(double angle)
{
  double dis;

  if( selected_link < 0 ) return;
  rkChainLinkJointGetDis( &chain, selected_link, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &chain, selected_link, &dis );
  rkChainUpdateFK( &chain );
}

void mouse(int button, int state, int x, int y)
{
  rkglSelectionBuffer sb;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglSelect( &sb, &cam, draw_scene, x, y, 1, 1 );
      select_link( &sb );
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
    rkglChainUnload( &gr );
    rkChainDestroy( &chain );
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
  rkChainReadZTK( &chain, "../model/puma.ztk" );
  rkglChainLoad( &gr, &chain, &attr, &light );
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
