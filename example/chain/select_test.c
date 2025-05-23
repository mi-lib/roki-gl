#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

rkChain chain;
rkglChain gc;

#define NAME_CHAIN 0

void draw_scene(void)
{
  rkglChainSetName( &gc, NAME_CHAIN );
  rkglChainDraw( &gc );
}

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static int selected_link = -1;

void reset_link(void)
{
  if( selected_link >= 0 ){
    rkglChainResetLinkOptic( &gc, selected_link );
    selected_link = -1;
  }
}

void select_link(rkglSelectionBuffer *sb)
{
  zOpticalInfo oi_alt;

  reset_link();
  if( ( selected_link = rkglChainLinkFindSelected( &gc, sb ) ) < 0 ) return;
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  gc.attr.disptype = RKGL_FACE;
  rkglChainAlternateLinkOptic( &gc, selected_link, &oi_alt, &light );
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
  switch( key ){
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gc );
    rkChainDestroy( &chain );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 1, 1, 1, 45, -30, 0 );
  rkglCameraFitPerspective( &cam, 30.0, 1.0, 20.0 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  rkChainReadZTK( &chain, "../model/puma.ztk" );
  rkglChainLoad( &gc, &chain, NULL, &light );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

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
