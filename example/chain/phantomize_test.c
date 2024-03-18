#include <roki_gl/roki_glfw.h>

#define PHANTOM_NUM 15

#define PAINT_LINK_ID 3
#define MOVE_LINK01_ID 1
#define MOVE_LINK02_ID 3
#define MOVE_LINK03_ID 4

rkglCamera g_cam;
rkglLight g_light;

rkChain g_chain;
rkglChain gc;
int g_display_id_list[PHANTOM_NUM];

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

void draw_scene(void)
{
  int i;

  for( i=0; i<PHANTOM_NUM; i++ )
    glCallList( g_display_id_list[i] );
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
  glfwSwapBuffers( window );
}

int create_phantom(double alpha)
{
  int display_id;
  zOpticalInfo red;

  zOpticalInfoCreate( &red, 0.3, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  rkglChainAlternateLinkOptic( &gc, PAINT_LINK_ID, &red, &g_light );
  display_id = rkglBeginList();
  rkglChainPhantomize( &gc, alpha, &g_light );
  glEndList();
  return display_id;
}

void create_phantom_all(void)
{
  int i;
  double dis1, dis2, dis3;
  double dis1min, dis1max, dis2min, dis2max, dis3min, dis3max;

  rkLinkJointGetMin( rkChainLink( &g_chain, MOVE_LINK01_ID ), &dis1min );
  rkLinkJointGetMax( rkChainLink( &g_chain, MOVE_LINK01_ID ), &dis1max );
  rkLinkJointGetMin( rkChainLink( &g_chain, MOVE_LINK02_ID ), &dis2min );
  rkLinkJointGetMax( rkChainLink( &g_chain, MOVE_LINK02_ID ), &dis2max );
  rkLinkJointGetMin( rkChainLink( &g_chain, MOVE_LINK03_ID ), &dis3min );
  rkLinkJointGetMax( rkChainLink( &g_chain, MOVE_LINK03_ID ), &dis3max );
  for( i=0; i<PHANTOM_NUM; i++ ){
    /* generate phantom pose */
    dis1 = dis1min + ( dis1max - dis1min )*(double)i/PHANTOM_NUM;
    dis2 = dis2min + ( dis2max - dis2min )*(double)i/PHANTOM_NUM;
    dis3 = dis3min + ( dis3max - dis3min )*(double)i/PHANTOM_NUM;
    rkChainLinkJointSetDis( &g_chain, MOVE_LINK01_ID, &dis1 );
    rkChainLinkJointSetDis( &g_chain, MOVE_LINK02_ID, &dis2 );
    rkChainLinkJointSetDis( &g_chain, MOVE_LINK03_ID, &dis3 );
    rkChainUpdateFK( &g_chain );
    /* main process */
    g_display_id_list[i] = create_phantom( (double)(i+1)/PHANTOM_NUM );
  }
}

void mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
  if ( yoffset < 0 ) {
    g_scale -= 0.0001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar );
  } else if ( yoffset > 0 ) {
    g_scale += 0.0001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar );
  }
}

void resize(GLFWwindow* window, int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar );
}

void init(void)
{
  rkglSetDefaultCallbackParam( &g_cam, 1.0, 1.0, 20.0, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 1, 1, 1, 45.0, -30.0, 0.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.8, 0.8, 0.8, 1.0, 1.0, 1.0, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 1, 3, 6 );

  rkChainReadZTK( &g_chain, "../model/puma.ztk" );
  rkglChainLoad( &gc, &g_chain, NULL, &g_light );
  create_phantom_all();
}

void terminate(void)
{
  int i;

  for( i=0; i<PHANTOM_NUM; i++ )
    glDeleteLists( g_display_id_list[i], 1 );
  rkglChainUnload( &gc );
  rkChainDestroy( &g_chain );
}

int main(int argc, char *argv[])
{
  GLFWwindow* window;
  const int width = 640, height = 480;

  if( rkglInitGLFW( &argc, argv ) < 0 )
    return 1;
  glfwWindowHint( GLFW_VISIBLE, false );
  if( !( window = rkglWindowCreateAndOpenGLFW( 0, 0, width, height, argv[0] ) ) )
    return 1;

  glfwSetWindowSizeCallback( window, resize );
  glfwSetCharCallback( window, rkglCharFuncGLFW );
  glfwSetKeyCallback( window, rkglKeyFuncGLFW );
  glfwSetMouseButtonCallback( window, rkglMouseFuncGLFW );
  glfwSetScrollCallback( window, mouse_wheel );
  glfwSetCursorPosCallback( window, rkglMouseDragFuncGLFW );

  init();
  resize( window, width, height );
  glfwSwapInterval( 1 );
  while( glfwWindowShouldClose( window ) == GL_FALSE ){
    display( window );
    glfwPollEvents();
  }
  glfwDestroyWindow( window );
  glfwTerminate();
  terminate();
  return 0;
}
