#include <roki_gl/roki_glfw.h>

rkglCamera g_cam;
rkglLight g_light;

rkChain g_chain;
rkglChain gr;
int *g_display_id_list;

#define GHOST_NUM 15
#define PAINT_LINK_ID 3
#define MOVE_LINK01_ID 1
#define MOVE_LINK02_ID 3
#define MOVE_LINK03_ID 4

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;


/* This is suggstion code for rkgl_chain.c **************************************/
void rkglChainLinkDrawOpticalAlt(rkglChain *gc, int id, double alpha, zOpticalInfo *oi_alt, rkglLight *light)
{
  zShapeListCell *sp;
  rkLink *link;
  double org_alpha;
  zOpticalInfo *oi;
  zShape3D *s;
  link = rkChainLink(gc->chain,id);
  if( !gc->info[id].visible || rkLinkShapeIsEmpty( link ) ) return;
  glPushMatrix();
  rkglXform( rkLinkWldFrame(link) );
  zListForEach( rkLinkShapeList(link), sp ){
    s = zShapeListCellShape(sp);
    oi = zShape3DOptic(s);
    org_alpha = oi->alpha;
    oi->alpha = alpha;
    rkglShape( s, oi_alt, RKGL_FACE, light );
    oi->alpha = org_alpha;
  }
  glPopMatrix();
}

int rkglChainDrawOpticalAlt(rkglChain *gc, double alpha, zOpticalInfo *oi_alt[], rkglLight *light)
{
  int i, result;

  result = rkglBeginList();
  for( i=0; i<rkChainLinkNum(gc->chain); i++ )
    rkglChainLinkDrawOpticalAlt( gc, i, alpha, oi_alt[i], light );
  glEndList();
  return result;
}

int rkglChainCreateGhostDisplay(rkChain* chain, double alpha, zOpticalInfo **oi_alt, rkglLight* light)
{
  int i, display_id;
  rkglChain display_gr;
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &display_gr, chain, &attr, light ) ){
    ZRUNWARN( "Failed rkglChainLoad(&display_gr)" );
    return -1;
  }
  display_id = rkglChainDrawOpticalAlt( &display_gr, alpha, &oi_alt[0], light);
  for( i=0; i < rkChainLinkNum( chain ); i++ ){
    if( oi_alt[i] ) zOpticalInfoDestroy( oi_alt[i] );
  }
  rkglChainUnload( &display_gr );

  return display_id;
}
/* end of suggstion code for rkgl_chain.c ***************************************/


/************************************************/


void draw_scene(void)
{
  int i;
  for( i=0; i < GHOST_NUM; i++ ){
    glCallList( g_display_id_list[i] );
  }
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();

  draw_scene();

  glfwSwapBuffers( window );
}

int createDisplayList(rkChain* chain, double alpha){
  int display_id;

  /* change color of a link */
  int i;
  zOpticalInfo **oi_alt;
  if( !( oi_alt = zAlloc( zOpticalInfo*, rkChainLinkNum(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( zOpticalInfo, rkChainLinkNum(&g_chain) )." );
    return -1;
  }
  for( i=0; i<rkChainLinkNum(chain); i++ ){
    if( i == PAINT_LINK_ID ){
      oi_alt[i] = zAlloc( zOpticalInfo, 1 );
      /* Red */
      zOpticalInfoCreate( oi_alt[i], 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
    } else {
      oi_alt[i] = NULL;
    }
  }

  /* main process */
  display_id = rkglChainCreateGhostDisplay( chain, alpha, &oi_alt[0], &g_light);

  for( i=0; i < rkChainLinkNum( chain ); i++ )
    if( oi_alt[i] ) zOpticalInfoDestroy( oi_alt[i] );

  return display_id;
}

int createGhosts(rkChain* chain, int* display_id_list)
{
  int i;
  double alpha;
  for( i=0; i < GHOST_NUM; i++ ){
    alpha = (double)(i+1)/(double)(GHOST_NUM);

    /* generate ghost pose */
    double dis1, dis2, dis3;
    double dis1min, dis1max, dis2min, dis2max, dis3min, dis3max;
    rkLinkJointGetMin( rkChainLink( chain, MOVE_LINK01_ID ), &dis1min );
    rkLinkJointGetMax( rkChainLink( chain, MOVE_LINK01_ID ), &dis1max );
    rkLinkJointGetMin( rkChainLink( chain, MOVE_LINK02_ID ), &dis2min );
    rkLinkJointGetMax( rkChainLink( chain, MOVE_LINK02_ID ), &dis2max );
    rkLinkJointGetMin( rkChainLink( chain, MOVE_LINK03_ID ), &dis3min );
    rkLinkJointGetMax( rkChainLink( chain, MOVE_LINK03_ID ), &dis3max );
    rkChainLinkJointGetDis( chain, MOVE_LINK01_ID, &dis1 );
    rkChainLinkJointGetDis( chain, MOVE_LINK02_ID, &dis2 );
    rkChainLinkJointGetDis( chain, MOVE_LINK03_ID, &dis3 );
    dis1 = dis1min + (dis1max - dis1min)*(double)(i)/(double)(GHOST_NUM);
    dis2 = dis2min + (dis2max - dis2min)*(double)(i)/(double)(GHOST_NUM);
    dis3 = dis3min + (dis3max - dis3min)*(double)(i)/(double)(GHOST_NUM);
    rkChainLinkJointSetDis( chain, MOVE_LINK01_ID, &dis1 );
    rkChainLinkJointSetDis( chain, MOVE_LINK02_ID, &dis2 );
    rkChainLinkJointSetDis( chain, MOVE_LINK03_ID, &dis3 );
    rkChainUpdateFK( chain );

    /* main process */
    display_id_list[i] = createDisplayList( chain, alpha );
  }

  return 0;
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

bool init(void)
{
  rkglChainAttr attr;

  rkglSetDefaultCallbackParam( &g_cam, 1.0, 1.0, 20.0, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 1, 1, 1, 45.0, -30.0, 0.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.8, 0.8, 0.8, 1.0, 1.0, 1.0, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 1, 3, 6 );
  rkglLightSetAttenuationConst( &g_light, 1.0 );

  rkglChainAttrInit( &attr );
  rkChainReadZTK( &g_chain, "../model/puma.ztk" );
  rkglChainLoad( &gr, &g_chain, &attr, &g_light );

  if( !( g_display_id_list = zAlloc( int, GHOST_NUM ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( int, GHOST_NUM )." );
    return false;
  }

  if( createGhosts( &g_chain, g_display_id_list ) < 0 ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to createGhost( &g_chain )." );
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  GLFWwindow* window;
  int width, height;

  if( rkglInitGLFW( &argc, argv ) < 0 )
    return 1;
  glfwWindowHint( GLFW_VISIBLE, false );
  width = 640;
  height = 480;
  if( !( window = rkglWindowCreateAndOpenGLFW( 0, 0, width, height, argv[0] ) ) )
    return 1;

  glfwSetWindowSizeCallback( window, resize );
  glfwSetCharCallback( window, rkglCharFuncGLFW );
  glfwSetKeyCallback( window, rkglKeyFuncGLFW );
  glfwSetMouseButtonCallback( window, rkglMouseFuncGLFW );
  glfwSetScrollCallback( window, mouse_wheel );
  glfwSetCursorPosCallback( window, rkglMouseDragFuncGLFW );

  if( !init() ){
    glfwTerminate();
    return 1;
  }
  resize( window, width, height );
  glfwSwapInterval( 1 );
  while( glfwWindowShouldClose( window ) == GL_FALSE ){
    display( window );
    glfwPollEvents();
  }
  glfwDestroyWindow( window );
  glfwTerminate();
  return 0;
}


