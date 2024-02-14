#include <roki/rk_cd.h>
#include <roki_gl/roki_glfw.h>

/* #define DEBUG_PRINT */
#ifdef DEBUG_PRINT
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

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
  bool is_collision;
  pinStatus pin;
  rkIKCell *cell[2];
} rkglLinkInfo2;

/* the weight of pink link for IK */
#define IK_PIN_WEIGHT 1.0
/* the weight of drag link for IK */
#define IK_DRAG_WEIGHT 0.01

/* active selected the one object & link id */
typedef struct{
  int chain_id;
  int link_id;
  zVec3D ap;
} selectInfo;

selectInfo g_selected;

typedef struct{
  int chain_id;
  int link_id;
} cdInfoCellDat;
zListClass( cdInfoCellList, cdInfoCell, cdInfoCellDat );
cdInfoCellList g_cdlist;
bool g_is_collision;

/* end of the definition of select & pin information **********************************/

/* the main targets of this sample code */
rkglFrameHandle g_fh;
typedef struct{
  rkglLinkInfo2 *info2; /* num chainlink */
  rkChain chain;
  rkglChain glChain;
} rkglChainBlock;

rkglChainBlock *grs;
int g_chainNUM;

rkCD g_cd;

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
#define NAME_FRAMEHANDLE_OFFSET 100

bool rkglChainLoad_for_rkglChainBlock(rkglChainBlock *gcb )
{
  int i;
  rkglChainAttr attr;
  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &gcb->glChain, &gcb->chain, &attr, &g_light ) ){
    ZRUNWARN( "Failed rkglChainLoad()" );
    return false;
  }
  if( !( gcb->info2 = zAlloc( rkglLinkInfo2, rkChainLinkNum(&gcb->chain) ) ) ){
    ZALLOCERROR();
    return false;
  }
  for( i=0; i < rkChainLinkNum(&gcb->chain); i++ ){
    gcb->info2[i].is_selected = false;
    gcb->info2[i].pin = PIN_LOCK_OFF;
  }
  return true;
}

void rkglChainUnload_for_rkglChainBlock(rkglChainBlock *gcb)
{
  rkglChainUnload( &gcb->glChain );
  zFree( gcb->info2 );
}

void reset_link_drawing(int new_chain_id, int new_link_id)
{
  debug_printf( "reset link     : rkglChain[%d].info[%d] : list = %d, ", new_chain_id, new_link_id, grs[new_chain_id].glChain.info[new_link_id].list );
  debug_printf( "_list_backup = %d, ---> ", grs[new_chain_id].glChain.info[new_link_id]._list_backup );
  rkglChainLinkReset( &grs[new_chain_id].glChain, new_link_id );
  debug_printf( "list = %d, ", grs[new_chain_id].glChain.info[new_link_id].list );
  debug_printf( "_list_backup = %d\n", grs[new_chain_id].glChain.info[new_link_id]._list_backup );
}

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  if( g_selected.chain_id >=0 && g_selected.link_id >= 0 &&
      grs[g_selected.chain_id].info2[g_selected.link_id].is_selected ){
    rkglFrameHandleDraw( &g_fh );
  }
}

void draw_chain(void)
{
  int i;

  for( i=0; i < g_chainNUM; i++ ){
    rkglChainSetName( &grs[i].glChain, i ); /* NAME_CHAIN = i */
    rkglChainDraw( &grs[i].glChain );
  }
}

void draw_alternate_link(rkglChain *gc, int chain_id, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
  debug_printf( "alternate link : rkglChain[%d].info[%d] : list = %d, ", chain_id, id, gc->info[id].list );
  debug_printf( "_list_backup = %d, ---> ", gc->info[id]._list_backup );
  /* TODO : reuse selected link list value */
  /* (the current implementation generates new list value by glNewList() ) */
  rkglChainLinkAlt( gc, id, oi_alt, attr, light );
  debug_printf( "list = %d, ", gc->info[id].list );
  debug_printf( "_list_backup = %d\n", gc->info[id]._list_backup );
}

bool draw_pindrag_link(int chain_id, int link_id, bool is_alt)
{
  zOpticalInfo oi_alt;
  /* re-drawing the selected link once after proccessing with rkglChainLinkReset() */
  int pin = is_alt ? grs[chain_id].info2[link_id].pin : -1;
  if( is_alt ) grs[chain_id].glChain.attr.disptype = RKGL_FACE;
  if( is_alt && pin == PIN_LOCK_6D ){
    /* Red & semi-transparent*/
    zOpticalInfoCreate( &oi_alt, 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &grs[chain_id].glChain, chain_id, link_id, &oi_alt, &grs[chain_id].glChain.attr, &g_light );
  } else if( is_alt && pin == PIN_LOCK_POS3D ){
    /* Yellow & semi-transparent*/
    zOpticalInfoCreate( &oi_alt, 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &grs[chain_id].glChain, chain_id, link_id, &oi_alt, &grs[chain_id].glChain.attr, &g_light );
  } else if( is_alt && grs[chain_id].info2[link_id].is_selected ){
    /* Blue & semi-transparent */
    zOpticalInfoCreate( &oi_alt, 0.5, 0.7, 1.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, NULL );
    draw_alternate_link( &grs[chain_id].glChain, chain_id, link_id, &oi_alt, &grs[chain_id].glChain.attr, &g_light );
  } else {
    return false;
  }

  return true;
}

void draw_collision_link(void)
{
  zOpticalInfo oi_alt;
  if( g_is_collision ){
    debug_printf("\n\n is_collision == true ================================= \n");
    /* Red */
    zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
    cdInfoCell *cdcell;
    zListForEachRew( &g_cdlist, cdcell ){
      if( grs[cdcell->data.chain_id].glChain.info[cdcell->data.link_id]._list_backup==-1 ){
        grs[cdcell->data.chain_id].glChain.attr.disptype = RKGL_FACE;
        draw_alternate_link( &grs[cdcell->data.chain_id].glChain, cdcell->data.chain_id, cdcell->data.link_id, &oi_alt, &grs[cdcell->data.chain_id].glChain.attr, &g_light );
      }
    }
    debug_printf("end of is_collision == true ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n\n");
  }
  g_is_collision = false;
}

void draw_all_chain_link(void)
{
  int chain_id, link_id;
  bool is_pindrag;

  draw_collision_link();
  for( chain_id=0; chain_id < g_chainNUM; chain_id++ ){
    for( link_id=0; link_id < rkChainLinkNum(grs[chain_id].glChain.chain); link_id++ ){
      is_pindrag = false;
      if( grs[chain_id].glChain.info[link_id]._list_backup == -1 &&
          !grs[chain_id].info2[link_id].is_collision )
        is_pindrag = draw_pindrag_link( chain_id, link_id, true );
      if( !is_pindrag )
        rkglChainLinkDraw( &grs[chain_id].glChain, link_id );
    }
  }
}

void draw_scene(void)
{
  /* Transparency depends on drawing order */
  /* 1st, drawing FrameHandle */
  draw_fh_parts();
  /* 2nd, drawing each glChain, and its link frame */
  draw_all_chain_link();
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();

  draw_scene();
  glfwSwapBuffers( window );
}


bool is_select_chain_link(rkglSelectionBuffer *sb, int *chain_id, int *link_id)
{
  int i;

  *link_id = -1;
  *chain_id = -1;
  for( i=0; i < g_chainNUM; i++ ){
    if( ( *link_id = rkglChainLinkFindSelected( &grs[i].glChain, sb ) ) >= 0 ){
      *chain_id = i;
      return true;
    }
  }
  return false;
}

void update_selected_chain_link(int new_chain_id, int new_link_id)
{
  g_selected.chain_id = new_chain_id;
  g_selected.link_id = new_link_id;
}

void update_framehandle_location(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int chain_id, int link_id)
{
  zVec3D selected_point;
  double depth;

  /* the origin position of the selected link mode */
  zFrame3DCopy( rkChainLinkWldFrame( grs[chain_id].glChain.chain, link_id ), &g_fh.frame );

  /* ctrl key optional */
  if( rkgl_key_mod & GLFW_KEY_LEFT_CONTROL ){
    /* selected position mode */
    depth = rkglSelectionZnearDepth(sb);
    rkglUnproject( cam, x, y, depth, &selected_point );
    zFrame3DSetPos( &g_fh.frame, &selected_point );
    /* transform Wolrd -> Link frame */
    zXform3DInv( rkChainLinkWldFrame(&grs[chain_id].chain, link_id), zFrame3DPos(&g_fh.frame), &g_selected.ap );
  } else{
    zVec3DZero( &g_selected.ap );
  }
}

void switch_pin_link(int new_chain_id, int new_link_id)
{
  switch( grs[new_chain_id].info2[new_link_id].pin ){
  case PIN_LOCK_OFF:
    grs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_6D;
    break;
  case PIN_LOCK_6D:
    grs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_POS3D;
    break;
  case PIN_LOCK_POS3D:
    grs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_OFF;
    break;
  default: ;
  }
  /* reset for only pin link drawing */
  reset_link_drawing( new_chain_id, new_link_id );
  debug_printf("pin_link       : chain_id = %d, link_id = %d, pin status = %d\n", new_chain_id, new_link_id, grs[new_chain_id].info2[new_link_id].pin );
}

void register_one_link_for_IK(int chain_id, int link_id)
{
  rkIKAttr attr;
  attr.id = link_id;
  grs[chain_id].info2[link_id].cell[0] = rkChainRegIKCellWldAtt( &grs[chain_id].chain, &attr, RK_IK_ATTR_ID );
  grs[chain_id].info2[link_id].cell[1] = rkChainRegIKCellWldPos( &grs[chain_id].chain, &attr, RK_IK_ATTR_ID | RK_IK_ATTR_AP );
  switch( grs[chain_id].info2[link_id].pin ){
  case PIN_LOCK_6D:
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_POS3D:
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_OFF:
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( grs[chain_id].info2[link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    break;
  default: ;
  }
  zVec3DCopy( &g_selected.ap, rkIKCellAP(grs[chain_id].info2[link_id].cell[1]) );
}

void unregister_one_link_for_IK(int chain_id, int link_id)
{
  rkChainUnregIKCell( &grs[chain_id].chain, grs[chain_id].info2[link_id].cell[0] );
  rkChainUnregIKCell( &grs[chain_id].chain, grs[chain_id].info2[link_id].cell[1] );
}

void register_drag_weight_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  register_one_link_for_IK( drag_chain_id, drag_link_id );
  for( id=0; id < rkChainLinkNum( &grs[drag_chain_id].chain ); ++id ){
    if( id != drag_link_id && grs[drag_chain_id].info2[id].pin == PIN_LOCK_POS3D ){
      rkIKAttr attr;
      attr.id = id;
      grs[drag_chain_id].info2[id].cell[0] = rkChainRegIKCellWldAtt( &grs[drag_chain_id].chain, &attr, RK_IK_ATTR_ID );
      rkIKCellSetWeight( grs[drag_chain_id].info2[id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    }
  }
}

void unregister_drag_weight_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  unregister_one_link_for_IK( drag_chain_id, drag_link_id );
  for( id=0; id < rkChainLinkNum( &grs[drag_chain_id].chain ); ++id ){
    if( id != drag_link_id && grs[drag_chain_id].info2[id].pin == PIN_LOCK_POS3D )
      rkChainUnregIKCell( &grs[drag_chain_id].chain, grs[drag_chain_id].info2[id].cell[0] );
  }
}

void register_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &grs[drag_chain_id].chain ); ++id ){
    if( id == drag_link_id || grs[drag_chain_id].info2[id].pin != PIN_LOCK_OFF )
      register_one_link_for_IK( drag_chain_id, id );
  }
}

void unregister_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &grs[drag_chain_id].chain ); ++id ){
    if( id == drag_link_id || grs[drag_chain_id].info2[id].pin != PIN_LOCK_OFF )
      unregister_one_link_for_IK( drag_chain_id, id );
  }
}

void register_chain_for_CD(rkglChainBlock *glChainBlock)
{
  int i;
  for( i=0; i < g_chainNUM; ++i )
    rkCDChainReg( &g_cd, &glChainBlock[i].chain, RK_CD_CELL_MOVE );
}

void unregister_chain_for_CD(rkglChainBlock *glChainBlock)
{
  int i;
  for( i=0; i < g_chainNUM; ++i )
    rkCDPairChainUnreg( &g_cd, &glChainBlock[i].chain );
}

int get_chain_num(rkChain *chain)
{
  int i;
  for( i=0; i < g_chainNUM; ++i )
    if( &grs[i].chain == chain ) return i;
  return -1;
}

int get_link_num(rkChain *chain, rkLink* link)
{
  int l;
  return ( ( ( l=( link - rkChainRoot(chain) ) ) < rkChainLinkNum(chain) ) ? l : -1 );
}

bool is_collision_detected()
{
  rkCDPair* cp;
  bool is_collision = false;
  cdInfoCell *cdcell;
  zListForEachRew( &g_cdlist, cdcell ){
    reset_link_drawing( cdcell->data.chain_id, cdcell->data.link_id );
    grs[cdcell->data.chain_id].info2[cdcell->data.link_id].is_collision = false;
    grs[cdcell->data.chain_id].info2[cdcell->data.link_id].is_collision = false;
  }
  zListDestroy( cdInfoCell, &g_cdlist );
  rkCDColChkGJK( &g_cd );
  zListForEach( &(g_cd.plist), cp ){
    if ( cp->data.is_col ){
      cdInfoCell* cdcell0 = zAlloc( cdInfoCell, 1 );
      cdInfoCell* cdcell1 = zAlloc( cdInfoCell, 1 );
      cdcell0->data.chain_id = get_chain_num( cp->data.cell[0]->data.chain );
      cdcell1->data.chain_id = get_chain_num( cp->data.cell[1]->data.chain );
      cdcell0->data.link_id  = get_link_num( cp->data.cell[0]->data.chain, cp->data.cell[0]->data.link );
      cdcell1->data.link_id  = get_link_num( cp->data.cell[1]->data.chain, cp->data.cell[1]->data.link );
      reset_link_drawing( cdcell0->data.chain_id, cdcell0->data.link_id );
      reset_link_drawing( cdcell1->data.chain_id, cdcell1->data.link_id );
      grs[cdcell0->data.chain_id].info2[cdcell0->data.link_id].is_collision = true;
      grs[cdcell1->data.chain_id].info2[cdcell1->data.link_id].is_collision = true;
      zQueueEnqueue( &g_cdlist, cdcell0 );
      zQueueEnqueue( &g_cdlist, cdcell1 );
      debug_printf("Collision is %s : chain[%d] %s link[%d] %s %s : chain[%d] %s link[%d] %s %s\n",
        zBoolStr(cp->data.is_col),
        cdcell0->data.chain_id,
          zName(cp->data.cell[0]->data.chain),
          cdcell0->data.link_id,
          zName(cp->data.cell[0]->data.link), zName(cp->data.cell[0]->data.shape),
        cdcell1->data.chain_id,
          zName(cp->data.cell[1]->data.chain),
          cdcell1->data.link_id,
          zName(cp->data.cell[1]->data.link), zName(cp->data.cell[1]->data.shape) );
      is_collision = true;
    }
  }
  return is_collision;
}


/* inverse kinematics */
void update_alljoint_by_IK_with_frame(int drag_chain_id, int drag_link_id, zFrame3D *ref_frame)
{
  if( drag_link_id < 0 ) return;
  zVec dis; /* joints zVec pointer */
  dis = zVecAlloc( rkChainJointSize( &grs[drag_chain_id].chain ) );
  rkChainGetJointDisAll( &grs[drag_chain_id].chain, dis );
  /* prepare IK */
  rkChainDeactivateIK( &grs[drag_chain_id].chain );
  rkChainBindIK( &grs[drag_chain_id].chain );
  /* set rotation reference */
  if( grs[drag_chain_id].info2[drag_link_id].pin == PIN_LOCK_6D ||
      rkglFrameHandleIsInRotation( &g_fh ) ){
    zVec3D zyx;
    zMat3DToZYX( &(ref_frame->att), &zyx );
    rkIKCellSetRefVec( grs[drag_chain_id].info2[drag_link_id].cell[0], &zyx );
  }
  /* set position reference */
  rkIKCellSetRefVec( grs[drag_chain_id].info2[drag_link_id].cell[1], &(ref_frame->pos) );
  rkChainFK( &grs[drag_chain_id].chain, dis ); /* copy state to mentatin result consistency */
  /* IK */
  /* printf("pre IK Joint[deg]  = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  int iter = 100;
  double ztol = zTOL;
  /* backup in case the result of rkChainIK() is NaN */
  rkChain clone_chain;
  rkChainClone( &grs[drag_chain_id].chain, &clone_chain );
  rkChainIK( &grs[drag_chain_id].chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &grs[drag_chain_id].chain );
  }
  /* IK again with only pin link */
  unregister_drag_weight_link_for_IK( drag_chain_id, drag_link_id );
  rkChainIK( &grs[drag_chain_id].chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &grs[drag_chain_id].chain );
  }
  register_drag_weight_link_for_IK( drag_chain_id, drag_link_id );
  /* printf("post IK Joint[deg] = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  zVecFree(dis);

  /* keep FrameHandle position */
  if( rkglFrameHandleIsInRotation( &g_fh ) )
    zFrame3DCopy( rkChainLinkWldFrame( &grs[drag_chain_id].chain, drag_link_id ), &g_fh.frame );
  zXform3D( rkChainLinkWldFrame( &grs[drag_chain_id].chain, drag_link_id ), &g_selected.ap, zFrame3DPos( &g_fh.frame ) );
}

void print_status(void)
{
  int chain_id, link_id;
  printf("# Status ###################################################\n");
  for( chain_id=0; chain_id < g_chainNUM; chain_id++ ){
    printf("=== chain[%d] %s ========================================\n",
           chain_id, zName(&grs[chain_id].chain));
    printf("- Joint ( Size ( Joints [m,rad] ) ) ------------------------\n");
    zVec dis; /* joints zVec pointer */
    dis = zVecAlloc( rkChainJointSize( &grs[chain_id].chain ) );
    rkChainGetJointDisAll( &grs[chain_id].chain, dis );
    printf("  "); zVecPrint(dis); printf("\n");
    zVecFree( dis );
    /* printf("- Pin Link ---------------------------------------\n"); */
    for( link_id=0; link_id < rkChainLinkNum( &grs[chain_id].chain ); link_id++ ){
      if( grs[chain_id].info2[link_id].pin == PIN_LOCK_6D ){
        printf("- PIN_LOCK_6D : link[%d] %s ---------------------\n",
               link_id, zName( rkChainLink(&grs[chain_id].chain, link_id) ) );
        printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( &grs[chain_id].chain, link_id ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &grs[chain_id].chain, link_id ) );
        printf("\n");
      } else if( grs[chain_id].info2[link_id].pin == PIN_LOCK_6D ){
        printf("- PIN_LOCK_POS3D : link[%d] %s ------------------\n",
               link_id, zName( rkChainLink(&grs[chain_id].chain, link_id) ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &grs[chain_id].chain, link_id ) );
        printf("\n");
      }
    }
    /* printf("-------------------------------------------------------\n"); */
  }
  if( g_selected.chain_id >=0 && g_selected.link_id >=0 &&
      grs[g_selected.chain_id].info2[g_selected.link_id].is_selected ){
    printf("=== Drag(Select) : chain[%d] %s : link[%d] %s ================\n",
           g_selected.chain_id,
           zName( &grs[g_selected.chain_id].chain ),
           g_selected.link_id,
           zName( rkChainLink(&grs[g_selected.chain_id].chain, g_selected.link_id) ) );
    printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( &grs[g_selected.chain_id].chain, g_selected.link_id ) );
    printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &grs[g_selected.chain_id].chain, g_selected.link_id ) );
  }
  printf("############################################################\n\n\n");
}

void move_link(double angle)
{
  double dis;

  if( g_selected.link_id < 0 ) return;
  rkChainLinkJointGetDis( &grs[g_selected.chain_id].chain, g_selected.link_id, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &grs[g_selected.chain_id].chain, g_selected.link_id, &dis );
  rkChainUpdateFK( &grs[g_selected.chain_id].chain );
}

void motion(GLFWwindow* window, double x, double y)
{
  if( rkgl_mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
             !rkglFrameHandleIsUnselected( &g_fh ) ){
    /* moving mode */
    rkglFrameHandleMove( &g_fh, &g_cam, rkgl_mouse_x, rkgl_mouse_y );
    update_alljoint_by_IK_with_frame( g_selected.chain_id, g_selected.link_id, &g_fh.frame );
    g_is_collision = is_collision_detected();
  } else{
    rkglMouseDragFuncGLFW( window, x, y );
  }
  rkglMouseStoreXY( floor(x), floor(y) );
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  int new_chain_id = -1;
  int new_link_id = -1;
  rkglSelectionBuffer sb;

  /* store button when state == GLFW_PRESS */
  rkglMouseFuncGLFW( window, button, state, mods );

  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      /* draw only frame handle */
      if( rkglSelectNearest( &sb, &g_cam, draw_fh_parts, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          rkglFrameHandleAnchor( &g_fh, &sb, &g_cam, rkgl_mouse_x, rkgl_mouse_y ) >= 0 ){
        register_link_for_IK( g_selected.chain_id, g_selected.link_id );
      } else{
        rkglFrameHandleUnselect( &g_fh );
        /* draw only chain links */
        if( rkglSelectNearest( &sb, &g_cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
            is_select_chain_link( &sb, &new_chain_id, &new_link_id ) ){
          /* a link of a chain is selected. At the same time, new_chain_id >= 0. */
          update_framehandle_location( &sb, &g_cam, rkgl_mouse_x, rkgl_mouse_y, new_chain_id, new_link_id );
          if( !grs[new_chain_id].info2[new_link_id].is_selected ){
            reset_link_drawing( new_chain_id, new_link_id );
          }
          grs[new_chain_id].info2[new_link_id].is_selected = true;
        } else{
          /* no link is selected */
          int i, j;
          debug_printf("reset_all_link_selected_status(): reset all link\n");
          for( i=0; i < g_chainNUM; i++ ){
            for( j=0; j < rkChainLinkNum( &grs[i].chain ); j++ ){
              if( grs[i].info2[j].is_selected &&
                  grs[i].info2[j].pin == PIN_LOCK_OFF ){
                grs[i].info2[j].is_selected = false;
              }
              reset_link_drawing( i, j );
            } /* end of for j : 0 -> rkChainLinkNum */
          } /* end of for i : 0 -> g_chainNUM */
        } /* end of if-else chain link is selected */
        update_selected_chain_link( new_chain_id, new_link_id );
      } /* end of if-else FrameHandle is selected */
    } else if( state == GLFW_RELEASE ){
      if( !rkglFrameHandleIsUnselected( &g_fh ) ){
        unregister_link_for_IK( g_selected.chain_id, g_selected.link_id );
      }
      print_status();
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
      grs[g_selected.chain_id].info2[g_selected.link_id].is_selected = false;
      if( rkglSelectNearest( &sb, &g_cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          is_select_chain_link( &sb, &new_chain_id, &new_link_id ) ){
        switch_pin_link( new_chain_id, new_link_id );
        update_selected_chain_link( new_chain_id, new_link_id );
      }
    }
  }
  debug_printf( "selected       : link_id = %d, ", g_selected.link_id );
  debug_printf( "fh_parts_id = %d \n", g_fh.selected_id );
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
  int i;

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
    for( i=0; i < g_chainNUM; i++ ){
      rkglChainUnload_for_rkglChainBlock( &grs[i] );
      rkChainDestroy( &grs[i].chain );
      rkCDDestroy( &g_cd );
    }

    exit( EXIT_SUCCESS );
  default: ;
  }
}

char **g_modelfiles = NULL;

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
  int i;

  rkglSetDefaultCallbackParam( &g_cam, 1.0, g_znear, g_zfar, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 0, 0, 0, 45, -30, 0 );

  glEnable(GL_LIGHTING);
  rkglLightCreate( &g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
  rkglLightMove( &g_light, 3, 5, 9 );
  rkglLightSetAttenuationConst( &g_light, 1.0 );

  if( g_modelfiles == NULL ){
    g_chainNUM = 1;
    g_modelfiles = zAlloc( char*, g_chainNUM );
    g_modelfiles[0] = zStrClone( "../model/puma.ztk" );
  }
  grs = zAlloc( rkglChainBlock, g_chainNUM );
  for( i=0; i < g_chainNUM; i++ ){
    printf( "modelfile[%d] = %s\n", i, g_modelfiles[i] );
    if( !extend_rkChainReadZTK( &grs[i].chain, g_modelfiles[i] ) ){
      ZRUNWARN( "Failed extend_rkChainReadZTK()" );
      return false;
    }
    if( !rkglChainLoad_for_rkglChainBlock( &grs[i] ) ){
      ZRUNWARN( "Failed rkglChainLoad_for_rkglLinkInfo2()" );
      return false;
    }
    /* IK */
    rkChainCreateIK( &grs[i].chain );
    rkChainRegIKJointAll( &grs[i].chain, IK_DRAG_WEIGHT );
  } /* end of for i : 0 -> g_chainNUM */

  /* Collision Detection */
  zListInit( &g_cdlist );
  rkCDCreate( &g_cd );
  register_chain_for_CD( grs );

  /* select */
  g_selected.link_id = -1;
  /* frame handle (NAME_FRAMEHANDLE = g_chainNUM + NAME_FRAMEHANDLE_OFFSET) */
  rkglFrameHandleCreate( &g_fh, g_chainNUM + NAME_FRAMEHANDLE_OFFSET, g_LENGTH, g_MAGNITUDE );
  /* initialize the location of frame handle object */
  zFrame3DFromAA( &g_fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

  return true;
}

int main(int argc, char *argv[])
{
  int i;

  if( argc > 1 ){
    g_chainNUM = argc - 1;
    g_modelfiles = zAlloc( char*, g_chainNUM );
    for( i=0; i < g_chainNUM; i++ ) {
      g_modelfiles[i] = argv[i+1];
    }
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
