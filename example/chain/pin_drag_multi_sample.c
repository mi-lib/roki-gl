#include <roki/rk_cd.h>
#include <roki_gl/roki_glfw.h>

/* #define DEBUG_PRINT */
#ifdef DEBUG_PRINT
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

/* the definition of select & pin information *****************************************/

/* rkglLinkInfo2.pin information */
typedef enum{
  PIN_LOCK_OFF=-1,
  PIN_LOCK_6D,
  PIN_LOCK_POS3D
} pinStatus;

/* selected link alpha using create_pindrag_link_color() */
#define PINDRAG_SELECTED_ALPHA 0.3

typedef struct{
  pinStatus pin;
} pinInfo;

#define IK_CONSTRAINED_CELL_SIZE 2

typedef struct{
  bool is_selected;
  bool is_collision;
  pinStatus pin;
  rkIKCell *cell[IK_CONSTRAINED_CELL_SIZE];
  int cell_size;
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

/* end of the definition of select & pin information **********************************/

/* the main targets of this sample code */
typedef struct{
  rkglLinkInfo2 *info2; /* num chainlink */
  rkChain chain;
  rkglChain glChain;
} rkglChainBlock;

/* the collision detector & the result list */
typedef struct{
  int chain_id;
  int link_id;
} cdInfoCellDat;
zListClass( cdInfoCellList, cdInfoCell, cdInfoCellDat );

typedef enum{
  GHOST_MODE_OFF=-1,
  GHOST_MODE_READY,
  GHOST_MODE_ON
} ghostMode_t;

typedef struct{
  ghostMode_t mode;
  int chain_id;
  zVec phantom_q;
  pinInfo *phantom_pinfo;
  int phantom_display_id;
} ghostInfo;

typedef struct{
  char **modelfiles;
  selectInfo selected;
  rkglFrameHandle fh;
  rkglChainBlock *gcs; /* main data */
  int chainNUM;
  rkCD cd; /* collision detector */
  cdInfoCellList cdlist; /* the result list of collided chain & link IDs by rkCDColChk**() */
  bool is_sum_collision; /* simple summary result whether collision occured or not */
  ghostInfo ghost_info;
  int fixed_scene_display_id;
  /* viewing parameters */
  rkglCamera cam;
  rkglLight light;
  rkglShadow shadow;
} pindragIFData;

pindragIFData *g_main;

void copy_pindragIFData(void* _src, void* _dest)
{
  int chain_id, link_id, cell_id;

  pindragIFData* src = (pindragIFData*)(_src);
  pindragIFData* dest = (pindragIFData*)(_dest);
  for( chain_id=0; chain_id<src->chainNUM; chain_id++){
    /* copy char** modelfiles */
    dest->modelfiles[chain_id] = zStrClone( src->modelfiles[chain_id] );
    /* copy rkglChainBlock *gcs */
    /* copy rkChain */
    rkChain* src_chain = &src->gcs[chain_id].chain;
    rkChain* dest_chain = &dest->gcs[chain_id].chain;
    rkChainCopyState( src_chain, dest_chain );
    for( link_id=0; link_id < rkChainLinkNum(src_chain); link_id++ ){
      rkglLinkInfo *src_linkinfo = &src->gcs[chain_id].glChain.linkinfo[link_id];
      rkglLinkInfo *dest_linkinfo = &dest->gcs[chain_id].glChain.linkinfo[link_id];
      rkglLinkInfo2 *src_info2 = &src->gcs[chain_id].info2[link_id];
      rkglLinkInfo2 *dest_info2 = &dest->gcs[chain_id].info2[link_id];
      /* copy rkglLinkInfo */
      dest_linkinfo->visible      = src_linkinfo->visible;
      /* not copy display _list (GLint), because it must be unique for each keyframe */
      /* not copy display _list_backup (GLint), the reason is the same as _list */
      if( src_linkinfo->_optic_alt != NULL ){
        if( dest_linkinfo->_optic_alt == NULL ){
          dest_linkinfo->_optic_alt = zAlloc( zOpticalInfo, 1 );
          zOpticalInfoInit( dest_linkinfo->_optic_alt );
        }
        zOpticalInfoCopy( src_linkinfo->_optic_alt, dest_linkinfo->_optic_alt );
      }
      /* copy rkglLinkInfo2 */
      dest_info2->is_selected  = src_info2->is_selected;
      dest_info2->is_collision = src_info2->is_collision;
      dest_info2->pin          = src_info2->pin;
      dest_info2->cell_size    = src_info2->cell_size;
      /* copy rkIKCell */
      for( cell_id=0; cell_id < src_info2->cell_size; cell_id++){
        if( src_info2->cell[cell_id] != NULL ){
          if( dest_info2->cell[cell_id] == NULL ){
            dest_info2->cell[cell_id] = zAlloc( rkIKCell, 1 );
          }
          rkIKCellCopy( src_info2->cell[cell_id], dest_info2->cell[cell_id] );
        }
      }
    } /* end of for link_id = 0 -> rkChainLinkNum(src_chain) */
    /* copy GLuint name */
    dest->gcs[chain_id].glChain.name = src->gcs[chain_id].glChain.name;
    /* not copy rkChain pointer chain* to be left unchanged */
    /* copy rkglChainAttr attr */
    rkglChainAttrCopy(&src->gcs[chain_id].glChain.attr, &dest->gcs[chain_id].glChain.attr);
  } /* end of for chain_id = 0 -> chainNUM */
  /* copy selectInfo selected */
  dest->selected.chain_id = src->selected.chain_id;
  dest->selected.link_id   = src->selected.link_id;
  zVec3DCopy( &src->selected.ap, &dest->selected.ap );
  /* copy rkglFrameHandle fh */
  memcpy( dest->fh.part, src->fh.part, 6);
  zFrame3DCopy( &src->fh.frame, &dest->fh.frame );
  dest->fh.name = src->fh.name;
  dest->fh.selected_id = src->fh.selected_id;
  zVec3DCopy( &src->fh._anchor, &dest->fh._anchor);
  dest->fh._depth = src->fh._depth;
  /* not copy chainNum, it must be guaranteed to be the same at init() */
  /* not copy rkCD cd, because it is depends on the result of chain state */
  /* not copy cdInfoCellList cdlist, the reason is the same as rkCD cd */
  /* not copy bool is_sum_collision, the reason is the same as rkCD cd */
  /* not copy ghostInfo ghost_info, keep destination as initial state(GHOST_MODE_OFF) */
  /* not copy fixed_scene_display_id, set destination as initial state */
  /* copy rkglCamera cam */
  rkglCameraCopy( &src->cam, &dest->cam );
  /* not copy rkglLight light, it must be guaranteed to be the same at init() */
  /* not copy rkglShadow shadow, the reason is the same above light */
}

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

/* FrameHandle shape property */
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.7;

/* To avoid duplication between selected_link and selected_parts_id */
/* NAME_FRAMEHANDLE_OFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NAME_FRAMEHANDLE_OFFSET 100
#define NAME_FIXED_SCENE 1000

bool rkglChainLoad_for_rkglChainBlock(rkglChainBlock *gcb, rkglLight *light )
{
  int i;
  rkglChainAttr attr;
  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &gcb->glChain, &gcb->chain, &attr, light ) ){
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
    gcb->info2[i].cell[0] = NULL;
    gcb->info2[i].cell[1] = NULL;
    gcb->info2[i].cell_size = IK_CONSTRAINED_CELL_SIZE;
  }
  return true;
}

void rkglChainUnload_for_rkglChainBlock(rkglChainBlock *gcb)
{
  rkglChainUnload( &gcb->glChain );
  zFree( gcb->info2 );
}



/* draw part --------------------------------------------------------------------- */

void reset_link_drawing(int new_chain_id, int new_link_id)
{
  debug_printf( "reset link     : rkglChain[%d].linkinfo[%d] : list = %d, ", new_chain_id, new_link_id, g_main->gcs[new_chain_id].glChain.linkinfo[new_link_id].list );
  debug_printf( "_list_backup = %d, ---> ", g_main->gcs[new_chain_id].glChain.linkinfo[new_link_id]._list_backup );
  rkglChainResetLinkOptic( &g_main->gcs[new_chain_id].glChain, new_link_id );
  debug_printf( "list = %d, ", g_main->gcs[new_chain_id].glChain.linkinfo[new_link_id].list );
  debug_printf( "_list_backup = %d\n", g_main->gcs[new_chain_id].glChain.linkinfo[new_link_id]._list_backup );
}

/* draw FrameHandle parts shape */
void draw_fh_parts(void)
{
  if( g_main->selected.chain_id >=0 && g_main->selected.link_id >= 0 &&
      g_main->gcs[g_main->selected.chain_id].info2[g_main->selected.link_id].is_selected ){
    rkglFrameHandleDraw( &g_main->fh );
  }
}

void draw_original_chain_phantom(void)
{
  if( g_main->ghost_info.mode == GHOST_MODE_ON ){
    glCallList( g_main->ghost_info.phantom_display_id );
  }
}

void draw_chain(void)
{
  int i;

  for( i=0; i < g_main->chainNUM; i++ ){
    rkglChainSetName( &g_main->gcs[i].glChain, i ); /* NAME_CHAIN = i */
    rkglChainDraw( &g_main->gcs[i].glChain );
  }
}

void draw_alternate_link(rkglChain *gc, int chain_id, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
  debug_printf( "alternate link : rkglChain[%d].linkinfo[%d] : list = %d, ", chain_id, id, gc->info[id].list );
  debug_printf( "_list_backup = %d, ---> ", gc->linkinfo[id]._list_backup );
  /* TODO : reuse selected link list value */
  /* (the current implementation generates new list value by glNewList() ) */
  rkglChainAlternateLinkOptic( gc, id, oi_alt, light );
  debug_printf( "list = %d, ", gc->linkinfo[id].list );
  debug_printf( "_list_backup = %d\n", gc->linkinfo[id]._list_backup );
}

bool create_pindrag_link_color(int chain_id, int link_id, int pin, bool is_alt, bool is_selected, double alpha, zOpticalInfo* oi_alt)
{
  /* re-drawing the selected link once after proccessing with rkglChainResetLinkOptic() */
  if( is_alt ) g_main->gcs[chain_id].glChain.attr.disptype = RKGL_FACE;
  if( is_alt && pin == PIN_LOCK_6D ){
    /* Red & semi-transparent*/
    zOpticalInfoCreate( oi_alt, 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  } else if( is_alt && pin == PIN_LOCK_POS3D ){
    /* Yellow & semi-transparent*/
    zOpticalInfoCreate( oi_alt, 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  } else if( is_alt && is_selected ){
    /* Blue & semi-transparent */
    zOpticalInfoCreate( oi_alt, 0.5, 0.7, 1.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  } else {
    return false;
  }

  return true;
}

bool draw_pindrag_link(int chain_id, int link_id, bool is_alt)
{
  zOpticalInfo oi_alt;

  int pin = is_alt ? g_main->gcs[chain_id].info2[link_id].pin : -1;
  if( !create_pindrag_link_color(chain_id, link_id, pin, is_alt, g_main->gcs[chain_id].info2[link_id].is_selected, PINDRAG_SELECTED_ALPHA, &oi_alt) )
    return false;
  draw_alternate_link( &g_main->gcs[chain_id].glChain, chain_id, link_id, &oi_alt, &g_main->gcs[chain_id].glChain.attr, &g_main->light );

  return true;
}

void draw_collision_link(void)
{
  zOpticalInfo oi_alt;
  debug_printf("\n\n is_collision == true ================================= \n");
  /* Red */
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  cdInfoCell *cdcell;
  zListForEachRew( &g_main->cdlist, cdcell ){
    if( g_main->gcs[cdcell->data.chain_id].glChain.linkinfo[cdcell->data.link_id]._list_backup==-1 ){
      g_main->gcs[cdcell->data.chain_id].glChain.attr.disptype = RKGL_FACE;
      draw_alternate_link( &g_main->gcs[cdcell->data.chain_id].glChain, cdcell->data.chain_id, cdcell->data.link_id, &oi_alt, &g_main->gcs[cdcell->data.chain_id].glChain.attr, &g_main->light );
    }
  }
  debug_printf("end of is_collision == true ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n\n");
}

void draw_all_chain_link(void)
{
  int chain_id, link_id;

  draw_collision_link();
  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){
    rkglChainDraw( &g_main->gcs[chain_id].glChain );
    for( link_id=0; link_id < rkChainLinkNum(g_main->gcs[chain_id].glChain.chain); link_id++ ){
      if( g_main->gcs[chain_id].glChain.linkinfo[link_id]._list_backup == -1 &&
          !g_main->gcs[chain_id].info2[link_id].is_collision )
        draw_pindrag_link( chain_id, link_id, true );
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
  /* 3rd, drawing phantom in ghost mode */
  draw_original_chain_phantom();
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_main->cam );
  rkglLightPut( &g_main->light );
  rkglClear();

  draw_scene();
}

void clear_display(void)
{
  rkglClear();
}


const int keep_fixed_scene_displayList(const double alpha)
{
  int chain_id, link_id;
  zOpticalInfo **oi_alt;
  rkChain* chain;
  rkglChain* glChain;

  if( g_main->fixed_scene_display_id > 0 ){
    printf("glDeleteLists( %d, 1 )\n", g_main->fixed_scene_display_id);
    glDeleteLists( g_main->fixed_scene_display_id, 1 );
  }

  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){

    chain = &g_main->gcs[chain_id].chain;
    glChain = &g_main->gcs[chain_id].glChain;

    if( !( oi_alt = zAlloc( zOpticalInfo*, rkChainLinkNum(chain) ) ) ){
      ZALLOCERROR();
      ZRUNERROR( "Failed to zAlloc( zOpticalInfo, rkChainLinkNum(&g_chain) )." );
      return -1;
    }
    /* pin link color changed */
    for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ ){
      rkglLinkInfo2 *info2 = &g_main->gcs[chain_id].info2[link_id];
      oi_alt[link_id] = zAlloc( zOpticalInfo, 1 );
      if( !create_pindrag_link_color( chain_id, link_id, info2->pin, true, info2->is_selected, alpha, oi_alt[link_id] ) ){
        zOpticalInfoDestroy( oi_alt[link_id] );
        oi_alt[link_id] = NULL;
      }
      rkglChainAlternateLinkOptic( glChain, link_id, oi_alt[link_id], &g_main->light );
    } /* end of pin link color changed */

    if( chain_id==0 )
      g_main->fixed_scene_display_id = rkglBeginList();
    rkglChainPhantomize( glChain, alpha, &g_main->light );

    for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ )
      if( oi_alt[link_id] ) zOpticalInfoDestroy( oi_alt[link_id] );

  } /* end of for loop chain_id=0 -> chainNUM */
  glEndList();

  printf("keep_fixed_scene_displayList(%f)=%d\n", alpha, g_main->fixed_scene_display_id);
  return g_main->fixed_scene_display_id;
}

const int get_fixed_scene_display_id(void)
{
  return g_main->fixed_scene_display_id;
}

void set_top_name_for_fixed_scene(void)
{
  glLoadName(NAME_FIXED_SCENE);
}

/* The Input argument display_id is taken into account the case to draw other scene */
void draw_fixed_scene(const int display_id, const int name_id)
{
  glPushName( name_id );
  glCallList( display_id );
  glPopName();
}

int find_fixed_scene(void (* draw_all_fixed_scene)(void))
{
  if( g_main->selected.chain_id >=0 && g_main->selected.link_id >=0 &&
      g_main->gcs[g_main->selected.chain_id].info2[g_main->selected.link_id].is_selected ){
    return -1;
  }

  int selected_id = -1;
  rkglSelectionBuffer sb;
  /* (!) In its original usage, rkglSelectNearest() should be called with mouse() */
  if( rkglSelectNearest( &sb, &g_main->cam, draw_all_fixed_scene, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) ){
    selected_id =
    ( rkglSelectionName(&sb,0) == NAME_FIXED_SCENE ) ?
    rkglSelectionName(&sb,1) : -1;
  }
  return selected_id;
}

/* end of draw part -------------------------------------------------------------- */

bool is_selected_chain_link(rkglSelectionBuffer *sb, int *chain_id, int *link_id)
{
  int i;

  *link_id = -1;
  *chain_id = -1;
  for( i=0; i < g_main->chainNUM; i++ ){
    if( ( *link_id = rkglChainLinkFindSelected( &g_main->gcs[i].glChain, sb ) ) >= 0 ){
      *chain_id = i;
      return true;
    }
  }
  return false;
}

void update_selected_chain_link(int new_chain_id, int new_link_id)
{
  g_main->selected.chain_id = new_chain_id;
  g_main->selected.link_id = new_link_id;
}

void update_framehandle_location(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int chain_id, int link_id)
{
  zVec3D selected_point;
  double depth;

  /* the origin position of the selected link mode */
  zFrame3DCopy( rkChainLinkWldFrame( g_main->gcs[chain_id].glChain.chain, link_id ), &g_main->fh.frame );

  /* ctrl key optional */
  if( rkgl_key_mod & GLFW_KEY_LEFT_CONTROL ){
    /* selected position mode */
    depth = rkglSelectionZnearDepth(sb);
    rkglUnproject( cam, x, y, depth, &selected_point );
    zFrame3DSetPos( &g_main->fh.frame, &selected_point );
    /* transform Wolrd -> Link frame */
    zXform3DInv( rkChainLinkWldFrame(&g_main->gcs[chain_id].chain, link_id), zFrame3DPos(&g_main->fh.frame), &g_main->selected.ap );
  } else{
    zVec3DZero( &g_main->selected.ap );
  }
}

void switch_pin_link(int new_chain_id, int new_link_id)
{
  switch( g_main->gcs[new_chain_id].info2[new_link_id].pin ){
  case PIN_LOCK_OFF:
    g_main->gcs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_6D;
    break;
  case PIN_LOCK_6D:
    g_main->gcs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_POS3D;
    break;
  case PIN_LOCK_POS3D:
    g_main->gcs[new_chain_id].info2[new_link_id].pin = PIN_LOCK_OFF;
    break;
  default: ;
  }
  /* reset for only pin link drawing */
  reset_link_drawing( new_chain_id, new_link_id );
  debug_printf("pin_link       : chain_id = %d, link_id = %d, pin status = %d\n", new_chain_id, new_link_id, g_main->gcs[new_chain_id].info2[new_link_id].pin );
}

void register_one_link_for_IK(int chain_id, int link_id)
{
  rkIKAttr attr;
  attr.id = link_id;
  g_main->gcs[chain_id].info2[link_id].cell[0] = rkChainRegIKCellWldAtt( &g_main->gcs[chain_id].chain, &attr, RK_IK_ATTR_ID );
  g_main->gcs[chain_id].info2[link_id].cell[1] = rkChainRegIKCellWldPos( &g_main->gcs[chain_id].chain, &attr, RK_IK_ATTR_ID | RK_IK_ATTR_AP );
  switch( g_main->gcs[chain_id].info2[link_id].pin ){
  case PIN_LOCK_6D:
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_POS3D:
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_OFF:
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[1], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    break;
  default: ;
  }
  zVec3DCopy( &g_main->selected.ap, rkIKCellAP(g_main->gcs[chain_id].info2[link_id].cell[1]) );
}

void unregister_one_link_for_IK(int chain_id, int link_id)
{
  rkChainUnregIKCell( &g_main->gcs[chain_id].chain, g_main->gcs[chain_id].info2[link_id].cell[0] );
  rkChainUnregIKCell( &g_main->gcs[chain_id].chain, g_main->gcs[chain_id].info2[link_id].cell[1] );
}

void register_drag_weight_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  register_one_link_for_IK( drag_chain_id, drag_link_id );
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    if( id != drag_link_id && g_main->gcs[drag_chain_id].info2[id].pin == PIN_LOCK_POS3D ){
      rkIKAttr attr;
      attr.id = id;
      g_main->gcs[drag_chain_id].info2[id].cell[0] = rkChainRegIKCellWldAtt( &g_main->gcs[drag_chain_id].chain, &attr, RK_IK_ATTR_ID );
      rkIKCellSetWeight( g_main->gcs[drag_chain_id].info2[id].cell[0], IK_DRAG_WEIGHT, IK_DRAG_WEIGHT, IK_DRAG_WEIGHT );
    }
  }
}

void unregister_drag_weight_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  unregister_one_link_for_IK( drag_chain_id, drag_link_id );
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    if( id != drag_link_id && g_main->gcs[drag_chain_id].info2[id].pin == PIN_LOCK_POS3D )
      rkChainUnregIKCell( &g_main->gcs[drag_chain_id].chain, g_main->gcs[drag_chain_id].info2[id].cell[0] );
  }
}

void register_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    if( id == drag_link_id || g_main->gcs[drag_chain_id].info2[id].pin != PIN_LOCK_OFF )
      register_one_link_for_IK( drag_chain_id, id );
  }
}

void unregister_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    if( id == drag_link_id || g_main->gcs[drag_chain_id].info2[id].pin != PIN_LOCK_OFF )
      unregister_one_link_for_IK( drag_chain_id, id );
  }
}

void register_chain_for_CD(rkglChainBlock *glChainBlock)
{
  int i;
  for( i=0; i < g_main->chainNUM; ++i )
    rkCDChainReg( &g_main->cd, &glChainBlock[i].chain, RK_CD_CELL_MOVE );
}

void unregister_chain_for_CD(rkglChainBlock *glChainBlock)
{
  int i;
  for( i=0; i < g_main->chainNUM; ++i )
    rkCDPairChainUnreg( &g_main->cd, &glChainBlock[i].chain );
}

int get_chain_num(rkChain *chain)
{
  int i;
  for( i=0; i < g_main->chainNUM; ++i )
    if( &g_main->gcs[i].chain == chain ) return i;
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
  zListForEachRew( &g_main->cdlist, cdcell ){
    reset_link_drawing( cdcell->data.chain_id, cdcell->data.link_id );
    g_main->gcs[cdcell->data.chain_id].info2[cdcell->data.link_id].is_collision = false;
    g_main->gcs[cdcell->data.chain_id].info2[cdcell->data.link_id].is_collision = false;
  }
  zListDestroy( cdInfoCell, &g_main->cdlist );
  rkCDColChkGJK( &g_main->cd );
  zListForEach( &(g_main->cd.plist), cp ){
    if ( cp->data.is_col ){
      cdInfoCell* cdcell0 = zAlloc( cdInfoCell, 1 );
      cdInfoCell* cdcell1 = zAlloc( cdInfoCell, 1 );
      cdcell0->data.chain_id = get_chain_num( cp->data.cell[0]->data.chain );
      cdcell1->data.chain_id = get_chain_num( cp->data.cell[1]->data.chain );
      cdcell0->data.link_id  = get_link_num( cp->data.cell[0]->data.chain, cp->data.cell[0]->data.link );
      cdcell1->data.link_id  = get_link_num( cp->data.cell[1]->data.chain, cp->data.cell[1]->data.link );
      reset_link_drawing( cdcell0->data.chain_id, cdcell0->data.link_id );
      reset_link_drawing( cdcell1->data.chain_id, cdcell1->data.link_id );
      g_main->gcs[cdcell0->data.chain_id].info2[cdcell0->data.link_id].is_collision = true;
      g_main->gcs[cdcell1->data.chain_id].info2[cdcell1->data.link_id].is_collision = true;
      zQueueEnqueue( &g_main->cdlist, cdcell0 );
      zQueueEnqueue( &g_main->cdlist, cdcell1 );
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
void update_alljoint_by_IK_with_frame(int drag_chain_id, int drag_link_id, zVec init_joints, zFrame3D *ref_frame)
{
  /* prepare IK */
  rkChainDeactivateIK( &g_main->gcs[drag_chain_id].chain );
  rkChainBindIK( &g_main->gcs[drag_chain_id].chain );
  if( init_joints != NULL ) {
    rkChainSetJointDisAll( &g_main->gcs[drag_chain_id].chain, init_joints );
    rkChainUpdateFK( &g_main->gcs[drag_chain_id].chain );
  }
  if( ref_frame != NULL ) {
    if( drag_link_id < 0 ) return;
    /* set rotation reference */
    if( g_main->gcs[drag_chain_id].info2[drag_link_id].pin == PIN_LOCK_6D ||
        rkglFrameHandleIsInRotation( &g_main->fh ) ){
      zVec3D zyx;
      zMat3DToZYX( &(ref_frame->att), &zyx );
      rkIKCellSetRefVec( g_main->gcs[drag_chain_id].info2[drag_link_id].cell[0], &zyx );
    }
    /* set position reference */
    rkIKCellSetRefVec( g_main->gcs[drag_chain_id].info2[drag_link_id].cell[1], &(ref_frame->pos) );
  }
  /* IK */
  /* printf("pre IK Joint[deg]  = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  int iter = 100;
  double ztol = zTOL;
  /* backup in case the result of rkChainIK() is NaN */
  rkChain clone_chain;
  rkChainClone( &g_main->gcs[drag_chain_id].chain, &clone_chain );
  zVec dis = zVecAlloc( rkChainJointSize( &g_main->gcs[drag_chain_id].chain ) ); /* IK output */
  rkChainIK( &g_main->gcs[drag_chain_id].chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, &g_main->gcs[drag_chain_id].chain );
  }
  /* IK again with only pin link */
  if( ref_frame != NULL ){
    unregister_drag_weight_link_for_IK( drag_chain_id, drag_link_id );
    rkChainIK( &g_main->gcs[drag_chain_id].chain, dis, ztol, iter );
    if( zVecIsNan(dis) ){
      printf("the result of rkChainIK() is NaN\n");
      rkChainCopyState( &clone_chain, &g_main->gcs[drag_chain_id].chain );
    }
    register_drag_weight_link_for_IK( drag_chain_id, drag_link_id );

    /* keep FrameHandle position */
    if( rkglFrameHandleIsInRotation( &g_main->fh ) )
      zFrame3DCopy( rkChainLinkWldFrame( &g_main->gcs[drag_chain_id].chain, drag_link_id ), &g_main->fh.frame );
    zXform3D( rkChainLinkWldFrame( &g_main->gcs[drag_chain_id].chain, drag_link_id ), &g_main->selected.ap, zFrame3DPos( &g_main->fh.frame ) );
  }
  /* printf("post IK Joint[deg] = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI)); */
  zVecFree(dis);
}


int create_original_chain_phantom(int chain_id, ghostInfo* backup_ghost_info)
{
  int pin, link_id, display_id;
  const double alpha = 0.3;
  zOpticalInfo **oi_alt;
  rkChain* chain = &g_main->gcs[chain_id].chain;
  if( !( oi_alt = zAlloc( zOpticalInfo*, rkChainLinkNum(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( zOpticalInfo, rkChainLinkNum(&g_main->gcs[%d].chain) ).", chain_id );
    return -1;
  }
  if( backup_ghost_info->phantom_pinfo != NULL ) zFree( backup_ghost_info->phantom_pinfo );
  if( !( backup_ghost_info->phantom_pinfo = zAlloc( pinInfo, rkChainLinkNum(&g_main->gcs[chain_id].chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( pinInfo, rkChainLinkNum(&g_main->gcs[%d].chain) ).", chain_id );
    return -1;
  }
  for( link_id=0; link_id<rkChainLinkNum( chain ); link_id++ ){
    /* create optical color info of phantom */
    oi_alt[link_id] = zAlloc( zOpticalInfo, 1 );
    pin = g_main->gcs[chain_id].info2[link_id].pin;
    /* pin color */
    if( !create_pindrag_link_color(chain_id, link_id, pin, true, false, alpha, oi_alt[link_id]) ){
      /* other is gray */
      /* zOpticalInfoCreate( oi_alt[link_id], 0.8, 0.8, 0.8,  0.6, 0.6, 0.6,  0.0, 0.0, 0.0,  0.0, 0.0, alpha, NULL ); */
      /* other is Transparency */
      zOpticalInfoDestroy( oi_alt[link_id] );
      reset_link_drawing( chain_id, link_id );
      oi_alt[link_id] = NULL;
    }
    rkglChainAlternateLinkOptic( &g_main->gcs[chain_id].glChain, link_id, oi_alt[link_id], &g_main->light );
    /* store the backup of pinInfo */
    backup_ghost_info->phantom_pinfo[link_id].pin = g_main->gcs[chain_id].info2[link_id].pin;
  }
  display_id = rkglBeginList();
  rkglChainPhantomize( &g_main->gcs[chain_id].glChain, alpha, &g_main->light );
  glEndList();

  /* store the backup of chain_id, q */
  backup_ghost_info->chain_id = chain_id;
  if( backup_ghost_info->phantom_q != NULL ) zVecFree( backup_ghost_info->phantom_q );
  if( !( backup_ghost_info->phantom_q = zVecAlloc( rkChainJointSize(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zVecAlloc( rkChainLinkNum(&g_main->gcs[%d].chain) ).", chain_id );
  }
  rkChainGetJointDisAll( chain, backup_ghost_info->phantom_q );

  /* clean up */
  for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ )
    if( oi_alt[link_id] ) zOpticalInfoDestroy( oi_alt[link_id] );

  return display_id;
}

void delete_original_chain_phantom(ghostInfo* backup_ghost_info)
{
  if( backup_ghost_info->phantom_display_id >= 0 ){
    glDeleteLists( backup_ghost_info->phantom_display_id, 1 );
    backup_ghost_info->phantom_display_id = -1;
  }
  if( backup_ghost_info->phantom_q != NULL ){
    zVecFree( backup_ghost_info->phantom_q );
    backup_ghost_info->phantom_q = NULL;
  }
  if( backup_ghost_info->phantom_pinfo != NULL ){
    zFree( backup_ghost_info->phantom_pinfo );
    backup_ghost_info->phantom_pinfo = NULL;
  }
}

void switch_ghost_mode(bool is_active)
{
  if( !is_active ){
    g_main->ghost_info.mode = GHOST_MODE_OFF;
    delete_original_chain_phantom( &g_main->ghost_info );
    return;
  }
  switch( g_main->ghost_info.mode ){
  case GHOST_MODE_OFF:
    printf(" >>> GHOST_MODE_OFF->READY \n");
    g_main->ghost_info.mode = GHOST_MODE_READY;
    delete_original_chain_phantom( &g_main->ghost_info );
    break;
  case GHOST_MODE_READY:
    printf(" >>> GHOST_MODE_READY->ON \n");
    g_main->ghost_info.mode = GHOST_MODE_ON;
    g_main->ghost_info.phantom_display_id = create_original_chain_phantom( g_main->selected.chain_id, &g_main->ghost_info );
    break;
  default: ;
  }
}

void restore_original_chain_phantom(ghostInfo* backup_ghost_info)
{
  if( backup_ghost_info->phantom_pinfo == NULL ) return;

  int link_id;
  int chain_id =  backup_ghost_info->chain_id;
  rkChain* chain = &g_main->gcs[chain_id].chain;

  /* restore q  */
  rkChainSetJointDisAll( chain, backup_ghost_info->phantom_q );
  rkChainUpdateFK( chain );

  /* register IK cell with backup q & pinInfo */
  for( link_id=0; link_id < rkChainLinkNum( chain ); ++link_id ){
    g_main->gcs[chain_id].info2[link_id].pin = backup_ghost_info->phantom_pinfo[link_id].pin;
    reset_link_drawing( chain_id, link_id );
    if( g_main->gcs[chain_id].info2[link_id].pin != PIN_LOCK_OFF ){
      register_one_link_for_IK( chain_id, link_id );
      debug_printf("restore & register g_main->gcs[%d].info2[%d].pin = %d\n", chain_id, link_id, g_main->gcs[chain_id].info2[link_id].pin);
    }
  }
}

void update_alljoint_by_IK_with_ghost()
{
  zVec ghost_q;
  rkChain* chain = &g_main->gcs[g_main->ghost_info.chain_id].chain;
  if( !( ghost_q = zVecAlloc( rkChainJointSize(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zVecAlloc( rkChainLinkNum(&g_main->gcs[%d].chain) ).", g_main->ghost_info.chain_id );
  }
  rkChainGetJointDisAll( chain, ghost_q );

  /* restore and register phantom pininfo for IK */
  restore_original_chain_phantom( &g_main->ghost_info );
  /* IK */
  update_alljoint_by_IK_with_frame( g_main->ghost_info.chain_id, -1, ghost_q, NULL );
  /* update_alljoint_by_IK_with_frame( g_main->ghost_info.chain_id, -1, g_main->ghost_info.phantom_q, NULL ); */
  unregister_link_for_IK( g_main->ghost_info.chain_id, -1 );
}

const int jointSize_of_chain(const int chain_id)
{
  return rkChainJointSize( &g_main->gcs[chain_id].chain );
}

zVec clone_q_state_zVec(const int chain_id)
{
  zVec dis; /* joints zVec pointer */
  dis = zVecAlloc( jointSize_of_chain( chain_id ) );
  rkChainGetJointDisAll( &g_main->gcs[chain_id].chain, dis );
  return dis;
}

double* clone_q_state_array(const int chain_id)
{
  zVec dis = clone_q_state_zVec( chain_id );
  return zArrayBuf(dis);
}

const int linkNum_of_chain(const int chain_id)
{
  return rkChainLinkNum( &g_main->gcs[chain_id].chain );
}

int* clone_pin_state_array(const int chain_id)
{
  int link_id;
  int* pin_id;

  pin_id = zAlloc( int, linkNum_of_chain( chain_id ) );
  for( link_id=0; link_id < rkChainLinkNum( &g_main->gcs[chain_id].chain ); link_id++ ){
    pin_id[link_id] = g_main->gcs[chain_id].info2[link_id].pin;
  }
  return pin_id;
}


void print_status(void)
{
  int chain_id, link_id;
  printf("# Status ###################################################\n");
  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){
    printf("=== chain[%d] %s ========================================\n",
           chain_id, zName(&g_main->gcs[chain_id].chain));
    printf("- Joint ( Size ( Joints [m,rad] ) ) ------------------------\n");
    zVec dis = clone_q_state_zVec( chain_id ); /* joints zVec pointer */
    printf("  "); zVecPrint(dis); printf("\n");
    zVecFree( dis );
    /* printf("- Pin Link ---------------------------------------\n"); */
    for( link_id=0; link_id < rkChainLinkNum( &g_main->gcs[chain_id].chain ); link_id++ ){
      if( g_main->gcs[chain_id].info2[link_id].pin == PIN_LOCK_6D ){
        printf("- PIN_LOCK_6D : link[%d] %s ---------------------\n",
               link_id, zName( rkChainLink(&g_main->gcs[chain_id].chain, link_id) ) );
        printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( &g_main->gcs[chain_id].chain, link_id ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &g_main->gcs[chain_id].chain, link_id ) );
        printf("\n");
      } else if( g_main->gcs[chain_id].info2[link_id].pin == PIN_LOCK_POS3D ){
        printf("- PIN_LOCK_POS3D : link[%d] %s ------------------\n",
               link_id, zName( rkChainLink(&g_main->gcs[chain_id].chain, link_id) ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &g_main->gcs[chain_id].chain, link_id ) );
        printf("\n");
      }
    }
    /* printf("-------------------------------------------------------\n"); */
  }
  if( g_main->selected.chain_id >=0 && g_main->selected.link_id >=0 &&
      g_main->gcs[g_main->selected.chain_id].info2[g_main->selected.link_id].is_selected ){
    printf("=== Drag(Select) : chain[%d] %s : link[%d] %s ================\n",
           g_main->selected.chain_id,
           zName( &g_main->gcs[g_main->selected.chain_id].chain ),
           g_main->selected.link_id,
           zName( rkChainLink(&g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id) ) );
    printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id ) );
    printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id ) );
  }
  printf("############################################################\n\n\n");
}

/* data handling ----------------------------------------------------------------- */

bool create_empty_pindragIFData(void** src)
{
  pindragIFData** main_ptr = (pindragIFData**)( src );
  if( !( *main_ptr = zAlloc( pindragIFData, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  (*main_ptr)->modelfiles = NULL;

  return true;
}

void set_pindragIFData(void* src)
{
  g_main = (pindragIFData*)( src );
}

void* get_pindragIFData()
{
  return (void *)( g_main );
}

void destroy_pindragIFData(void* src)
{
  int i;

  pindragIFData* main_ptr = (pindragIFData*)( src );
  for( i=0; i < main_ptr->chainNUM; i++ ){
    rkglChainUnload_for_rkglChainBlock( &main_ptr->gcs[i] );
    rkChainDestroy( &main_ptr->gcs[i].chain );
    rkCDDestroy( &main_ptr->cd );
    delete_original_chain_phantom( &main_ptr->ghost_info );
  }
  zFree( main_ptr->modelfiles );
  zFree( main_ptr );
}

/* end of data handling ---------------------------------------------------------- */


/* GLFW callback part ------------------------------------------------------------ */

void motion(GLFWwindow* window, double x, double y)
{
  if( rkgl_mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
             !rkglFrameHandleIsUnselected( &g_main->fh ) ){
    /* moving mode */
    rkglFrameHandleMove( &g_main->fh, &g_main->cam, rkgl_mouse_x, rkgl_mouse_y );
    switch_ghost_mode( g_main->ghost_info.mode != GHOST_MODE_OFF );
    /* IK */
    update_alljoint_by_IK_with_frame( g_main->selected.chain_id, g_main->selected.link_id, NULL, &g_main->fh.frame );
    /* Collision Detection */
    g_main->is_sum_collision = is_collision_detected();
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
      if( rkglSelectNearest( &sb, &g_main->cam, draw_fh_parts, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          rkglFrameHandleAnchor( &g_main->fh, &sb, &g_main->cam, rkgl_mouse_x, rkgl_mouse_y ) >= 0 ){
        register_link_for_IK( g_main->selected.chain_id, g_main->selected.link_id );
      } else{
        rkglFrameHandleUnselect( &g_main->fh );
        /* draw only chain links */
        if( rkglSelectNearest( &sb, &g_main->cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
            is_selected_chain_link( &sb, &new_chain_id, &new_link_id ) ){
          /* a link of a chain is selected. At the same time, new_chain_id >= 0. */
          update_framehandle_location( &sb, &g_main->cam, rkgl_mouse_x, rkgl_mouse_y, new_chain_id, new_link_id );
          if( !g_main->gcs[new_chain_id].info2[new_link_id].is_selected &&
              !g_main->gcs[new_chain_id].info2[new_link_id].is_collision ){
            reset_link_drawing( new_chain_id, new_link_id );
          }
          g_main->gcs[new_chain_id].info2[new_link_id].is_selected = true;
        } else{
          /* no link is selected */
          int i, j;
          debug_printf("reset_all_link_selected_status(): reset all link\n");
          for( i=0; i < g_main->chainNUM; i++ ){
            for( j=0; j < rkChainLinkNum( &g_main->gcs[i].chain ); j++ ){
              if( g_main->gcs[i].info2[j].is_selected &&
                  g_main->gcs[i].info2[j].pin == PIN_LOCK_OFF &&
                  !g_main->gcs[i].info2[j].is_collision ){
                g_main->gcs[i].info2[j].is_selected = false;
                reset_link_drawing( i, j );
              }
            } /* end of for j : 0 -> rkChainLinkNum */
          } /* end of for i : 0 -> g_main->chainNUM */
        } /* end of if-else chain link is selected */
        update_selected_chain_link( new_chain_id, new_link_id );
      } /* end of if-else FrameHandle is selected */
    } else if( state == GLFW_RELEASE ){
      if( !rkglFrameHandleIsUnselected( &g_main->fh ) ){
        unregister_link_for_IK( g_main->selected.chain_id, g_main->selected.link_id );
      }
      print_status();
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
      if( rkglSelectNearest( &sb, &g_main->cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          is_selected_chain_link( &sb, &new_chain_id, &new_link_id ) ){
        g_main->gcs[new_chain_id].info2[new_link_id].is_selected = false;
        switch_pin_link( new_chain_id, new_link_id );
        update_selected_chain_link( new_chain_id, new_link_id );
      }
    }
  }
  debug_printf( "selected       : link_id = %d, ", g_main->selected.link_id );
  debug_printf( "fh_parts_id = %d \n", g_main->fh.selected_id );
}

void mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
  if ( yoffset < 0 ) {
    g_scale -= 0.0001; rkglOrthoScaleH( &g_main->cam, g_scale, g_znear, g_zfar );
  } else if ( yoffset > 0 ) {
    g_scale += 0.0001; rkglOrthoScaleH( &g_main->cam, g_scale, g_znear, g_zfar );
  }
}

void resize(GLFWwindow* window, int w, int h)
{
  rkglVPCreate( &g_main->cam, 0, 0, w, h );
  rkglOrthoScaleH( &g_main->cam, g_scale, g_znear, g_zfar );
}

void move_link(double angle)
{
  double dis;

  if( g_main->selected.link_id < 0 ) return;
  rkChainLinkJointGetDis( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id, &dis );
  dis += angle;
  rkChainLinkJointSetDis( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id, &dis );
  rkChainUpdateFK( &g_main->gcs[g_main->selected.chain_id].chain );
}

void keyboard(GLFWwindow* window, unsigned int key)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &g_main->cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &g_main->cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &g_main->cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &g_main->cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &g_main->cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &g_main->cam, 0, 0,-5 ); break;
  case '8': g_scale += 0.0001; rkglOrthoScaleH( &g_main->cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.0001; rkglOrthoScaleH( &g_main->cam, g_scale, g_znear, g_zfar ); break;
  case 'g':
    if( g_main->ghost_info.mode == GHOST_MODE_ON ){
      update_alljoint_by_IK_with_ghost();
      switch_ghost_mode( false );
    } else{
      switch_ghost_mode( g_main->ghost_info.mode == GHOST_MODE_OFF ); /* OFF <-> READY */
    }
    break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'q': case 'Q': case '\033':
    destroy_pindragIFData( g_main );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

/* end of GLFW callback part ----------------------------------------------------- */

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

void set_modelfiles( char* modelfiles[], int size )
{
  int i;

  g_main->chainNUM = size;
  g_main->modelfiles = zAlloc( char*, g_main->chainNUM );
  for( i=0; i < g_main->chainNUM; i++ ) {
    g_main->modelfiles[i] = modelfiles[i];
  }
}

void setDefaultCallbackParam(void)
{
  rkglSetDefaultCallbackParam( &g_main->cam, 1.0, g_znear, g_zfar, 1.0, 5.0 );
}

void copyFromDefaultCamera(void)
{
  rkglCameraCopyDefault( &g_main->cam );
}

bool init(void)
{
  int i;

  rkglBGSet( &g_main->cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_main->cam, 0, 0, 0, 45, -30, 0 );

  if( rkglLightNum() == 0 ){
    glEnable(GL_LIGHTING);
    rkglLightCreate( &g_main->light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
    rkglLightMove( &g_main->light, 3, 5, 9 );
    rkglLightSetAttenuationConst( &g_main->light, 1.0 );
  }

  if( g_main->modelfiles == NULL ){
    g_main->chainNUM = 1;
    g_main->modelfiles = zAlloc( char*, g_main->chainNUM );
    g_main->modelfiles[0] = zStrClone( "../model/puma.ztk" );
  }
  g_main->gcs = zAlloc( rkglChainBlock, g_main->chainNUM );
  for( i=0; i < g_main->chainNUM; i++ ){
    printf( "modelfile[%d] = %s\n", i, g_main->modelfiles[i] );
    if( !extend_rkChainReadZTK( &g_main->gcs[i].chain, g_main->modelfiles[i] ) ){
      ZRUNWARN( "Failed extend_rkChainReadZTK()" );
      return false;
    }
    if( !rkglChainLoad_for_rkglChainBlock( &g_main->gcs[i], &g_main->light ) ){
      ZRUNWARN( "Failed rkglChainLoad_for_rkglLinkInfo2()" );
      return false;
    }
    /* IK */
    rkChainCreateIK( &g_main->gcs[i].chain );
    rkChainRegIKJointAll( &g_main->gcs[i].chain, IK_DRAG_WEIGHT );
  } /* end of for i : 0 -> g_main->chainNUM */

  /* Collision Detection */
  zListInit( &g_main->cdlist );
  rkCDCreate( &g_main->cd );
  register_chain_for_CD( g_main->gcs );

  /* select */
  g_main->selected.chain_id = 0;
  g_main->selected.link_id = -1;
  /* frame handle (NAME_FRAMEHANDLE = g_main->chainNUM + NAME_FRAMEHANDLE_OFFSET) */
  rkglFrameHandleCreate( &g_main->fh, g_main->chainNUM + NAME_FRAMEHANDLE_OFFSET, g_LENGTH, g_MAGNITUDE );
  /* initialize the location of frame handle object */
  zFrame3DFromAA( &g_main->fh.frame, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
  /* reset ghost mode */
  switch_ghost_mode(false);

  return true;
}


GLFWwindow* g_window;

int main(int argc, char *argv[])
{
  /* First of All, create pindragIFData g_mains */
  void* test_main_ptr = NULL;
  create_empty_pindragIFData( &test_main_ptr );
  set_pindragIFData( test_main_ptr );

  if( argc > 1 ){
    set_modelfiles( &argv[1], argc - 1 );
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

  setDefaultCallbackParam();

  if( !init() ){
    glfwTerminate();
    return 1;
  }
  resize( g_window, width, height );
  glfwSwapInterval(1);

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE ){
    display(g_window);
    glfwPollEvents();
    glfwSwapBuffers( g_window );
  }
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
