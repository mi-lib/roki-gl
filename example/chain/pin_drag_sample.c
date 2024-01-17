#include <roki_gl/roki_glfw.h>

GLFWwindow* g_window;

/* the definition of select & pin information *****************************************/

/* rkglLinkInfo2.pin information */
typedef enum{
  PIN_LOCK_OFF=-1,
  PIN_LOCK_6D,
  PIN_LOCK_POS3D
} pinStatus;

typedef struct{
  bool is_selected;
  pinStatus pin;
  rkIKCell *cell[2];
} rkglLinkInfo2;

rkglLinkInfo2 *gr_info2;

bool rkglChainLoad_for_rkglLinkInfo2(rkChain *gc_chain)
{
  int i;
  if( !( gr_info2 = zAlloc( rkglLinkInfo2, rkChainLinkNum(gc_chain) ) ) ){
    ZALLOCERROR();
    return false;
  }
  for( i=0; i<rkChainLinkNum(gc_chain); i++ ){
    gr_info2[i].is_selected = false;
    gr_info2[i].pin = PIN_LOCK_OFF;
  }
  return true;
}

void rkglChainUnload_for_rkglLinkInfo2()
{
  zFree( gr_info2 );
}

/* the weight of pink link for IK */
#define IK_PIN_WEIGHT 1.0
/* the weight of drag link for IK */
#define IK_DRAG_WEIGHT 0.01

/* active selected the one object & link id */
typedef struct{
  int link_id;
  zVec3D ap;
} selectInfo;

selectInfo g_selected;

/* end of the definition of select & pin information **********************************/

/* the main targets of this sample code */
rkglFrameHandle g_fh;
rkChain g_chain;
rkglChain gr;

/* an imitation of rkglFrameHandleIsUnselected() */
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
static const double g_MAGNITUDE = g_LENGTH * 0.7;

/* To avoid duplication between selected_link and selected_parts_id */
/* NAME_FRAMEHANDLE_OFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NAME_CHAIN 0
#define NAME_FRAMEHANDLE_OFFSET 50

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  if( g_selected.link_id >= 0 ){
    rkglFrameHandleDraw( &g_fh );
  }
}

void draw_alternate_link(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
    printf( "alternate link : pre gr.info[%d].list = %d, ", id, gc->info[id].list );
    printf( "_list_backup = %d, ---> ", gc->info[id]._list_backup );
    /* TODO : reuse selected link list value */
    /* (the current implementation generates new list value by glNewList() ) */
    rkglChainLinkAlt( gc, id, oi_alt, attr, light );
    printf( "list = %d, ", gc->info[id].list );
    printf( "_list_backup = %d\n", gc->info[id]._list_backup );
}

void draw_scene(void)
{
  /* Transparency depends on drawing order */
  /* 1st, drawing FrameHandle */
  draw_fh_parts();
  /* 2nd, drawing link frame */
  zOpticalInfo oi_alt;
  gr.attr.disptype = RKGL_FACE;
  int link_id = g_selected.link_id;
  bool is_alt = ( link_id>=0 && gr.info[link_id]._list_backup == -1 );
  int pin = gr_info2[link_id].pin;
  /* re-drawing the selected link once after proccessing with rkglChainLinkReset() */
  if( is_alt && pin == PIN_LOCK_6D ){
    /* Red */
    zOpticalInfoCreate( &oi_alt, 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else if( is_alt && pin == PIN_LOCK_POS3D ){
    /* Green */
    zOpticalInfoCreate( &oi_alt, 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else if( is_alt && gr_info2[link_id].is_selected ){
    /* Blue */
    zOpticalInfoCreate( &oi_alt, 0.5, 0.7, 1.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else{
    rkglChainDraw( &gr );
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

void reset_link_drawing(int new_link_id)
{
  printf( "reset link     : pre gr.info[%d].list = %d, ", new_link_id, gr.info[new_link_id].list );
  printf( "_list_backup = %d, ---> ", gr.info[new_link_id]._list_backup );
  rkglChainLinkReset( &gr, new_link_id );
  printf( "list = %d, ", gr.info[new_link_id].list );
  printf( "_list_backup = %d\n", gr.info[new_link_id]._list_backup );
}

void reset_link_selected_status(int new_link_id)
{
  if( !rkglChainLinkIsUnselected( new_link_id ) ){
    if( !gr_info2[new_link_id].is_selected ){
      reset_link_drawing( new_link_id );
    }
    gr_info2[new_link_id].is_selected = true;
  } else {
    int i;
    printf("reset_all_link_selected_status(): reset all link\n");
    for( i=0; i<rkChainLinkNum(gr.chain); i++ ){
      if( gr_info2[i].is_selected
          && gr_info2[i].pin == PIN_LOCK_OFF ){
        reset_link_drawing( i );
        gr_info2[i].is_selected = false;
      }
    } /* end of for */
  } /* end of if( !rkglChainLinkIsUnselected( new_link_id ) ) else */
}

void update_selected_link(int new_link_id)
{
  g_selected.link_id = new_link_id;
}

void update_framehandle_location(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int link_id)
{
  zVec3D selected_point;
  double depth;

  /* the origin position of the selected link mode */
  zFrame3DCopy( rkChainLinkWldFrame( gr.chain, link_id ), &g_fh.frame );

  /* key optional */
  if( rkgl_key_mod & GLFW_KEY_LEFT_CONTROL ){
    /* selected position mode */
    depth = rkglSelectionZnearDepth(sb);
    rkglUnproject( cam, x, y, depth, &selected_point );
    zFrame3DSetPos( &g_fh.frame, &selected_point );
    /* transform Wolrd -> Link frame */
    zXform3DInv( rkChainLinkWldFrame(&g_chain, link_id), zFrame3DPos(&g_fh.frame), &g_selected.ap );
  } else{
    zVec3DZero( &g_selected.ap );
  }
}

void switch_pin_link(int new_link_id)
{
  switch( gr_info2[new_link_id].pin ){
  case PIN_LOCK_OFF:
    gr_info2[new_link_id].pin = PIN_LOCK_6D;
    break;
  case PIN_LOCK_6D:
    gr_info2[new_link_id].pin = PIN_LOCK_POS3D;
    break;
  case PIN_LOCK_POS3D:
    gr_info2[new_link_id].pin = PIN_LOCK_OFF;
    break;
  default: ;
  }
  /* reset for only pin link drawing */
  reset_link_drawing( new_link_id );
  printf("pin_link       : link_id = %d, pin status = %d\n", new_link_id, gr_info2[new_link_id].pin );
}

void register_one_link_for_IK(int link_id)
{
  rkIKAttr attr;
  attr.id = link_id;
  gr_info2[link_id].cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
  gr_info2[link_id].cell[1] = rkChainRegIKCellWldPos( &g_chain, &attr, RK_IK_ATTR_ID | RK_IK_ATTR_AP );
  switch( gr_info2[link_id].pin ){
  case PIN_LOCK_6D:
    rkIKCellSetWeight( gr_info2[link_id].cell[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    rkIKCellSetWeight( gr_info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_POS3D:
    rkIKCellSetWeight( gr_info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( gr_info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_OFF:
    rkIKCellSetWeight( gr_info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( gr_info2[link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    break;
  default: ;
  }
  zVec3DCopy( &g_selected.ap, rkIKCellAP(gr_info2[link_id].cell[1]) );
}

void unregister_one_link_for_IK(int link_id)
{
  rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[0] );
  rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[1] );
}

void register_drag_weight_link_for_IK(int drag_link_id)
{
  int id;
  register_one_link_for_IK( drag_link_id );
  for( id=0; id<rkChainLinkNum(gr.chain); id++ ){
    if( id != drag_link_id && gr_info2[id].pin == PIN_LOCK_POS3D ){
      rkIKAttr attr;
      attr.id = id;
      gr_info2[id].cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
      rkIKCellSetWeight( gr_info2[id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    }
  }
}

void unregister_drag_weight_link_for_IK(int drag_link_id)
{
  int id;
  unregister_one_link_for_IK( drag_link_id );
  for( id=0; id<rkChainLinkNum(gr.chain); id++ ){
    if( id != drag_link_id && gr_info2[id].pin == PIN_LOCK_POS3D )
      rkChainUnregIKCell( &g_chain, gr_info2[id].cell[0] );
  }
}

void register_link_for_IK(int drag_link_id)
{
  int id;
  for( id=0; id<rkChainLinkNum(gr.chain); id++ ){
    if( id == drag_link_id || gr_info2[id].pin != PIN_LOCK_OFF )
      register_one_link_for_IK( id );
  }
}

void unregister_link_for_IK(int drag_link_id)
{
  int id;
  for( id=0; id<rkChainLinkNum(gr.chain); id++ ){
    if( id == drag_link_id || gr_info2[id].pin != PIN_LOCK_OFF )
      unregister_one_link_for_IK( id );
  }
}

/* inverse kinematics */
void update_alljoint_by_IK_with_frame(int drag_link_id, zFrame3D *ref_frame)
{
  if( drag_link_id < 0 ) return;
  zVec dis; /* joints zVec pointer */
  dis = zVecAlloc( rkChainJointSize( &g_chain ) );
  rkChainGetJointDisAll(&g_chain, dis);
  /* prepare IK */
  rkChainDeactivateIK( &g_chain );
  rkChainBindIK( &g_chain );
  /* set rotation reference */
  if( gr_info2[drag_link_id].pin == PIN_LOCK_6D
      || rkglFrameHandleIsInRotation( &g_fh ) ){
    zVec3D zyx;
    zMat3DToZYX( &(ref_frame->att), &zyx );
    rkIKCellSetRefVec( gr_info2[drag_link_id].cell[0], &zyx );
  }
  /* set position reference */
  rkIKCellSetRefVec( gr_info2[drag_link_id].cell[1], &(ref_frame->pos) );
  rkChainFK( &g_chain, dis ); /* copy state to mentatin result consistency */
  /* IK */
  /* printf("pre IK Joint[deg]  = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  int iter = 100;
  double ztol = zTOL;
  /* backup in case the result of rkChainIK() is NaN */
  rkChain clone_chain;
  rkChainClone( &g_chain, &clone_chain );
  rkChainIK( &g_chain, dis, ztol, iter );
  /* printf("post IK Joint[deg] = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_chain );
  }
  /* IK again with only pin link */
  unregister_drag_weight_link_for_IK( drag_link_id );
  rkChainIK( &g_chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_chain );
  }
  register_drag_weight_link_for_IK( drag_link_id );
  /* keep FrameHandle position */
  if( rkglFrameHandleIsInRotation( &g_fh ) )
    zFrame3DCopy( rkChainLinkWldFrame( gr.chain, drag_link_id ), &g_fh.frame );
  zXform3D( rkChainLinkWldFrame( &g_chain, drag_link_id ), &g_selected.ap, zFrame3DPos( &g_fh.frame ) );
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
  rkglChainSetName( &gr, NAME_CHAIN );
  rkglChainDraw( &gr );
}

void draw_select_fh_parts(void)
{
  draw_fh_parts();
}

void motion(GLFWwindow* window, double x, double y)
{
  if( rkgl_mouse_button == GLFW_MOUSE_BUTTON_RIGHT ||
      rkglFrameHandleIsUnselected( &g_fh ) ){
    rkglMouseDragFuncGLFW( window, x, y );
  } else if( rkgl_mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
             !rkglFrameHandleIsUnselected( &g_fh ) ){
    /* moving mode */
    rkglFrameHandleMove( &g_fh, &g_cam, rkgl_mouse_x, rkgl_mouse_y );
    update_alljoint_by_IK_with_frame( g_selected.link_id, &g_fh.frame );
  }
  rkglMouseStoreXY( floor(x), floor(y) );
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  int new_link_id = -1;
  rkglSelectionBuffer sb;

  /* store button when state == GLFW_PRESS */
  rkglMouseFuncGLFW( window, button, state, mods );

  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      /* draw only frame handle */
      if( rkglSelectNearest( &sb, &g_cam, draw_select_fh_parts, rkgl_mouse_x, rkgl_mouse_y, 1, 1 )
          && rkglFrameHandleAnchor( &g_fh, &sb, &g_cam, rkgl_mouse_x, rkgl_mouse_y ) >= 0 ){
        register_link_for_IK( g_selected.link_id );
      } else{
        /* draw only chain */
        if( rkglSelectNearest( &sb, &g_cam, draw_select_link, rkgl_mouse_x, rkgl_mouse_y, 1, 1 )
            && ( new_link_id = rkglChainLinkFindSelected( &gr, &sb ) ) >= 0 ){
          update_framehandle_location( &sb, &g_cam, rkgl_mouse_x, rkgl_mouse_y, new_link_id );
        }
        reset_link_selected_status( new_link_id );
        update_selected_link( new_link_id );
        rkglFrameHandleUnselect( &g_fh );
      }
    } else if( state == GLFW_RELEASE ){
      if( !rkglFrameHandleIsUnselected( &g_fh ) ){
        unregister_link_for_IK( g_selected.link_id );
      }
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
      if( rkglSelectNearest( &sb, &g_cam, draw_select_link, rkgl_mouse_x, rkgl_mouse_y, 1, 1 )
          && ( new_link_id = rkglChainLinkFindSelected( &gr, &sb ) ) >= 0 ){
        switch_pin_link( new_link_id );
      }
      update_selected_link( new_link_id );
    }
  }
  printf( "selected       : link_id = %d, ", g_selected.link_id );
  printf( "fh_parts_id = %d \n", g_fh.selected_id );
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
  rkglSetDefaultCallbackParam( &g_cam, 1.0, g_znear, g_zfar, 1.0, 5.0 );

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
  /* IK */
  rkChainCreateIK( &g_chain );
  rkChainRegIKJointAll( &g_chain, IK_DRAG_WEIGHT );
  /* select */
  g_selected.link_id = -1;
  /* frame handle */
  rkglFrameHandleCreate( &g_fh, NAME_FRAMEHANDLE_OFFSET, g_LENGTH, g_MAGNITUDE );
  /* initialize the location of frame handle object */
  zFrame3DFromAA( &g_fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

  return true;
}

int main(int argc, char *argv[])
{
  if( argc > 1 ){
    g_modelfile = argv[1];
  }
  if( rkglInitGLFW( &argc, argv ) < 0 ){
    return 1;
  }
  int width = 640;
  int height = 480;
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
