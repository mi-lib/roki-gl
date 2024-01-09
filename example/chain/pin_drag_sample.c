#include <roki_gl/roki_gl.h>
#include <GLFW/glfw3.h>

GLFWwindow* g_window;

/* suggestion to add rkglLinkInfo rkgl_chain.h ********************************************/
typedef struct{
  int select; /* suggestion info */
  int pin;
  rkIKCell *cell[2];
} rkglLinkInfo2;
/* End of suggestion to add rkglLinkInfo rkgl_chain.h *************************************/

/* rkglLinkInfo2.select information (user defined) */
static const int NOT_SELECTED = -1;
static const int INACTIVE_SELECT = 0;
static const int ACTIVE_SELECT = 1;

/* rkglLinkInfo2.pin information (user defined) */
static const int NOT_PIN_LINK = -1;
static const int PIN_LINK = 0;
static const int ONLY_POS3D_PIN_LINK = 1;

rkglLinkInfo2 *gr_info2;

/* suggestion to add rkglChainLoad() rkgl_chain.c *****************************************/
bool rkglChainLoad_for_rkglLinkInfo2(rkChain *gc_chain)
{
  int i;
  if( !( gr_info2 = zAlloc( rkglLinkInfo2, rkChainLinkNum(gc_chain) ) ) ){
    ZALLOCERROR();
    return false;
  }
  for( i=0; i<rkChainLinkNum(gc_chain); i++ ){
    gr_info2[i].select = -1; /* suggestion code */
    gr_info2[i].pin = -1;
  }
  return true;
}
/* End of suggestion to add rkglChainLoad() rkgl_chain.c **********************************/

void rkglChainUnload_for_rkglLinkInfo2()
{
  zFree( gr_info2 );
}

/* the weight of pink link for IK */
#define IK_PIN_WEIGHT 1.0
/* the weight of drag link for IK */
#define IK_DRAG_WEIGHT 0.01

/* the main targets of this sample code */
rkglFrameHandle g_fh;
rkChain g_chain;
rkglChain gr;

/* mouse selected information */
typedef enum{
  NONE=1,
  LINKFRAME,
  FRAMEHANDLE,
} selectedObj;

/* select object id */
typedef struct{
  selectedObj obj;
  int link_id;
} selectInfo;

selectInfo g_selected;

#define rkglChainLinkIsUnselected(selected_link_id) ( selected_link_id == -1 )

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
rkglShadow g_shadow;

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

/* FrameHandle shape property */
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.5;

/* judge whether the selected shape ID is translation moving parts or not */
bool is_translation_mode(rkglFrameHandle *handle)
{
  /* define the range of translation ID */
  return handle->selected_id >=3 && handle->selected_id <= 5;
}

/* judge whether the selected shape ID is rotation moving parts or not */
bool is_rotation_mode(rkglFrameHandle *handle)
{
  /* define the range of rotation ID */
  return handle->selected_id >=3 && handle->selected_id <= 5;
}

/* To avoid duplication between selected_link and selected_parts_id */
/* NAME_FRAMEHANDLE_OFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NAME_CHAIN 0
#define NAME_FRAMEHANDLE_OFFSET 50

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  if( g_selected.obj != NONE ){
    rkglFrameHandleDraw( &g_fh );
  }
}

void draw_alternate_link(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
    printf( "alternate link : pre gr.info[%d].list = %d, ", id, gc->info[id].list );
    printf( "list_alt = %d, ---> ", gc->info[id].list_alt );
    /* TODO : reuse selected link list value */
    /* (the current implementation generates new list value by glNewList() ) */
    rkglChainSetName( gc, NAME_CHAIN );
    rkglChainLinkAlt( gc, id, oi_alt, attr, light );
    printf( "list = %d, ", gc->info[id].list );
    printf( "list_alt = %d\n", gc->info[id].list_alt );
}

void draw_scene(void)
{
  /* Transparency depends on drawing order */
  /* drawing FrameHandle */
  draw_fh_parts();
  /* drawing link frame */
  zOpticalInfo oi_alt;
  gr.attr.disptype = RKGL_FACE;
  int link_id = g_selected.link_id;
  bool is_alt = ( link_id>=0 && gr.info[link_id].list_alt == -1 );
  int pin = gr_info2[link_id].pin;
  /* re-drawing the selected link once after proccessing with rkglChainLinkReset() */
  if( is_alt && pin == PIN_LINK ){
    /* Red */
    zOpticalInfoCreate( &oi_alt, 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else if( is_alt && pin == ONLY_POS3D_PIN_LINK ){
    /* Green */
    zOpticalInfoCreate( &oi_alt, 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else if( is_alt && g_selected.obj != NONE ){
    /* Blue */
    zOpticalInfoCreate( &oi_alt, 0.5, 0.7, 1.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else{
    rkglChainDraw( &gr );
  }
  /* end of if( g_selected.obj != NONE ) */
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();

  draw_scene();
  glfwSwapBuffers( window );
}

void reset_link_drawing(int new_link_id)
{
  printf( "reset link     : pre gr.info[%d].list = %d, ", new_link_id, gr.info[new_link_id].list );
  printf( "list_alt = %d, ---> ", gr.info[new_link_id].list_alt );
  rkglChainLinkReset( &gr, new_link_id );
  printf( "list = %d, ", gr.info[new_link_id].list );
  printf( "list_alt = %d\n", gr.info[new_link_id].list_alt );
}

void reset_link_select_status(int new_link_id)
{
  if( g_selected.link_id >= 0 ){
    gr_info2[g_selected.link_id].select = INACTIVE_SELECT;
  } else{
    /* This may be redundant */
    gr_info2[g_selected.link_id].select = NOT_SELECTED;
  }
  if( new_link_id >= 0 ){
    if( gr_info2[new_link_id].select == NOT_SELECTED ){
      reset_link_drawing( new_link_id );
    }
    gr_info2[new_link_id].select = ACTIVE_SELECT;
  } else{
    int i;
    printf("reset_link_select_status(): reset all link\n");
    for( i=0; i<rkChainLinkNum(gr.chain); i++ ){
      if( gr_info2[i].select != NOT_SELECTED
          && gr_info2[i].pin == NOT_PIN_LINK ){
        reset_link_drawing( i );
        gr_info2[i].select = NOT_SELECTED;
      }
    }
    /* end of for( i=0; i<rkChainLinkNum(gr.chain); i++ ) */
  }
  /* end of if( new_link_id >= 0 ) else */
}

void reset_selected_link(int new_link_id)
{
  if( new_link_id != g_selected.link_id ){
    g_selected.link_id = new_link_id;
  }
}

int select_link(rkglSelectionBuffer *sb)
{
  /* If LINKFRAME is selected, g_selected.link_id is not changed. (skipped this function) */
  int nearest_shape_id = -1;

  if( !rkglSelectionFindNearest( sb ) ){
    return nearest_shape_id;
  }
  if( rkglSelectionName( sb, 0 ) != 0 ){
    return nearest_shape_id;
  }
  nearest_shape_id = rkglSelectionName( sb, 1 );
  if( nearest_shape_id < 0 || nearest_shape_id >= rkChainLinkNum(gr.chain) ){
    nearest_shape_id = -1;
  }
  return nearest_shape_id;
}

void switch_pin_link(int new_link_id)
{
  rkIKAttr attr;
  int pin = gr_info2[new_link_id].pin;
  if( pin == NOT_PIN_LINK ){
    gr_info2[new_link_id].pin = PIN_LINK;
    /* register rot & pos & weight up */
    attr.id = new_link_id;
    gr_info2[new_link_id].cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
    gr_info2[new_link_id].cell[1] = rkChainRegIKCellWldPos( &g_chain, &attr, RK_IK_ATTR_ID );
    rkIKAttrSetWeight( &attr, IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
  } else if( pin == PIN_LINK ){
    gr_info2[new_link_id].pin = ONLY_POS3D_PIN_LINK;
    /* pos weight up */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    /* un-register rot */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkChainUnregIKCell( &g_chain, gr_info2[new_link_id].cell[0] );
  } else if( pin == ONLY_POS3D_PIN_LINK ){
    gr_info2[new_link_id].pin = NOT_PIN_LINK;
    /* un-register pos */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkChainUnregIKCell( &g_chain, gr_info2[new_link_id].cell[1] );
  }
  /* reset only pin link drawing */
  reset_link_drawing( new_link_id );
  printf("pin_link       : link_id = %d, pin status = PIN_LINK\n", new_link_id );
}

void register_drag_link_for_IK(void)
{
  int link_id = g_selected.link_id;
  int pin = gr_info2[link_id].pin;
  if( pin != PIN_LINK ){
    rkIKAttr attr;
    attr.id = link_id;
    if( is_rotation_mode( &g_fh ) ){
      gr_info2[link_id].cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
      rkIKCellSetWeight( gr_info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    }
    if( pin == NOT_PIN_LINK ){
      gr_info2[link_id].cell[1] = rkChainRegIKCellWldPos( &g_chain, &attr, RK_IK_ATTR_ID );
      rkIKCellSetWeight( gr_info2[link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    }
  }
  /* end of if( pin != PIN_LINK ) */
}

void unregister_drag_link_for_IK(void)
{
  int link_id = g_selected.link_id;
  int pin = gr_info2[link_id].pin;
  if( pin != PIN_LINK ){
    if( is_rotation_mode( &g_fh ) ){
      rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[0] );
    }
    if( pin == NOT_PIN_LINK ){
      rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[1] );
    }
  }
  /* end of if( pin != PIN_LINK ) */
}

/* inverse kinematics */
void update_alljoint_by_IK_with_frame(zFrame3D *ref_frame )
{
  zVec dis;
  dis = zVecAlloc( rkChainJointSize( &g_chain ) );
  rkChainGetJointDisAll(&g_chain, dis);

  int link_id = g_selected.link_id;
  int pin = gr_info2[link_id].pin;

  rkChainDeactivateIK( &g_chain );
  rkChainBindIK( &g_chain );
  /* set reference */
  if( pin == PIN_LINK
      || is_rotation_mode( &g_fh ) ){
    zVec3D zyx;
    zMat3DToZYX( &(ref_frame->att), &zyx );
    rkIKCellSetRefVec( gr_info2[link_id].cell[0], &zyx );
  }
  rkIKCellSetRefVec( gr_info2[link_id].cell[1], &(ref_frame->pos) );
  rkChainFK( &g_chain, dis ); /* copy state to mentatin result consistency */
  /* IK */
  /* printf("pre IK Joint[deg]  = "); */
  /* zVecPrint(zVecMulDRC( zVecClone(dis), 180.0/zPI )); */
  int iter = 100;
  double ztol = zTOL;
  /* backup in case the result of rkChainIK() is NaN */
  rkChain clone_chain;
  rkChainClone( &g_chain, &clone_chain );
  rkChainIK( &g_chain, dis, ztol, iter );
  /* printf("post IK Joint[deg] = "); */
  /* zVecPrint(zVecMulDRC( zVecClone(dis), 180.0/zPI )); */
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_chain );
  }
  unregister_drag_link_for_IK();
  rkChainIK( &g_chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_chain );
  }
  register_drag_link_for_IK();
}

void move_link(double angle)
{
  double dis;

  if( g_selected.link_id < 0 ) return;
  rkChainLinkJointGetDis( &g_chain, g_selected.link_id, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &g_chain, g_selected.link_id, &dis );
  rkChainUpdateFK( &g_chain );
}

void draw_select_link(void)
{
  rkglChainDraw( &gr );
}

void rkglMouseFuncGLFW(int button, int event, int x, int y)
{
  rkglMouseStoreInput( button, event, GLFW_PRESS, x, y, GLFW_KEY_LEFT_CONTROL );
}

void rkglMouseDragFuncGLFW(int x, int y)
{
  double dx, dy;

  rkglMouseDragGetIncrementer( &g_cam, x, y, &dx, &dy );
  switch( rkgl_mouse_button ){
  case GLFW_MOUSE_BUTTON_LEFT:  rkglMouseDragCARotate(    &g_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_RIGHT: rkglMouseDragCATranslate( &g_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  case GLFW_MOUSE_BUTTON_MIDDLE: rkglMouseDragCAZoom(      &g_cam, dx, dy, GLFW_KEY_LEFT_CONTROL ); break;
  default: ;
  }
  rkglMouseStoreXY( x, y );
  glfwPostEmptyEvent();
}

bool g_mouse_left_button_clicked;
bool g_mouse_right_button_clicked;
int g_x;
int g_y;

void motion(GLFWwindow* window, double x, double y)
{
  g_x = floor(x);
  g_y = floor(y);
  if( g_mouse_left_button_clicked || g_mouse_right_button_clicked )
  {
    if( g_selected.obj != FRAMEHANDLE || g_mouse_right_button_clicked ){
      rkglMouseDragFuncGLFW(g_x, g_y);
    } else if( g_mouse_left_button_clicked ){
      /* moving mode */
      rkglFrameHandleMove( &g_fh, &g_cam, g_x, g_y );
      update_alljoint_by_IK_with_frame( &g_fh.frame );
    }
    /* end moving mode */
  }
  /* end of if( g_mouse_left_button_clicked ) */
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  rkglSelectionBuffer sb;

  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      g_mouse_left_button_clicked = true;
      rkglSelect( &sb, &g_cam, draw_fh_parts, g_x, g_y, 1, 1 );
      rkglFrameHandleSelect( &g_fh, &sb, &g_cam, g_x, g_y );
      if( !rkglFrameHandleIsUnselected( &g_fh ) ){
        g_selected.obj = FRAMEHANDLE;
        register_drag_link_for_IK();
      } else{
        rkglSelect( &sb, &g_cam, draw_select_link, g_x, g_y, 1, 1 );
        int new_link_id = select_link( &sb );
        reset_link_select_status( new_link_id );
        if( rkglChainLinkIsUnselected( new_link_id ) ){
          g_selected.obj = NONE;
        } else{
          g_selected.obj = LINKFRAME;
          /* update frame handle location */
          zFrame3DCopy( rkChainLinkWldFrame( gr.chain, new_link_id ), &g_fh.frame );
        }
        reset_selected_link( new_link_id );
      }
    } else if( state == GLFW_RELEASE ){
      g_mouse_left_button_clicked = false;
      if( !rkglFrameHandleIsUnselected( &g_fh ) ){
        unregister_drag_link_for_IK();
      }
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
      g_mouse_right_button_clicked = true;
      rkglSelect( &sb, &g_cam, draw_select_link, g_x, g_y, 1, 1 );
      int new_link_id = select_link( &sb );
      if( !rkglChainLinkIsUnselected( new_link_id ) ){
        switch_pin_link( new_link_id );
      }
      reset_selected_link( new_link_id );
    } else if( state == GLFW_RELEASE ){
      g_mouse_right_button_clicked = false;
    }
  }
  printf( "selected       : link_id = %d, ", g_selected.link_id );
  printf( "fh_parts_id = %d \n", g_fh.selected_id );

  if( g_selected.obj != FRAMEHANDLE || g_mouse_right_button_clicked ){
    rkglMouseFuncGLFW(button, state, g_x, g_y);
  }
}

void mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
  if ( yoffset < 0 ) {
    g_scale -= 0.0001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
  }
  if ( yoffset > 0 ) {
    g_scale += 0.0001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
  }
}

void resize(GLFWwindow* window, int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
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
  case '8': g_scale += 0.0001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.0001; rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar ); break;
  case 'g': move_link( zDeg2Rad(5) ); break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    rkglChainUnload( &gr );
    rkglChainUnload_for_rkglLinkInfo2();
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


bool init(void)
{
  g_mouse_left_button_clicked = false;
  g_mouse_right_button_clicked = false;

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  /* rkglCASet( &g_cam, 1, 1, 1, 45, -30, 0 ); */
  rkglCASet( &g_cam, 0, 0, 0, 45, -30, 0 );
  glEnable(GL_LIGHTING);
  rkglLightCreate( &g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 3, 5, 9 );
  rkglLightSetAttenuationConst( &g_light, 1.0 );

  rkglChainAttr attr;
  rkglChainAttrInit( &attr );
  if( g_modelfile == NULL ){
    g_modelfile = zStrClone( "../model/puma.ztk" );
  }
  printf( "modelfile = %s\n", g_modelfile );
  if( !extend_rkChainReadZTK( &g_chain, g_modelfile ) ){
    ZRUNWARN( "Failed extend_rkChainReadZTK()" );
    return false;
  }
  if( !rkglChainLoad( &gr, &g_chain, &attr, &g_light ) ){
    ZRUNWARN( "Failed rkglChainLoad()" );
    return false;
  }
  if( !rkglChainLoad_for_rkglLinkInfo2( &g_chain ) ){
    ZRUNWARN( "Failed rkglChainLoad_for_rkglLinkInfo2()" );
    return false;
  }
  int i;
  for( i=0; i<rkChainLinkNum(gr.chain); i++ ){
    /* definitons specific to this example code */
    gr_info2[i].select = NOT_SELECTED;
  }
  /* IK */
  rkChainCreateIK( &g_chain );
  rkChainRegIKJointAll( &g_chain, IK_DRAG_WEIGHT );
  /* select */
  g_selected.obj = NONE;
  g_selected.link_id = -1;
  /* frame handle */
  rkglFrameHandleCreate( &g_fh, NAME_FRAMEHANDLE_OFFSET, g_LENGTH, g_MAGNITUDE );

  return true;
}

void idle(void){
  glfwPostEmptyEvent();
}

int main(int argc, char *argv[])
{
  if( argc > 1 ){
    g_modelfile = argv[1];
  }
  /* initialize the location of frame handle object */
  zFrame3DFromAA( &g_fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

  if ( glfwInit() == GL_FALSE ){
    ZRUNERROR("Failed glfwInit()");
    return 1;
  }

  int width = 640;
  int height = 480;
  g_window = glfwCreateWindow( width, height, argv[0], NULL, NULL );
  if ( g_window == NULL ){
    ZRUNERROR("Failed glfwCreateWindow()");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent( g_window );

#ifdef __ROKI_GL_USE_GLEW
  rkglInitGLEW();
#endif /* __ROKI_GL_USE_GLEW */
  rkglEnableDefault();

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
    idle();
    display(g_window);
    glfwPollEvents();
  }
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
