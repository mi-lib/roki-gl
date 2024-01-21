#include <roki_gl/roki_glfw.h>

GLFWwindow* g_window;

/* rkglLinkInfo2.pin information */
typedef enum{
  PIN_LOCK_OFF=-1,
  PIN_LOCK_6D,
  PIN_LOCK_POS3D
} pinStatus;

typedef struct{
  pinStatus pin;
  double weight[2]; /* weight of pos, att */
} pinInfo;

typedef struct{
  zFrame3D root;
  zVec q;
  pinInfo *pinfo;
  int display_id;
} keyFrameInfo;
zArrayClass( keyFrameInfoArray, keyFrameInfo );

keyFrameInfoArray g_keyframe;

/* Test Dataset ********************************************************************/
#define TEST_KEYFRAME_NUM 2
#define TEST_JOINT_SIZE 6
#define TEST_LINK_SIZE 7
#define TEST_FRAME_DOF_SIZE 6
/* x, y, z, aax, aay, aaz */
double test_root_frame[TEST_KEYFRAME_NUM][TEST_FRAME_DOF_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { 0.0, 0.5, 0.0,  0.0, 0.0, 0.0 } };
/* q0 ... q5 */
double test_q[TEST_KEYFRAME_NUM][TEST_JOINT_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { zDeg2Rad(45.0), zDeg2Rad(-60.0), zDeg2Rad(-5.0), 0.0, zDeg2Rad(90.0), 0.0 } };
/* pin */
int test_pin[TEST_KEYFRAME_NUM][TEST_LINK_SIZE] =
  { { PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_OFF,
      PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_POS3D },
    { PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_6D,
      PIN_LOCK_POS3D, PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF }};
/* End of Test Dataset *************************************************************/

/* the weight of pink link for IK */
#define IK_PIN_WEIGHT 1.0
#define IK_JOINT_WEIGHT 0.01
#define IK_NO_WEIGHT 0.0

/* the main targets of this sample code */
rkChain g_chain;
rkglChain gr;

/* keyframe_alpha */
double g_keyframe_alpha = 0.3;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
rkglShadow g_shadow;

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;


zNURBS3D g_nurbs;
int g_selected_cp = -1;
rkglSelectionBuffer g_sb;

#define NAME_NURBS 100
#define INTERMEDIATE_CP_NUM 2
#define SIZE_CP 10.0


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

int createPinInfoDisplayList(rkChain* chain, pinInfo pinfo[], double alpha, rkglLight* light)
{
  int i, display_id;
  rkglChain display_gr;
  zOpticalInfo **oi_alt;
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &display_gr, chain, &attr, &g_light ) ){
    ZRUNWARN( "Failed rkglChainLoad(&display_gr)" );
    return -1;
  }
  if( !( oi_alt = zAlloc( zOpticalInfo*, rkChainLinkNum(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( zOpticalInfo, rkChainLinkNum(&g_chain) )." );
    return -1;
  }
  /* pin link color changed */
  for( i=0; i < rkChainLinkNum( chain ); i++ ){
    if( pinfo[i].pin == PIN_LOCK_6D ){
      /* Red */
      oi_alt[i] = zAlloc( zOpticalInfo, 1 );
      zOpticalInfoCreate( oi_alt[i], 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, g_keyframe_alpha, NULL );
    } else if( pinfo[i].pin == PIN_LOCK_POS3D ){
      /* Yellow */
      oi_alt[i] = zAlloc( zOpticalInfo, 1 );
      zOpticalInfoCreate( oi_alt[i], 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, g_keyframe_alpha, NULL );
    } else {
      /* Default */
      oi_alt[i] = NULL;
    }
  } /* end of pin link color changed */

  display_id = rkglChainDrawOpticalAlt( &display_gr, alpha, &oi_alt[0], light);

  for( i=0; i < rkChainLinkNum( chain ); i++ ){
    if( oi_alt[i] ) zOpticalInfoDestroy( oi_alt[i] );
  }
  rkglChainUnload( &display_gr );

  return display_id;
}


/* define as user application */
bool rkglChainLoadKeyframeInfo(rkChain *chain)
{
  int i, j;
  rkglChainAttr attr;

  printf("size of link = %d\n", rkChainLinkNum(chain) );
  printf("size of joint = %d\n", rkChainJointSize(chain) );
  zArrayAlloc( &g_keyframe, keyFrameInfo, TEST_KEYFRAME_NUM );
  rkglChainAttrInit( &attr );
  for( i=0; i < zArraySize( &g_keyframe ); i++ ){
    if( !( g_keyframe.buf[i].pinfo = zAlloc( pinInfo, rkChainLinkNum(chain) ) ) ){
      ZALLOCERROR();
      ZRUNERROR( "Failed to zAlloc( pinInfo, rkChainLinkNum(chain) )." );
      return false;
    }
    for( j=0; j < rkChainLinkNum(chain); j++ ){
      g_keyframe.buf[i].pinfo[j].pin = test_pin[i][j];
      switch( g_keyframe.buf[i].pinfo[j].pin ){
      case PIN_LOCK_6D:
        g_keyframe.buf[i].pinfo[j].weight[0] = IK_PIN_WEIGHT;
        g_keyframe.buf[i].pinfo[j].weight[1] = IK_PIN_WEIGHT;
        break;
      case PIN_LOCK_POS3D:
        g_keyframe.buf[i].pinfo[j].weight[0]  = IK_NO_WEIGHT;
        g_keyframe.buf[i].pinfo[j].weight[1]  = IK_PIN_WEIGHT;
        break;
      case PIN_LOCK_OFF:
        g_keyframe.buf[i].pinfo[j].weight[0]  = IK_NO_WEIGHT;
        g_keyframe.buf[i].pinfo[j].weight[1]  = IK_NO_WEIGHT;
        break;
      default: ;
      }
    }
    for( j=0; j < rkChainJointSize(chain); j++ ){
      printf( "test_q[%d][%d] = %f\n", i, j, test_q[i][j] );
    }
    printf( "\n" );
    if( !( g_keyframe.buf[i].q = zVecCloneArray( test_q[i], rkChainJointSize(chain) ) ) ){
      ZRUNERROR( "Failed to zVecCloneArray( test_q[%d], rkChainJointSize(chain) ).", i );
      return false;
    };
    zFrame3DFromAA( &g_keyframe.buf[i].root,
                    test_root_frame[i][0], test_root_frame[i][1],
                    test_root_frame[i][2], test_root_frame[i][3],
                    test_root_frame[i][4], test_root_frame[i][5] );

    /* rkChainSetRootFrame( chain, &g_keyframe.buf[i].root ); */
    zFrame3DCopy( &g_keyframe.buf[i].root, rkChainOrgFrame(chain) );
    rkChainFK( chain, g_keyframe.buf[i].q );
    g_keyframe.buf[i].display_id = createPinInfoDisplayList( chain, g_keyframe.buf[i].pinfo, g_keyframe_alpha, &g_light );
    if( g_keyframe.buf[i].display_id < 0 ){
      return false;
    }
  } /* end of for( zArraySize( &g_keyframe ) ) */

  return true;
}

void rkglChainUnloadKeyframeInfo()
{
  int i;
  for( i=0; i < zArraySize( &g_keyframe ); i++ ){
    zVecFree( g_keyframe.buf[i].q );
    zFree( g_keyframe.buf[i].pinfo );
  }
  zArrayFree( &g_keyframe );
}


int find_cp(rkglSelectionBuffer *sb, zNURBS3D* nurbs)
{
  int i;

  rkglSelectionRewind( sb );
  g_selected_cp = -1;
  for( i=0; i<sb->hits; i++ ){
    if( rkglSelectionName(sb,0) == NAME_NURBS &&
        rkglSelectionName(sb,1) >= 0 && rkglSelectionName(sb,1) < zNURBS3D1CPNum(nurbs) ){
      g_selected_cp = rkglSelectionName(sb,1);
      break;
    }
    rkglSelectionNext( sb );
  }
  return g_selected_cp;
}


/******************************************************************************************/

void draw_nurbs(void)
{
  zRGB rgb;

  glPushMatrix();
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  glLoadName( NAME_NURBS );
  glLineWidth( 3 );
  rkglNURBSCurve( &g_nurbs, &rgb );
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  glLineWidth( 1 );
  rkglNURBSCurveCP( &g_nurbs, SIZE_CP, &rgb );
  glPopMatrix();
}


void draw_scene(void)
{
  int i;
  for( i=0; i < zArraySize( &g_keyframe ); i++ ){
    glCallList( g_keyframe.buf[i].display_id );
  }
  draw_nurbs();
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();

  draw_scene();
  glfwSwapBuffers( window );
}

void motion(GLFWwindow* window, double x, double y)
{
  zVec3D p;

  if( g_sb.hits == 0 ){
    rkglMouseDragFuncGLFW( window, x, y );
    return;
  }
  if( g_selected_cp >= 0 && g_selected_cp < zNURBS3D1CPNum(&g_nurbs) ){
    rkglUnproject( &g_cam, x, y, rkglSelectionZnearDepth(&g_sb), &p );
    zVec3DCopy( &p, zNURBS3D1CP(&g_nurbs,g_selected_cp) );
  }
  rkglMouseStoreXY( floor(x), floor(y) );
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      rkglSelect( &g_sb, &g_cam, draw_nurbs, rkgl_mouse_x, rkgl_mouse_y, SIZE_CP, SIZE_CP );
      if( find_cp( &g_sb, &g_nurbs ) >= 0 )
        eprintf( "Selected control point [%d]\n", g_selected_cp );
    } else if( state == GLFW_RELEASE ){
      g_sb.hits = 0;
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
    }
  }
  rkglMouseFuncGLFW( window, button, state, mods );
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

void keyboard(GLFWwindow* window, unsigned int key)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &g_cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &g_cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &g_cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &g_cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &g_cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &g_cam, 0, 0,-5 ); break;
  case '8': g_scale += 0.0001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.0001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkglChainUnloadKeyframeInfo();
    rkChainDestroy( &g_chain );

    exit( EXIT_SUCCESS );
  default: ;
  }
}

char *g_modelfile = NULL;

#ifdef __WINDOWS__
#include <direct.h>
#define compat_getcwd(buf,length) ( _getcwd( buf, length ) )
#define compat_chdir(path) ( _chdir( path ) )
#else
#include <unistd.h>
#define compat_getcwd(buf,length) ( getcwd( buf, length ) )
#define compat_chdir(path) ( chdir( path ) )
#endif

int change_dir(char *pathname, char *dirname, char *filename, char *cwd, size_t size)
{
  if( !compat_getcwd( cwd, size ) ){
    ZRUNERROR( "astray from directory" );
    return -1;
  }
  zGetDirFilename( pathname, dirname, filename, size );
  if( *dirname ){
    if( compat_chdir( dirname ) < 0 ){
      ZRUNERROR( "cannot change directory to %s", dirname );
      return -1;
    }
  }
  return 0;
}

int return_dir(char *cwd)
{
  if( compat_chdir( cwd ) < 0 ){
    ZRUNERROR( "cannot change directory to %s", cwd );
    return -1;
  }
  return 0;
}

rkChain *extend_rkChainReadZTK(rkChain *chain, char *pathname)
{
  char dirname[BUFSIZ], filename[BUFSIZ], cwd[BUFSIZ];

  change_dir( pathname, dirname, filename, cwd, BUFSIZ );
  chain = rkChainReadZTK( chain, filename );
  return_dir( cwd );
  return chain;
}

bool init_curve(rkChain *chain)
{
  int i, size;
  zOpticalInfo oi;

  size = zArraySize( &g_keyframe );
  if( size < 2 ) {
    ZRUNERROR( "too smapl keyframe size = %d (must be >= 2)", size );
    return false;
  }
  /* the number of intermediate control points */
  zNURBS3D1Alloc( &g_nurbs, size + INTERMEDIATE_CP_NUM, 3 );
  zNURBS3D1SetSliceNum( &g_nurbs, 50 );

  for( i=0; i < size-1; i++ ){
    /* start */
    zFrame3DCopy( &g_keyframe.buf[i].root, rkChainOrgFrame(chain) );
    rkChainFK( chain, g_keyframe.buf[i].q );
    zVec3D start;
    zVec3DCopy( rkChainLinkWldPos(chain, 6), &start ); /* 6 is test */
    zVec3DCopy( &start, zNURBS3D1CP(&g_nurbs,i) );
    /* end */
    zFrame3DCopy( &g_keyframe.buf[i+1].root, rkChainOrgFrame(chain) );
    rkChainFK( chain, g_keyframe.buf[i+1].q );
    zVec3D end;
    zVec3DCopy( rkChainLinkWldPos(chain, 6), &end );
    zVec3DCopy( &end, zNURBS3D1CP(&g_nurbs, i + INTERMEDIATE_CP_NUM + 1 ) );
    /* itermediate points */
    /* for( j=i+1; j<zNURBS3D1CPNum(&g_nurbs)-INTERMEDIATE_CP_NUM; j++ ){ */
      zVec3DCreate( zNURBS3D1CP(&g_nurbs,i+1),
                    zRandF(start.c.x - 1.0, start.c.x + 1.0),
                    zRandF(start.c.x - 1.0, start.c.x + 1.0),
                    zRandF(start.c.x - 1.0, start.c.x + 1.0) );
      zVec3DCreate( zNURBS3D1CP(&g_nurbs, i + INTERMEDIATE_CP_NUM),
                    zRandF(end.c.x - 1.0, end.c.x + 1.0),
                    zRandF(end.c.x - 1.0, end.c.x + 1.0),
                    zRandF(end.c.x - 1.0, end.c.x + 1.0) );
    /* } */
  }
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );

  return true;
}

bool init(void)
{
  rkglSetDefaultCallbackParam( &g_cam, 1.0, 1.0, 20.0, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 0, 0, 0, 45, -30, 0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 3, 5, 9 );
  rkglLightSetAttenuationConst( &g_light, 1.0 );

  if( g_modelfile == NULL ){
    g_modelfile = zStrClone( "../model/puma.ztk" );
  }
  printf( "modelfile = %s\n", g_modelfile );
  if( !extend_rkChainReadZTK( &g_chain, g_modelfile ) ){
    ZRUNWARN( "Failed extend_rkChainReadZTK()" );
    return false;
  }
  rkglChainAttr attr;
  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &gr, &g_chain, &attr, &g_light ) ){
    ZRUNWARN( "Failed rkglChainLoad(&gr)" );
    return false;
  }
  if( !rkglChainLoadKeyframeInfo( &g_chain ) ){
    ZRUNWARN( "Failed rkglChainLoadKeyframeInfo()" );
    return false;
  }
  /* IK */
  rkChainCreateIK( &g_chain );
  rkChainRegIKJointAll( &g_chain, IK_JOINT_WEIGHT );

  /* NURBS */
  init_curve( &g_chain );
  g_sb.hits = 0;
  return true;
}


int main(int argc, char *argv[])
{
  int width;
  int height;

  if( argc > 1 ){
    g_modelfile = argv[1];
  }
  if( rkglInitGLFW( &argc, argv ) < 0 )
    return 1;
  glfwWindowHint( GLFW_VISIBLE, false );
  width = 640;
  height = 480;
  if( !( g_window = rkglWindowCreateAndOpenGLFW( 0, 0, width, height, argv[0] ) ) )
    return 1;

  glfwSetWindowSizeCallback( g_window, resize );
  glfwSetCursorPosCallback( g_window, motion );
  glfwSetMouseButtonCallback( g_window, mouse );
  glfwSetScrollCallback( g_window, mouse_wheel );
  glfwSetCharCallback( g_window, keyboard );

  if( !init() ){
    glfwTerminate();
    return 1;
  }
  resize( g_window, width, height );
  glfwSwapInterval(1);

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE ){
    display(g_window);
    glfwPollEvents();
  }
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}


