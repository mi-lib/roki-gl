#include <roki_gl/roki_glut.h>

/* suggestion to add rk_shape.c ************************************************************/

void rkglFrameAxisMaterial(zAxis a)
{
  zOpticalInfo oi;
  switch(a){
  case zX:
    /* Red */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  case zY:
    /* Green */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  case zZ:
    /* Blue */
    zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, NULL );
    break;
  default: return;
  }
  rkglMaterial( &oi );
}

void rkglFrameHandleArrowParts(zFrame3D *f, zAxis a, double l, double mag)
{
  zVec3D v, vb;

  zVec3DMul( &zFrame3DAtt(f)->v[a], 0.5*l, &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
  zVec3DRevDRC( &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
}

void rkglFrameHandleTorusParts(zFrame3D *f, zAxis a, double l, double mag)
{
  double r1, r2;

  r1 = l * 0.5 + RKGL_ARROW_BOTTOM_RAD * mag;
  r2 = l * 0.5 - RKGL_ARROW_BOTTOM_RAD * mag;

  rkglTorus( zFrame3DPos(f), &zFrame3DAtt(f)->v[a], r1, r2, RKGL_ARROW_DIV*4, RKGL_ARROW_DIV, RKGL_FACE );
}

/* End of suggestion to add rk_shape.c *****************************************************/


/* suggestion to add rkglLinkInfo rkgl_chain.h ********************************************/
typedef struct{
  int select; /* suggestion info */
  int pin;
  rkIKCell *cell[2];
} rkglLinkInfo2;
/* End of suggestion to add rkglLinkInfo rkgl_chain.h *************************************/

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

/* pin information */
static const int NOT_PIN_LINK = -1;
static const int PIN_LINK = 1;
static const int ONLY_POS3D_PIN_LINK = 2;

/* the weight of pink link for IK */
#define IK_PIN_WEIGHT 1.0
/* the weight of drag link for IK */
#define IK_DRAG_WEIGHT 0.0001

/* the number of FrameHandle parts */
#define NOBJECTS 6
/* the number of translation size (x,y,z : 3) */
#define NPOSSIZE 3

static const zAxis g_AXES[NOBJECTS] = {zX, zY, zZ, zX, zY, zZ};

typedef struct{
  GLint list;
  int updown;
} partsInfo_t;

typedef struct{
  partsInfo_t partsInfo[NOBJECTS];
  zFrame3D frame;
} frameHandle_t;

/* the main targets of this sample code */
frameHandle_t g_fh;
rkChain g_chain;
rkglChain gr;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

/* FrameHandle shape property */
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.5;

/* mouse selected information */
typedef enum{
  NONE=1,
  LINKFRAME,
  FRAMEHANDLE,
} selectedObj;

/* for rkglLinkInfo2.select */
static const int NOT_SELECTED = -1;
static const int INACTIVE_SELECT = 0;
static const int ACTIVE_SELECT = 1;

/* select object id */
typedef struct{
  selectedObj obj;
  int link_id;
  int fh_parts_id; /* FrameHandle parts id */
} selectInfo;

selectInfo g_selected;

/* judge whether the selected shape ID is translation moving parts or not */
bool is_translation_mode(int fh_parts_id)
{
  /* define the range of translation ID */
  return (fh_parts_id >= 0 && fh_parts_id < NPOSSIZE);
}

/* judge whether the selected shape ID is rotation moving parts or not */
bool is_rotation_mode(int fh_parts_id)
{
  /* define the range of rotation ID */
  return (fh_parts_id >= NPOSSIZE && fh_parts_id < NOBJECTS);
}

/* To avoid duplication between selected_link and selected_parts_id */
/* NOFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NOFFSET 50

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  int i;
  for( i=0; i < NOBJECTS; i++ ){
    glLoadName( i + NOFFSET );
    if( g_fh.partsInfo[i].updown == 0 )
      rkglFrameAxisMaterial( g_AXES[i] );
    else
      rkglMaterial(NULL);
    glPushMatrix();
    rkglXform( &g_fh.frame );
    glCallList( g_fh.partsInfo[i].list );
    glPopMatrix();
  }
}

void draw_alternate_link(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
    printf( "alternate link : pre gr.info[%d].list = %d, ", id, gc->info[id].list );
    printf( "list_alt = %d, ---> ", gc->info[id].list_alt );
    /* TODO : reuse selected link list value */
    /* (the current implementation generates new list value by glNewList() ) */
    rkglChainLinkAlt( gc, id, oi_alt, attr, light );
    printf( "list = %d, ", gc->info[id].list );
    printf( "list_alt = %d\n", gc->info[id].list_alt );
}

void draw_scene(void)
{
  /* Transparency depends on drawing order */
  if( g_selected.obj != NONE ){
    /* drawing FrameHandle */
    draw_fh_parts();
  }
  /* drawing link frame */
  zOpticalInfo oi_alt;
  gr.attr.disptype = RKGL_FACE;
  int link_id = g_selected.link_id;
  bool is_alt = ( gr.info[link_id].list_alt == -1 );
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
  } else if( g_selected.obj != NONE && is_alt ){
    /* Blue */
    zOpticalInfoCreate( &oi_alt, 0.5, 0.7, 1.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &gr, link_id, &oi_alt, &gr.attr, &g_light );
  } else{
    rkglChainDraw( &gr );
  }
  /* end of if( g_selected.obj != NONE ) */
}

void display(void)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

static double g_zmin; /* selected depth of the nearest object */
static zFrame3D g_frame3D_org;
static zVec3D g_vp_mouse_start_3D;
static zVec3D g_vp_parts_axis_3D_vector;
static zVec3D g_parts_axis_unit_3D_vector;
static zVec3D g_vp_circle_center_3D;
static zVec3D g_vp_radial_dir_center_to_start;

void reset_link_drawing(int new_link_id)
{
  printf( "reset link     : pre gr.info[%d].list = %d, ", new_link_id, gr.info[new_link_id].list );
  printf( "list_alt = %d, ---> ", gr.info[new_link_id].list_alt );
  rkglChainLinkReset( &gr, new_link_id );
  printf( "list = %d, ", gr.info[new_link_id].list );
  printf( "list_alt = %d\n", gr.info[new_link_id].list_alt );
}

void reset_selected_link(int new_link_id)
{
  if( new_link_id != g_selected.link_id ){
    if( g_selected.link_id >= 0 ){
      gr_info2[g_selected.link_id].select = INACTIVE_SELECT;
    } else{
      /* This may be redundant */
      gr_info2[g_selected.link_id].select = NOT_SELECTED;
    }
    g_selected.link_id = new_link_id;
  }
  if( new_link_id >= 0 ){
    if( gr_info2[new_link_id].select == NOT_SELECTED ){
      reset_link_drawing( new_link_id );
    }
    gr_info2[new_link_id].select = ACTIVE_SELECT;
  } else{
    int i;
    for( i=0; i<rkChainLinkNum(gr.chain); i++ ){
      if( gr_info2[i].select != NOT_SELECTED
          && gr_info2[i].pin == NOT_PIN_LINK ){
        reset_link_drawing(i);
        gr_info2[i].select = NOT_SELECTED;
      }
    }
  }
  /* end of if( new_link_id >= 0 ) */
}

void reset_fh_parts(void)
{
  if( g_selected.fh_parts_id >= 0 ){
    g_fh.partsInfo[ g_selected.fh_parts_id ].updown = 0;
  }
}

int select_object(GLuint selbuf[], int hits)
{
  int nearest_shape_id = -1;
  GLuint *ns;
  if( !( ns = rkglFindNearside( selbuf, hits ) ) ){
    g_selected.obj = NONE;
    return nearest_shape_id;
  }
  nearest_shape_id = ns[3];
  /* zmin(=ns[1]) is GLuint data. GLuint maximum value is 0xffffffff. */
  g_zmin = ns[1] / (GLdouble)(0xffffffff);
  if( nearest_shape_id >= 0
      && nearest_shape_id < rkChainLinkNum(gr.chain) ){
    g_selected.obj = LINKFRAME;
  } else if( nearest_shape_id >= NOFFSET
             && nearest_shape_id < NOFFSET + NOBJECTS ){
    g_selected.obj = FRAMEHANDLE;
  } else{
    g_selected.obj = NONE;
  }
  return nearest_shape_id;
}

void switch_status(int new_selected_shape_id)
{
  reset_fh_parts();
  switch( g_selected.obj ){
  case LINKFRAME:
    g_selected.fh_parts_id = -1;
    /* update frame handle location */
    if( new_selected_shape_id != g_selected.link_id ) {
      zFrame3DCopy( rkChainLinkWldFrame(gr.chain, new_selected_shape_id), &g_fh.frame );
    }
    reset_selected_link( new_selected_shape_id );
    break;
  case FRAMEHANDLE:
    /* g_selected.link_id is not changed */
    g_selected.fh_parts_id = new_selected_shape_id - NOFFSET;
    break;
  case NONE:
    reset_selected_link( new_selected_shape_id );
    g_selected.fh_parts_id = -1;
    break;
  default: break;
  }
  printf( "selected       : link_id = %d, ", g_selected.link_id );
  printf( "fh_parts_id = %d \n", g_selected.fh_parts_id );
}

void switch_pin_link(int new_link_id)
{
  rkIKAttr attr;
  switch( gr_info2[new_link_id].pin ){
  case NOT_PIN_LINK:
    gr_info2[new_link_id].pin = PIN_LINK;
    /* register rot & pos & weight up */
    attr.id = new_link_id;
    gr_info2[new_link_id].cell[0] = rkChainRegIKCellWldAtt( &g_chain, &attr, RK_IK_ATTR_ID );
    gr_info2[new_link_id].cell[1] = rkChainRegIKCellWldPos( &g_chain, &attr, RK_IK_ATTR_ID );
    rkIKAttrSetWeight( &attr, IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LINK:
    gr_info2[new_link_id].pin = ONLY_POS3D_PIN_LINK;
    /* pos weight up */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    /* un-register rot */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkChainUnregIKCell( &g_chain, gr_info2[new_link_id].cell[0] );
    break;
  case ONLY_POS3D_PIN_LINK:
    gr_info2[new_link_id].pin = NOT_PIN_LINK;
    /* un-register pos */
    rkIKCellSetWeight( gr_info2[new_link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkChainUnregIKCell( &g_chain, gr_info2[new_link_id].cell[1] );
    break;
  default:
    break;
  }
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
    if( is_rotation_mode( g_selected.fh_parts_id ) ){
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
    if( is_rotation_mode( g_selected.fh_parts_id ) ){
      rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[0] );
    }
    if( pin == NOT_PIN_LINK ){
      rkChainUnregIKCell( &g_chain, gr_info2[link_id].cell[1] );
    }
  }
  /* end of if( pin != PIN_LINK ) */
}

void select_fh_parts(int x, int y)
{
  g_fh.partsInfo[ g_selected.fh_parts_id ].updown = 1;
  zFrame3DCopy( &g_fh.frame, &g_frame3D_org );

  zVec3D parts_axis_start_3D;
  rkglUnproject( &g_cam, x, y, g_zmin, &parts_axis_start_3D );
  /* project to view port */
  rkglUnproject( &g_cam, x, y, g_znear, &g_vp_mouse_start_3D );

  if( is_translation_mode( g_selected.fh_parts_id ) ){
    /* translate mode */
    if( g_selected.fh_parts_id == 0 ){
      /* x */
      zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 1 ){
      /* y */
      zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 2 ){
      /* z */
      zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
    }
    zVec3D parts_axis_unit_end_3D;
    zVec3DAdd( &parts_axis_start_3D, &g_parts_axis_unit_3D_vector, &parts_axis_unit_end_3D );

    /* project to view port */
    int ax, ay;
    rkglProject( &g_cam, &parts_axis_unit_end_3D, &ax, &ay );
    zVec3D vp_parts_axis_unit_end_3D;
    rkglUnproject( &g_cam, ax, ay, g_znear, &vp_parts_axis_unit_end_3D );
    zVec3DSub( &vp_parts_axis_unit_end_3D, &g_vp_mouse_start_3D, &g_vp_parts_axis_3D_vector );

    /* end of translate mode */
  } else if( is_rotation_mode( g_selected.fh_parts_id ) ){
    /* rotate mode */
    if( g_selected.fh_parts_id == 3 ){
      /* x */
      zFrame3DCopy( &g_fh.frame.att.b.x, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 4 ){
      /* y */
      zFrame3DCopy( &g_fh.frame.att.b.y, &g_parts_axis_unit_3D_vector );
    } else if( g_selected.fh_parts_id == 5 ){
      /* z */
      zFrame3DCopy( &g_fh.frame.att.b.z, &g_parts_axis_unit_3D_vector );
    }
    /* project to view port */
    int cx, cy;
    rkglProject( &g_cam, &g_fh.frame.pos, &cx, &cy );
    rkglUnproject( &g_cam, cx, cy, g_znear, &g_vp_circle_center_3D );
    zVec3DSub( &g_vp_mouse_start_3D, &g_vp_circle_center_3D, &g_vp_radial_dir_center_to_start);
  } else{
    ZRUNERROR( "selected_parts_id is ERROR\n" );
  }
  /* end of rotate mode */
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
      || is_rotation_mode( g_selected.fh_parts_id ) ){
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
  int ret = rkChainIK( &g_chain, dis, ztol, iter );
  /* printf("post IK Joint[deg] = "); */
  /* zVecPrint(zVecMulDRC( zVecClone(dis), 180.0/zPI )); */
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_chain);
  } else if( ret >= 0 ){
    rkChainSetJointDisAll( &g_chain, dis );
    rkChainUpdateFK( &g_chain );
  }
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

void draw_select_fh_parts(void)
{
  if( g_selected.obj != NONE ){
    draw_fh_parts();
  }
}

void draw_select_link(void)
{
  rkglChainDraw( &gr );
}

void mouse(int button, int state, int x, int y)
{
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      GLuint selbuf[BUFSIZ];
      int hits = rkglPick( &g_cam, draw_select_fh_parts, selbuf, BUFSIZ, x, y, 1, 1 );
      int new_selected_shape_id = select_object( selbuf, hits );
      if( g_selected.obj == FRAMEHANDLE ){
        /* update g_selected status */
        switch_status( new_selected_shape_id );
        select_fh_parts( x, y );
        register_drag_link_for_IK();
      } else{
        GLuint selbuf2[BUFSIZ];
        hits = rkglPick( &g_cam, draw_select_link, selbuf2, BUFSIZ, x, y, 1, 1 );
        new_selected_shape_id = select_object( selbuf2, hits );
        switch_status( new_selected_shape_id );
      }
    } else if( state == GLUT_UP ){
      if( g_selected.obj == FRAMEHANDLE ){
        unregister_drag_link_for_IK();
      }
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if( state == GLUT_DOWN ){
      GLuint selbuf3[BUFSIZ];
      int hits = hits = rkglPick( &g_cam, draw_select_link, selbuf3, BUFSIZ, x, y, 1, 1 );
      /* int org_obj = g_selected.obj; */
      int new_selected_shape_id = select_object( selbuf3, hits );
      if( g_selected.obj == LINKFRAME ){
        switch_pin_link( new_selected_shape_id );
      }
      switch_status( new_selected_shape_id );
      /* g_selected.obj = org_obj; */
    }
    break;
  default: break;
  } /* end of switch( button ) */
  if( g_selected.obj != FRAMEHANDLE ){
    rkglMouseFuncGLUT(button, state, x, y);
  }
}

void motion(int x, int y)
{
  if( g_selected.obj != FRAMEHANDLE ){
    rkglMouseDragFuncGLUT(x, y);
  } else{
    /* moving mode */
    zVec3D vp_mouse_goal_3D;
    rkglUnproject( &g_cam, x, y, g_znear, &vp_mouse_goal_3D );

    int fh_parts_id = g_selected.fh_parts_id;
    if( is_translation_mode( fh_parts_id ) ){
      /* translate mode */
      zVec3D vp_mouse_drag_3D_vector;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_mouse_start_3D, &vp_mouse_drag_3D_vector );

      double project_3D_scale = zVec3DInnerProd( &vp_mouse_drag_3D_vector, &g_vp_parts_axis_3D_vector ) / zVec3DSqrNorm( &g_vp_parts_axis_3D_vector );

      zVec3D dir_3D;
      zVec3DMul( &g_parts_axis_unit_3D_vector, project_3D_scale, &dir_3D );

      /* translate frame */
      zVec3DAdd( &g_frame3D_org.pos, &dir_3D, &g_fh.frame.pos );

      /* end of translate mode */
    } else if( is_rotation_mode( fh_parts_id ) ){
      /* rotate mode */
      zVec3D vp_radial_dir_center_to_goal;
      zVec3DSub( &vp_mouse_goal_3D, &g_vp_circle_center_3D, &vp_radial_dir_center_to_goal);

      if( !zVec3DIsTiny( &g_vp_radial_dir_center_to_start ) ){
        /* oval circle rotation on view port plane */
        double theta = zVec3DAngle( &g_vp_radial_dir_center_to_start, &vp_radial_dir_center_to_goal, &g_parts_axis_unit_3D_vector);
        zMat3D m;
        /* zVec3D axis; */
        if( fh_parts_id == 3 ){
          /* x */
          zMat3DFromZYX( &m, 0, 0, theta );
        } else if( fh_parts_id == 4 ){
          /* y */
          zMat3DFromZYX( &m, 0, theta, 0 );
        } else if( fh_parts_id == 5 ){
          /* z */
          zMat3DFromZYX( &m, theta, 0, 0 );
        }
        zMulMat3DMat3D( &g_frame3D_org.att, &m, &g_fh.frame.att );
      }
      /* end of if g_vp_radial_dir_center_to_start is not Tiny */
    } else{
      ZRUNERROR( "selected_parts_id is ERROR\n" );
    }
    /* end of rotate mode */

    update_alljoint_by_IK_with_frame( &g_fh.frame );
  }
  /* end moving mode */
}

void resize(int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScale( &g_cam, g_scale, g_znear, g_zfar );
}

void keyboard(unsigned char key, int x, int y)
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

void init_fh_parts(void)
{
  int i;
  zFrame3DIdent( &g_fh.frame );
  for( i=0; i<NOBJECTS; i++ ){
    g_fh.partsInfo[i].updown = 0;
    glPushMatrix();
    rkglXform( &g_fh.frame );
    rkglFrameAxisMaterial( g_AXES[i] );
    /* start register */
    g_fh.partsInfo[i].list = glGenLists( 1 );
    if( glIsList( g_fh.partsInfo[i].list ) )
      glDeleteLists( g_fh.partsInfo[i].list, 1 );
    glNewList( g_fh.partsInfo[i].list, GL_COMPILE );
    if( i < NPOSSIZE ){
      /* translation arrow shape */
      rkglFrameHandleArrowParts( &g_fh.frame, g_AXES[i], g_LENGTH, g_MAGNITUDE );
    } else {
      /* rotation torus shape */
      rkglFrameHandleTorusParts( &g_fh.frame, g_AXES[i], g_LENGTH, g_MAGNITUDE );
    }
    glEndList();
    /* end register */
    glPopMatrix();
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
  rkglSetCallbackParamGLUT( &g_cam, 0, 0, 0, 0, 0 );
  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 1, 1, 1, 45, -30, 0 );
  glEnable(GL_LIGHTING);
  rkglLightCreate( &g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 3, 5, 9 );
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
  g_selected.fh_parts_id = -1;
  /* frame handle */
  init_fh_parts();

  return true;
}

void idle(void){ glutPostRedisplay(); }

int main(int argc, char *argv[])
{
  if( argc > 1 ){
    g_modelfile = argv[1];
  }
  /* initialize the location of frame handle object */
  zFrame3DFromAA( &g_fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 320, 320, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );
  if( !init() ) return 1;
  glutMainLoop();

  return 0;
}
