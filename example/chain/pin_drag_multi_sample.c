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
  PIN_LOCK_POS3D,
  PIN_LOCK_1AP,
  PIN_LOCK_2AP,
  PIN_LOCK_6D
} pinStatus;

/* selected link alpha using create_pindrag_link_color() */
#define PINDRAG_SELECTED_ALPHA 0.3

#define AP_MAX_NUM 2

typedef struct{
  pinStatus pin;
  zVec3D ap[AP_MAX_NUM];
  int ap_displaylist[AP_MAX_NUM];
  rkIKCell *ap_cell[AP_MAX_NUM];
  int ap_num; /* = AP_MAX_NUM */
  int ap_cnt;
} pinInfo;

#define IK_CONSTRAINED_CELL_SIZE 3

typedef struct{
  bool is_selected;
  bool is_collision;
  pinInfo pinfo;
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
  zVec pre_q; /* joint angles before step */
} rkglChainBlock;

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
  rkCDPairList cplist; /* the result list of collided chain & link IDs by rkCDColChk**() */
  bool is_sum_collision; /* simple summary result whether collision occured or not */
  bool is_collision_avoidance;
  ghostInfo ghost_info;
  int fixed_scene_display_id;
  bool is_visible;
} pindragIFData;

pindragIFData *g_main;

void copy_pindragIFData(void* _src, void* _dest)
{
  int chain_id, link_id, ap_id, cell_id;

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
      } else{
        dest_linkinfo->_optic_alt = NULL;
      }
      /* copy rkglLinkInfo2 */
      dest_info2->is_selected  = src_info2->is_selected;
      dest_info2->is_collision = src_info2->is_collision;
      dest_info2->pinfo.pin    = src_info2->pinfo.pin;
      for( ap_id=0; ap_id < src_info2->pinfo.ap_num; ap_id++ ){
        zVec3DCopy( &src_info2->pinfo.ap[ap_id], &dest_info2->pinfo.ap[ap_id] );
        dest_info2->pinfo.ap_displaylist[ap_id] = src_info2->pinfo.ap_displaylist[ap_id];
        if( src_info2->pinfo.ap_cell[ap_id] != NULL ){
          if( dest_info2->pinfo.ap_cell[ap_id] != NULL ){
            rkIKCellDestroy( dest_info2->pinfo.ap_cell[ap_id] );
            zFree( dest_info2->pinfo.ap_cell[ap_id] );
            dest_info2->pinfo.ap_cell[ap_id] = NULL;
          }
          dest_info2->pinfo.ap_cell[ap_id] = rkIKCellClone( src_info2->pinfo.ap_cell[ap_id] );
        } else {
          dest_info2->pinfo.ap_cell[ap_id] = NULL;
        }
      }
      dest_info2->pinfo.ap_cnt = src_info2->pinfo.ap_cnt;
      dest_info2->cell_size    = src_info2->cell_size;
      /* copy rkIKCell */
      for( cell_id=0; cell_id < src_info2->cell_size; cell_id++){
        if( src_info2->cell[cell_id] != NULL ){
          if( dest_info2->cell[cell_id] != NULL ){
            zNameFree( &dest_info2->cell[cell_id]->data );
            rkIKCellDestroy( dest_info2->cell[cell_id] );
            zFree( dest_info2->cell[cell_id] );
            dest_info2->cell[cell_id] = NULL;
          }
          dest_info2->cell[cell_id] = rkIKCellClone( src_info2->cell[cell_id] );
        } else{
          dest_info2->cell[cell_id] = NULL;
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
  /* not copy rkCDPairList cplist, the reason is the same as rkCD cd */
  /* not copy bool is_sum_collision, the reason is the same as rkCD cd */
  /* not copy ghostInfo ghost_info, keep destination as initial state(GHOST_MODE_OFF) */
  /* not copy fixed_scene_display_id, set destination as initial state */
  /* not copy is_visible, set destination as initial state */
}

/* viewing parameters */
static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
rkglCamera *g_cam;
rkglLight *g_light;
rkglShadow *g_shadow;
double *g_scale;

/* FrameHandle shape property */
static const double g_LENGTH = 0.2;
static const double g_MAGNITUDE = g_LENGTH * 0.7;
static const double g_AP_SIZE = g_LENGTH * 0.01;

/* To avoid duplication between selected_link and selected_parts_id */
/* NAME_FRAMEHANDLE_OFFSET must be enough large than rkChainLinkNum(gr.chain)  */
#define NAME_FRAMEHANDLE_OFFSET 100
#define NAME_AP 1000
#define NAME_FIXED_SCENE 10000

bool g_is_exit = false;

bool is_exit(void)
{
  return g_is_exit;
}

bool rkglChainLoad_for_rkglChainBlock(rkglChainBlock *gcb, rkglLight *light )
{
  int chain_id, ap_id, cell_id;
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
  for( chain_id=0; chain_id < rkChainLinkNum(&gcb->chain); chain_id++ ){
    gcb->info2[chain_id].is_selected = false;
    gcb->info2[chain_id].pinfo.pin = PIN_LOCK_OFF;
    gcb->info2[chain_id].pinfo.ap_num = AP_MAX_NUM;
    for( ap_id=0; ap_id < gcb->info2[chain_id].pinfo.ap_num; ap_id++ ){
      zVec3DZero( &gcb->info2[chain_id].pinfo.ap[ap_id] );
      gcb->info2[chain_id].pinfo.ap_cell[ap_id] = NULL;
    }
    gcb->info2[chain_id].pinfo.ap_cnt = 0;
    gcb->info2[chain_id].cell_size = IK_CONSTRAINED_CELL_SIZE;
    for( cell_id=0; cell_id < gcb->info2[chain_id].cell_size; cell_id++ ){
      gcb->info2[chain_id].cell[cell_id] = NULL;
    }
  }
  return true;
}

void rkglChainUnload_for_rkglChainBlock(rkglChainBlock *gcb)
{
  rkglChainUnload( &gcb->glChain );
  zFree( gcb->info2 );
}

/* utilized function -------------------------------------------------------------- */

int get_chain_id(rkChain *chain)
{
  int i;
  for( i=0; i < g_main->chainNUM; ++i )
    if( &g_main->gcs[i].chain == chain ) return i;
  return -1;
}

int get_link_id(rkChain *chain, rkLink* link)
{
  int l;
  return ( ( ( l=( link - rkChainRoot(chain) ) ) < rkChainLinkNum(chain) ) ? l : -1 );
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
  int chain_id;

  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){
    rkglChainSetName( &g_main->gcs[chain_id].glChain, chain_id ); /* NAME_CHAIN = chain_id */
    rkglChainDraw( &g_main->gcs[chain_id].glChain );
  }
}

void create_chain_ap_displaylist(int chain_id)
{
  zSphere3D sphere;
  zVec3D wld_ap_zero;
  zVec3DZero( &wld_ap_zero );
  ubyte DISPSWITCH = 1;
  int link_id, ap_id, display_id;

  for( link_id=0; link_id < rkChainLinkNum(g_main->gcs[chain_id].glChain.chain); link_id++ ){
    pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
    for( ap_id=0; ap_id < pinfo->ap_num; ap_id++ ){

      zSphere3DCreate( &sphere, &wld_ap_zero, g_AP_SIZE, 0 );

      display_id = rkglBeginList();
      zRGB color;
      zRGBSet( &color, 1.0, 1.0, 0.0 ); /* yellow */
      rkglRGB( &color );

      rkglSphere( &sphere, DISPSWITCH );

      glEndList();
      /* if(glGetError() != GL_NO_ERROR) */
      /*   printf("\n\n DISPLAY ERROR !!! \n\n"); */
      pinfo->ap_displaylist[ap_id] = display_id;
    }
  } /* end of for link_id */
}

void draw_ap(int chain_id, int link_id)
{
  int ap_id;
  zVec3D wld_ap;
  pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
  bool lighting_is_enabled;
  rkglSaveLighting( &lighting_is_enabled );
  for( ap_id=0; ap_id < pinfo->ap_cnt; ap_id++ ){
    glPushMatrix();
    zXform3D( rkChainLinkWldFrame( &g_main->gcs[chain_id].chain, link_id ), &pinfo->ap[ap_id], &wld_ap );
    rkglTranslate( &wld_ap );
    glLoadName( NAME_AP + pinfo->ap_displaylist[ap_id] ); /* ToDo : Rename more better */
    glPushName( 0 );
    glCallList( pinfo->ap_displaylist[ap_id] );
    glPopName();
    glPopMatrix();
  }
  rkglLoadLighting( lighting_is_enabled );
}

void draw_alternate_link(rkglChain *gc, int chain_id, int link_id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light){
  debug_printf( "alternate link : rkglChain[%d].linkinfo[%d] : list = %d, ", chain_id, link_id, gc->linkinfo[link_id].list );
  debug_printf( "_list_backup = %d, ---> ", gc->linkinfo[link_id]._list_backup );
  /* TODO : reuse selected link list value */
  /* (the current implementation generates new list value by glNewList() ) */
  rkglChainAlternateLinkOptic( gc, link_id, oi_alt, light );
  debug_printf( "list = %d, ", gc->linkinfo[link_id].list );
  debug_printf( "_list_backup = %d\n", gc->linkinfo[link_id]._list_backup );
}

bool create_pindrag_link_color(int chain_id, int link_id, int pin, bool is_alt, bool is_selected, double alpha, zOpticalInfo* oi_alt)
{
  /* re-drawing the selected link once after proccessing with rkglChainResetLinkOptic() */
  if( is_alt ) g_main->gcs[chain_id].glChain.attr.disptype = RKGL_FACE;
  if( is_alt && pin == PIN_LOCK_6D ){
    /* Red & semi-transparent*/
    zOpticalInfoCreate( oi_alt, 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  } else if( is_alt && pin == PIN_LOCK_2AP ){
    /* Safe Color(Pink) & semi-transparent*/
    zOpticalInfoCreate( oi_alt, 1.0, 0.2, 0.8, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
  } else if( is_alt && ( pin == PIN_LOCK_POS3D || pin == PIN_LOCK_1AP ) ){
    /* Yellow & semi-transparent*/
    zOpticalInfoCreate( oi_alt, 1.0, 0.8, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, alpha, NULL );
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

  int pin = is_alt ? g_main->gcs[chain_id].info2[link_id].pinfo.pin : -1;
  if( !create_pindrag_link_color(chain_id, link_id, pin, is_alt, g_main->gcs[chain_id].info2[link_id].is_selected, PINDRAG_SELECTED_ALPHA, &oi_alt) )
    return false;
  draw_alternate_link( &g_main->gcs[chain_id].glChain, chain_id, link_id, &oi_alt, &g_main->gcs[chain_id].glChain.attr, g_light );

  return true;
}

void draw_collision_link(void)
{
  int i, chain_id, link_id;
  zOpticalInfo oi_alt;
  /* Red */
  zOpticalInfoCreateSimple( &oi_alt, 1.0, 0.0, 0.0, NULL );
  rkCDPair* cp;
  zListForEachRew( &g_main->cplist, cp ){
    for( i=0; i < 2; i++ ){
      chain_id = get_chain_id( cp->data.cell[i]->data.chain );
      link_id  = get_link_id(  cp->data.cell[i]->data.chain, cp->data.cell[i]->data.link );
      rkglChain* glChain = &g_main->gcs[chain_id].glChain; /* for omission */
      if( glChain->linkinfo[link_id]._list_backup==-1 ){
        glChain->attr.disptype = RKGL_FACE;
        draw_alternate_link( glChain, chain_id, link_id, &oi_alt, &glChain->attr, g_light );
      }
    }
  } /* end of zListForEachRew( &g_main->cplist, cp ) */
}

void draw_all_chain_link(void)
{
  int chain_id, link_id;

  draw_collision_link();
  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){
    rkglChainDraw( &g_main->gcs[chain_id].glChain );
    for( link_id=0; link_id < rkChainLinkNum(g_main->gcs[chain_id].glChain.chain); link_id++ ){
      draw_ap( chain_id, link_id );
      if( g_main->gcs[chain_id].glChain.linkinfo[link_id]._list_backup == -1 &&
          !g_main->gcs[chain_id].info2[link_id].is_collision )
        draw_pindrag_link( chain_id, link_id, true );
    }
  }
}

void ready_scene(void)
{
  rkglCALoad( g_cam );
  rkglLightPut( g_light );
  rkglClear();
}

void draw_scene(void)
{
  if( !g_main->is_visible )
    return;
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
  ready_scene();
  draw_scene();
}

void clear_display(void)
{
  rkglClear();
}

bool is_visible(void){ return g_main->is_visible; }

void set_visible(void){ g_main->is_visible = true; }

void set_invisible(void){ g_main->is_visible = false; }

bool is_chain_visible(int chain_id)
{
  return g_main->gcs[chain_id].glChain.linkinfo->visible;
}

void set_chain_visible(int chain_id)
{
  g_main->gcs[chain_id].glChain.linkinfo->visible = true;
}

void set_chain_invisible(int chain_id)
{
  g_main->gcs[chain_id].glChain.linkinfo->visible = false;
}

const int keep_fixed_scene_displayList(const double alpha)
{
  int chain_id, link_id;
  zOpticalInfo **oi_alt;
  rkChain* chain;
  rkglChain* glChain;

  if( !g_main->is_visible )
    return -1;

  if( g_main->fixed_scene_display_id > 0 ){
    debug_printf("glDeleteLists( %d, 1 )\n", g_main->fixed_scene_display_id);
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

    draw_collision_link();
    /* pin link color changed */
    for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ ){
      if( g_main->gcs[chain_id].info2[link_id].is_collision )
        continue;
      rkglLinkInfo2 *info2 = &g_main->gcs[chain_id].info2[link_id];
      oi_alt[link_id] = zAlloc( zOpticalInfo, 1 );
      if( !create_pindrag_link_color( chain_id, link_id, info2->pinfo.pin, true, info2->is_selected, alpha, oi_alt[link_id] ) ){
        zOpticalInfoDestroy( oi_alt[link_id] );
        oi_alt[link_id] = NULL;
      }
      rkglChainAlternateLinkOptic( glChain, link_id, oi_alt[link_id], g_light );
    } /* end of pin link color changed */

    if( chain_id==0 )
      g_main->fixed_scene_display_id = rkglBeginList();
    rkglChainPhantomize( glChain, alpha, g_light );

    for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ )
      if( oi_alt[link_id] ) zOpticalInfoDestroy( oi_alt[link_id] );

  } /* end of for loop chain_id=0 -> chainNUM */
  glEndList();

  debug_printf("keep_fixed_scene_displayList(%f)=%d\n", alpha, g_main->fixed_scene_display_id);
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
  if( !g_main->is_visible )
    return;
  glPushName( name_id );
  glCallList( display_id );
  glPopName();
}

int find_fixed_scene(void (* draw_all_fixed_scene)(void))
{
  if( g_main->is_visible &&
      g_main->selected.chain_id >=0 && g_main->selected.link_id >=0 &&
      g_main->gcs[g_main->selected.chain_id].info2[g_main->selected.link_id].is_selected ){
    return -1;
  }

  int selected_id = -1;
  rkglSelectionBuffer sb;
  /* (!) In its original usage, rkglSelectNearest() should be called with mouse() */
  if( rkglSelectNearest( &sb, g_cam, draw_all_fixed_scene, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) ){
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

bool get_ap(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int chain_id, int link_id, zVec3D* selected_wld_ap, zVec3D* ap)
{
  double depth;

  /* ctrl key optional */
  if( rkgl_key_mod & GLFW_KEY_LEFT_CONTROL ){
    /* selected position mode */
    depth = rkglSelectionZnearDepth(sb);
    /* selected point is on Wolrd Frame*/
    rkglUnproject( cam, x, y, depth, selected_wld_ap );
    /* AP from transformed selected_point on Wolrd -> Link frame */
    zXform3DInv( rkChainLinkWldFrame(&g_main->gcs[chain_id].chain, link_id), selected_wld_ap, ap );
  } else{
    zVec3DZero( ap );
    return false;
  }

  return true;
}

void update_framehandle_location(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int chain_id, int link_id)
{
  zVec3D selected_wld_ap;

  /* the origin position of the selected link mode */
  zFrame3DCopy( rkChainLinkWldFrame( g_main->gcs[chain_id].glChain.chain, link_id ), &g_main->fh.frame );

  get_ap( sb, cam, x, y, chain_id, link_id, &selected_wld_ap, &g_main->selected.ap);

  /* ctrl key optional */
  if( rkgl_key_mod & GLFW_KEY_LEFT_CONTROL )
    zFrame3DSetPos( &g_main->fh.frame, &selected_wld_ap );
}

bool get_pin_ap(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y, int chain_id, int link_id)
{
  zVec3D selected_wld_ap;
  pinInfo *pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;

  int ap_id = ( pinfo->ap_cnt < 2 ) ? pinfo->ap_cnt : 1;
  bool is_ap_mode = get_ap( sb, cam, x, y, chain_id, link_id, &selected_wld_ap, &pinfo->ap[ap_id] );

  return is_ap_mode;
}

void switch_pin_link(int new_chain_id, int new_link_id, bool is_ap_mode)
{
  pinInfo *pinfo = &g_main->gcs[new_chain_id].info2[new_link_id].pinfo;
  switch( pinfo->pin ){
  case PIN_LOCK_OFF:
    if( !is_ap_mode ){
      pinfo->pin = PIN_LOCK_POS3D;
      pinfo->ap_cnt = 0;
    } else{
      pinfo->pin = PIN_LOCK_1AP;
      pinfo->ap_cnt = 1;
    }
    break;
  case PIN_LOCK_POS3D:
    if( !is_ap_mode ){
      pinfo->pin = PIN_LOCK_6D;
      pinfo->ap_cnt = 0;
    } else{
      pinfo->pin = PIN_LOCK_1AP;
      pinfo->ap_cnt = 1;
    }
    break;
  case PIN_LOCK_1AP:
    if( !is_ap_mode ){
      pinfo->pin = PIN_LOCK_6D;
      pinfo->ap_cnt = 0;
    } else{
      pinfo->pin = PIN_LOCK_2AP;
      pinfo->ap_cnt = 2;
    }
    break;
  case PIN_LOCK_2AP:
    if( !is_ap_mode ){
      pinfo->pin = PIN_LOCK_6D;
      pinfo->ap_cnt = 0;
    }
    break;
  case PIN_LOCK_6D:
    pinfo->pin = PIN_LOCK_OFF;
    pinfo->ap_cnt = 0;
    break;
  default: ;
  }
  /* reset for only pin link drawing */
  reset_link_drawing( new_chain_id, new_link_id );
  debug_printf("pin_link       : chain_id = %d, link_id = %d, pin status = %d\n", new_chain_id, new_link_id, g_main->gcs[new_chain_id].info2[new_link_id].pinfo.pin );
}

void register_att_link(int chain_id, int link_id, double weight)
{
  rkIKAttr attr;
  attr.id = link_id;
  g_main->gcs[chain_id].info2[link_id].cell[0] = rkChainRegisterIKCellWldAtt( &g_main->gcs[chain_id].chain, NULL, 1, &attr, RK_IK_ATTR_MASK_ID );
  rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[0], weight, weight, weight );
}

void register_pos_link(int chain_id, int link_id, double weight)
{
  rkIKAttr attr;
  attr.id = link_id;
  g_main->gcs[chain_id].info2[link_id].cell[1] = rkChainRegisterIKCellWldPos( &g_main->gcs[chain_id].chain, NULL, 1, &attr, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT );
  rkIKCellSetWeight( g_main->gcs[chain_id].info2[link_id].cell[1], weight, weight, weight );
  zVec3DCopy( &g_main->selected.ap, rkIKCellAttentionPoint( g_main->gcs[chain_id].info2[link_id].cell[1] ) );
}

void register_one_link_for_IK(int chain_id, int link_id, bool is_drag_link)
{
  switch( g_main->gcs[chain_id].info2[link_id].pinfo.pin ){
  case PIN_LOCK_POS3D:
    if( is_drag_link ){
      register_att_link( chain_id, link_id, IK_DRAG_WEIGHT );
    }
    register_pos_link( chain_id, link_id, IK_PIN_WEIGHT );
    break;
  case PIN_LOCK_6D:
    register_att_link( chain_id, link_id, IK_PIN_WEIGHT );
    register_pos_link( chain_id, link_id, IK_PIN_WEIGHT );
    break;
  default:
    if( is_drag_link ){
      register_att_link( chain_id, link_id, IK_DRAG_WEIGHT );
      register_pos_link( chain_id, link_id, IK_DRAG_WEIGHT );
    }
    break;
  }
}

void unregister_one_link_for_IK(int chain_id, int link_id, bool is_drag_link)
{
  pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
  if( is_drag_link || pinfo->pin == PIN_LOCK_6D ){
    rkChainUnregisterAndDestroyIKCell( &g_main->gcs[chain_id].chain, g_main->gcs[chain_id].info2[link_id].cell[0] );
    g_main->gcs[chain_id].info2[link_id].cell[0] = NULL;
  }
  if( is_drag_link || pinfo->pin == PIN_LOCK_POS3D || pinfo->pin == PIN_LOCK_6D ){
    rkChainUnregisterAndDestroyIKCell( &g_main->gcs[chain_id].chain, g_main->gcs[chain_id].info2[link_id].cell[1] );
    g_main->gcs[chain_id].info2[link_id].cell[1] = NULL;
  }
}

void register_ap_in_one_link_for_IK(int chain_id, int link_id)
{
  int ap_id;
  pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
  for( ap_id=0; ap_id < pinfo->ap_cnt; ap_id++ ){
    rkIKAttr ap_attr;
    ap_attr.id = link_id;
    pinfo->ap_cell[ap_id] = rkChainRegisterIKCellWldPos( &g_main->gcs[chain_id].chain, NULL, 1, &ap_attr, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT );
    rkIKCellSetWeight( pinfo->ap_cell[ap_id], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
    zVec3DCopy( &pinfo->ap[ap_id], rkIKCellAttentionPoint( pinfo->ap_cell[ap_id] ) );
  }
 }

void unregister_ap_in_one_link_for_IK(int chain_id, int link_id)
{
  int ap_id;
  pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
  for( ap_id = 0; ap_id < pinfo->ap_cnt; ap_id++ ){
    rkChainUnregisterAndDestroyIKCell( &g_main->gcs[chain_id].chain, pinfo->ap_cell[ap_id] );
    pinfo->ap_cell[ap_id] = NULL;
  }
}

void register_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    pinInfo* pinfo = &g_main->gcs[drag_chain_id].info2[id].pinfo;
    if( id == drag_link_id ||
        pinfo->pin == PIN_LOCK_POS3D ||
        pinfo->pin == PIN_LOCK_6D )
      register_one_link_for_IK( drag_chain_id, id, (id == drag_link_id) );
    if( pinfo->pin == PIN_LOCK_1AP ||
        pinfo->pin == PIN_LOCK_2AP )
      register_ap_in_one_link_for_IK( drag_chain_id, id );
  }
}

void unregister_link_for_IK(int drag_chain_id, int drag_link_id)
{
  int id;
  for( id=0; id < rkChainLinkNum( &g_main->gcs[drag_chain_id].chain ); ++id ){
    pinInfo* pinfo = &g_main->gcs[drag_chain_id].info2[id].pinfo;
    if( id == drag_link_id ||
        pinfo->pin == PIN_LOCK_POS3D ||
        pinfo->pin == PIN_LOCK_6D )
      unregister_one_link_for_IK( drag_chain_id, id, (id == drag_link_id) );
    if( pinfo->pin == PIN_LOCK_1AP ||
        pinfo->pin == PIN_LOCK_2AP )
      unregister_ap_in_one_link_for_IK( drag_chain_id, id );
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

bool is_collision_detected()
{
  int chain0_id, link0_id, chain1_id, link1_id;
  rkCDPair* cp;
  bool is_collision = false;
  zListForEachRew( &g_main->cplist, cp ){
    chain0_id = get_chain_id( cp->data.cell[0]->data.chain );
    link0_id  = get_link_id(  cp->data.cell[0]->data.chain, cp->data.cell[0]->data.link );
    reset_link_drawing( chain0_id, link0_id );
    chain1_id = get_chain_id( cp->data.cell[1]->data.chain );
    link1_id  = get_link_id(  cp->data.cell[1]->data.chain, cp->data.cell[1]->data.link );
    reset_link_drawing( chain1_id, link1_id );
    g_main->gcs[chain0_id].info2[link0_id].is_collision = false;
    g_main->gcs[chain1_id].info2[link1_id].is_collision = false;
  }
  zListDestroy( rkCDPair, &g_main->cplist );
  rkCDColChkGJK( &g_main->cd );
  zListForEach( &(g_main->cd.plist), cp ){
    if ( cp->data.is_col ){
      chain0_id = get_chain_id( cp->data.cell[0]->data.chain );
      link0_id  = get_link_id(  cp->data.cell[0]->data.chain, cp->data.cell[0]->data.link );
      reset_link_drawing( chain0_id, link0_id );
      chain1_id = get_chain_id( cp->data.cell[1]->data.chain );
      link1_id  = get_link_id(  cp->data.cell[1]->data.chain, cp->data.cell[1]->data.link );
      reset_link_drawing( chain1_id, link1_id );
      g_main->gcs[chain0_id].info2[link0_id].is_collision = true;
      g_main->gcs[chain1_id].info2[link1_id].is_collision = true;
      rkCDPair* cp_new = zAlloc( rkCDPair, 1 );
      zCopy( rkCDPairDat, &cp->data, &cp_new->data );
      zQueueEnqueue( &g_main->cplist, cp_new );
      debug_printf("Collision is %s : chain[%d] %s link[%d] %s %s : chain[%d] %s link[%d] %s %s\n",
        zBoolStr(cp->data.is_col),
        chain0_id,
          zName(cp->data.cell[0]->data.chain),
          link0_id,
          zName(cp->data.cell[0]->data.link), zName(cp->data.cell[0]->data.shape),
        chain1_id,
          zName(cp->data.cell[1]->data.chain),
          link1_id,
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
  rkChainDisableIK( &g_main->gcs[drag_chain_id].chain );
  rkChainBindIK( &g_main->gcs[drag_chain_id].chain );
  if( init_joints != NULL ) {
    rkChainSetJointDisAll( &g_main->gcs[drag_chain_id].chain, init_joints );
    rkChainUpdateFK( &g_main->gcs[drag_chain_id].chain );
  }
  if( ref_frame != NULL ) {
    if( drag_link_id < 0 ) return;
    /* set rotation reference */
    if( g_main->gcs[drag_chain_id].info2[drag_link_id].pinfo.pin == PIN_LOCK_6D ||
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
  /* keep joint angles before step */
  rkChainGetJointDisAll( &g_main->gcs[drag_chain_id].chain, g_main->gcs[drag_chain_id].pre_q );
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
    unregister_one_link_for_IK( drag_chain_id, drag_link_id, true );
    rkChainIK( &g_main->gcs[drag_chain_id].chain, dis, ztol, iter );
    if( zVecIsNan(dis) ){
      printf("the result of rkChainIK() is NaN\n");
      rkChainCopyState( &clone_chain, &g_main->gcs[drag_chain_id].chain );
    }
    register_one_link_for_IK( drag_chain_id, drag_link_id, true );
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
    pin = g_main->gcs[chain_id].info2[link_id].pinfo.pin;
    /* pin color */
    if( !create_pindrag_link_color(chain_id, link_id, pin, true, false, alpha, oi_alt[link_id]) ){
      /* other is gray */
      /* zOpticalInfoCreate( oi_alt[link_id], 0.8, 0.8, 0.8,  0.6, 0.6, 0.6,  0.0, 0.0, 0.0,  0.0, 0.0, alpha, NULL ); */
      /* other is Transparency */
      zOpticalInfoDestroy( oi_alt[link_id] );
      reset_link_drawing( chain_id, link_id );
      oi_alt[link_id] = NULL;
    }
    rkglChainAlternateLinkOptic( &g_main->gcs[chain_id].glChain, link_id, oi_alt[link_id], g_light );
    /* store the backup of pinInfo */
    backup_ghost_info->phantom_pinfo[link_id].pin = g_main->gcs[chain_id].info2[link_id].pinfo.pin;
  }
  display_id = rkglBeginList();
  rkglChainPhantomize( &g_main->gcs[chain_id].glChain, alpha, g_light );
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
  for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ ){
    if( oi_alt[link_id] ) {
      zOpticalInfoDestroy( oi_alt[link_id] );
      zFree( oi_alt[link_id] );
    }
  }
  zFree( oi_alt );

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
    g_main->gcs[chain_id].info2[link_id].pinfo.pin = backup_ghost_info->phantom_pinfo[link_id].pin;
    reset_link_drawing( chain_id, link_id );
    if( g_main->gcs[chain_id].info2[link_id].pinfo.pin != PIN_LOCK_OFF ){
      register_one_link_for_IK( chain_id, link_id, false );
      debug_printf("restore & register g_main->gcs[%d].info2[%d].pin = %d\n", chain_id, link_id, g_main->gcs[chain_id].info2[link_id].pinfo.pin);
    }
  }
}

void switch_collision_avoidance(bool is_ON)
{
  if( !is_ON ){
    g_main->is_collision_avoidance = true;
    printf("collision avoidance is ON\n");
  } else {
    g_main->is_collision_avoidance = false;
    printf("collision avoidance is OFF\n");
  }
}

bool is_collision_avoidance(void)
{
  return g_main->is_collision_avoidance;
}

typedef struct{
  rkIKCell *cell_att[2];
  rkIKCell *cell_pos[2];
} cell6D;
void resolve_collision(void)
{
  bool is_selected_link_collision = false;
  int i, j, selected_chain_id, selected_link_id, chain0_id, chain1_id, link0_id, link1_id;
  rkChain *chain;
  zVec q, pre_q;
  rkCDPair* cp;
  if( zListIsEmpty( &g_main->cplist ) )
    return;
  cell6D* cell_array = zAlloc( cell6D, zListSize(&g_main->cplist) );
  /* A collision should always occur with the dragged link. Is this premise correct ? */
  selected_chain_id = g_main->selected.chain_id;
  selected_link_id = g_main->selected.link_id;
  if( selected_chain_id < 0 )
    return;
  chain = &g_main->gcs[selected_chain_id].chain;
  /* keep original collided joint angles q */
  q = zVecAlloc( rkChainJointSize( chain ) ); /* collided q */
  rkChainGetJointDisAll( chain, q );
  /* restore joint angles pre_q before collided */
  pre_q = g_main->gcs[selected_chain_id].pre_q;
  rkChainSetJointDisAll( chain, pre_q );
  rkChainUpdateFK( chain );
  /* */
  i=0;
  zListForEachRew( &g_main->cplist, cp ){
    cell_array[i].cell_att[0] = NULL;
    cell_array[i].cell_att[1] = NULL;
    cell_array[i].cell_pos[0] = NULL;
    cell_array[i].cell_pos[1] = NULL;
    chain0_id = get_chain_id( cp->data.cell[0]->data.chain );
    chain1_id = get_chain_id( cp->data.cell[1]->data.chain );
    link0_id  = get_link_id(  cp->data.cell[0]->data.chain, cp->data.cell[0]->data.link );
    link1_id  = get_link_id(  cp->data.cell[1]->data.chain, cp->data.cell[1]->data.link );
    if( chain0_id == selected_chain_id ||
        chain1_id == selected_chain_id ){
      /* check drag link collision */
      if( (chain0_id == selected_chain_id && link0_id == selected_link_id) ||
          (chain1_id == selected_chain_id && link1_id == selected_link_id) )
        is_selected_link_collision = true;
      /* */
      if( chain0_id == chain1_id ){
        /* self collsion */
        /* lock link 6D pose at previous motion() event */
        rkIKAttr attr0, attr1;
        attr0.id = link0_id;
        attr0.id_sub = link1_id;
        cell_array[i].cell_att[0] = rkChainRegisterIKCellL2LAtt( chain, NULL, 0, &attr0, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ID_SUB );
        rkIKCellSetWeight( cell_array[i].cell_att[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
        cell_array[i].cell_pos[0] = rkChainRegisterIKCellL2LPos( chain, NULL, 0, &attr0, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ID_SUB | RK_IK_ATTR_MASK_ATTENTION_POINT );
        rkIKCellSetWeight( cell_array[i].cell_pos[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
        attr1.id = link1_id;
        attr1.id_sub = link0_id;
        cell_array[i].cell_att[1] = rkChainRegisterIKCellL2LAtt( chain, NULL, 0, &attr1, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ID_SUB );
        rkIKCellSetWeight( cell_array[i].cell_att[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
        cell_array[i].cell_pos[1] = rkChainRegisterIKCellL2LPos( chain, NULL, 0, &attr1, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ID_SUB | RK_IK_ATTR_MASK_ATTENTION_POINT );
        rkIKCellSetWeight( cell_array[i].cell_pos[1], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
      } else{
        /* with environment collision */
        zVec3D moving_nearest_p0, env_nearest_p1;
        rkCDCell *moving_cell, *env_cell;
        int moving_link_id;
        if( chain0_id == selected_chain_id ){
          moving_cell    = cp->data.cell[0];
          moving_link_id = link0_id;
          env_cell       = cp->data.cell[1];
        } else {
          moving_cell    = cp->data.cell[1];
          moving_link_id = link1_id;
          env_cell       = cp->data.cell[0];
        }
        /* calculate nearest points on both of moving link and environment at previous motion() event */
        zPH3DXform( zShape3DPH(moving_cell->data.shape), rkLinkWldFrame(moving_cell->data.link), &moving_cell->data.ph );
        zPH3DXform( zShape3DPH(env_cell->data.shape), rkLinkWldFrame(env_cell->data.link), &env_cell->data.ph );
        zColChkPH3D( &moving_cell->data.ph, &env_cell->data.ph, &moving_nearest_p0, &env_nearest_p1 );
        zVec3D p1_to_p0, wld_ap, link_ap;
        /* link_ap is the attention point on moving_link frame from the nearest point of moving_link (moving_nearest_p0). */
        /* wld_ap is the avoided position of moving_link on world frame. */
        zXform3DInv( rkChainLinkWldFrame( chain, moving_link_id ), &moving_nearest_p0, &link_ap );
        zVec3DSub( &moving_nearest_p0, &env_nearest_p1, &p1_to_p0 );
        double d = zVec3DNorm( &p1_to_p0 );
        if( d > zTOL )
          zVec3DCat( &env_nearest_p1, (( 2.5 * zTOL ) / d), &p1_to_p0, &wld_ap );
        else
          zVec3DCopy( &env_nearest_p1, &wld_ap );
        /* lock at wld_ap */
        rkIKAttr attr;
        attr.id = moving_link_id;
        cell_array[i].cell_att[0] = rkChainRegisterIKCellWldAtt( chain, NULL, 0, &attr, RK_IK_ATTR_MASK_ID );
        rkIKCellSetWeight( cell_array[i].cell_att[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
        cell_array[i].cell_pos[0] = rkChainRegisterIKCellWldPos( chain, NULL, 0, &attr, RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT );
        rkIKCellSetWeight( cell_array[i].cell_pos[0], IK_PIN_WEIGHT, IK_PIN_WEIGHT, IK_PIN_WEIGHT );
        rkIKCellSetRefVec( cell_array[i].cell_pos[0], &wld_ap );
        zVec3DCopy( &link_ap, rkIKCellAttentionPoint( cell_array[i].cell_pos[0] ) );
      }
    } else {
      ZRUNERROR("A collision has occurred between links that were not dragged chain ! (chain_id=%d, link_id=%d) & (chain_id=%d, link_id=%d). This should not occur,\n", chain0_id, link0_id, chain1_id, link1_id );
    }
    i++;
  } /* end of zListForEachRew( &g_main->cplist, cp ) */

  /* IK */
  if( is_selected_link_collision )
    update_alljoint_by_IK_with_frame( selected_chain_id, -1, pre_q, NULL );
  else
    update_alljoint_by_IK_with_frame( selected_chain_id, selected_link_id, pre_q, &g_main->fh.frame );

  /* unlock */
  for( i=0; i < zListSize(&g_main->cplist); i++){
    for( j=0; j < 2; j++ ){
      if( cell_array[i].cell_att[j] != NULL )
        rkChainUnregisterAndDestroyIKCell( chain, cell_array[i].cell_att[j] );
      if( cell_array[i].cell_pos[j] != NULL )
        rkChainUnregisterAndDestroyIKCell( chain, cell_array[i].cell_pos[j] );
    }/* end of for j=0->2(pair size) */
  } /* end of for i=0->size of cell_array */
  zFree( cell_array );
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

bool set_q_state_array(const int chain_id, double* src_q, const int joint_size)
{
  if( joint_size != jointSize_of_chain( chain_id ) ){
    ZRUNERROR( "src_q array size=%d is not equal to destination of chain joint size=%d", joint_size, jointSize_of_chain( chain_id ) );
    return false;
  }
  zVec dis; /* joints zVec pointer */
  dis = zVecAlloc( jointSize_of_chain( chain_id ) );
  zVecCopyArray( src_q, joint_size, dis );
  rkChainSetJointDisAll( &g_main->gcs[chain_id].chain, dis );
  rkChainUpdateFK( &g_main->gcs[chain_id].chain );

  return true;
}

const int linkNum_of_chain(const int chain_id)
{
  return rkChainLinkNum( &g_main->gcs[chain_id].chain );
}

int* clone_pin_state_array(const int chain_id)
{
  int link_id;
  int* pin_state; /* pinStatus[link_num] */

  pin_state = zAlloc( int, linkNum_of_chain( chain_id ) );
  for( link_id=0; link_id < rkChainLinkNum( &g_main->gcs[chain_id].chain ); link_id++ ){
    pin_state[link_id] = g_main->gcs[chain_id].info2[link_id].pinfo.pin;
  }
  return pin_state;
}

bool set_pin_state_array(const int chain_id, int* src_pin, const int link_num)
{
  if( link_num != linkNum_of_chain( chain_id ) ){
    ZRUNERROR( "src_pin array size=%d is not equal to destination of chain link size=%d", link_num, linkNum_of_chain( chain_id ) );
    return false;
  }
  int link_id;
  for( link_id=0; link_id < link_num; link_id++ ){
    g_main->gcs[chain_id].info2[link_id].pinfo.pin = (pinStatus)( src_pin[link_id] );
  }
  return true;
}

/* ap[link_num][AP_MAX_NUM=2][Vec3D size=3] */
double*** clone_ap_state_array(const int chain_id)
{
  int link_id, ap_id;
  double*** ap_state;

  int link_num = linkNum_of_chain( chain_id );
  ap_state = zAlloc( double**, link_num );
  for( link_id=0; link_id < link_num; link_id++ ){
    pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
    ap_state[link_id] = zAlloc( double*, AP_MAX_NUM );
    for( ap_id=0; ap_id < pinfo->ap_num; ap_id++ ){
      ap_state[link_id][ap_id] = zAlloc( double, 3 );
      ap_state[link_id][ap_id][0] = pinfo->ap[ap_id].c.x;
      ap_state[link_id][ap_id][1] = pinfo->ap[ap_id].c.y;
      ap_state[link_id][ap_id][2] = pinfo->ap[ap_id].c.z;
    }
  }
  return ap_state;
}

/* ap[link_num][AP_MAX_NUM=2][Vec3D size=3] */
bool set_ap_state(const int chain_id, double*** src_ap, const int link_num)
{
  if( link_num != linkNum_of_chain( chain_id ) ){
    ZRUNERROR( "src_pin array size=%d is not equal to destination of chain link size=%d", link_num, linkNum_of_chain( chain_id ) );
    return false;
  }
  int link_id, ap_id;
  for( link_id=0; link_id < link_num; link_id++ ){
    pinInfo* pinfo = &g_main->gcs[chain_id].info2[link_id].pinfo;
    for( ap_id=0; ap_id < pinfo->ap_num; ap_id++ ){
      pinfo->ap[ap_id].c.x = src_ap[link_id][ap_id][0];
      pinfo->ap[ap_id].c.y = src_ap[link_id][ap_id][1];
      pinfo->ap[ap_id].c.z = src_ap[link_id][ap_id][2];
    }
  }
  return true;
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
      if( g_main->gcs[chain_id].info2[link_id].pinfo.pin == PIN_LOCK_6D ){
        printf("- PIN_LOCK_6D : link[%d] %s ---------------------\n",
               link_id, zName( rkChainLink(&g_main->gcs[chain_id].chain, link_id) ) );
        printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( &g_main->gcs[chain_id].chain, link_id ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( &g_main->gcs[chain_id].chain, link_id ) );
        printf("\n");
      } else if( g_main->gcs[chain_id].info2[link_id].pinfo.pin == PIN_LOCK_POS3D ){
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
  printf("############################################################\n\n");
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
  (*main_ptr)->is_visible = true;
  (*main_ptr)->is_collision_avoidance = false;

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
    zListDestroy( rkCDPair, &g_main->cplist );
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
  if( g_main->is_visible &&
      rkgl_mouse_button == GLFW_MOUSE_BUTTON_LEFT &&
      !rkglFrameHandleIsUnselected( &g_main->fh ) ){
    /* moving mode */
    rkglFrameHandleMove( &g_main->fh, g_cam, rkgl_mouse_x, rkgl_mouse_y );
    switch_ghost_mode( g_main->ghost_info.mode != GHOST_MODE_OFF );
    /* IK */
    update_alljoint_by_IK_with_frame( g_main->selected.chain_id, g_main->selected.link_id, NULL, &g_main->fh.frame );
    /* Collision Detection */
    g_main->is_sum_collision = is_collision_detected();

    if( g_main->is_collision_avoidance )
      resolve_collision();

    /* keep FrameHandle position of the link's AP relative to the world frame */
    if( rkglFrameHandleIsInRotation( &g_main->fh ) )
      zFrame3DCopy( rkChainLinkWldFrame( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id ), &g_main->fh.frame );
    zXform3D( rkChainLinkWldFrame( &g_main->gcs[g_main->selected.chain_id].chain, g_main->selected.link_id ), &g_main->selected.ap, zFrame3DPos( &g_main->fh.frame ) );

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

  if( !g_main->is_visible )
    return;
  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      /* draw only frame handle */
      if( rkglSelectNearest( &sb, g_cam, draw_fh_parts, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          rkglFrameHandleAnchor( &g_main->fh, &sb, g_cam, rkgl_mouse_x, rkgl_mouse_y ) >= 0 ){
        register_link_for_IK( g_main->selected.chain_id, g_main->selected.link_id );
      } else{
        rkglFrameHandleUnselect( &g_main->fh );
        /* draw only chain links */
        if( rkglSelectNearest( &sb, g_cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
            is_selected_chain_link( &sb, &new_chain_id, &new_link_id ) ){
          /* a link of a chain is selected. At the same time, new_chain_id >= 0. */
          update_framehandle_location( &sb, g_cam, rkgl_mouse_x, rkgl_mouse_y, new_chain_id, new_link_id );
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
                  g_main->gcs[i].info2[j].pinfo.pin == PIN_LOCK_OFF &&
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
      if( rkglSelectNearest( &sb, g_cam, draw_chain, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) &&
          is_selected_chain_link( &sb, &new_chain_id, &new_link_id ) ){
        g_main->gcs[new_chain_id].info2[new_link_id].is_selected = false;
        bool is_ap_mode = get_pin_ap( &sb, g_cam, rkgl_mouse_x, rkgl_mouse_y, new_chain_id, new_link_id );
        switch_pin_link( new_chain_id, new_link_id, is_ap_mode );
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
    *g_scale -= 0.0001; rkglOrthoScaleH( g_cam, *g_scale, g_znear, g_zfar );
  } else if ( yoffset > 0 ) {
    *g_scale += 0.0001; rkglOrthoScaleH( g_cam, *g_scale, g_znear, g_zfar );
  }
}

void resize(GLFWwindow* window, int w, int h)
{
  rkglVPCreate( g_cam, 0, 0, w, h );
  rkglOrthoScaleH( g_cam, *g_scale, g_znear, g_zfar );
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
  case 'u': rkglCALockonPTR( g_cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( g_cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( g_cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( g_cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( g_cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( g_cam, 0, 0,-5 ); break;
  case '8': *g_scale += 0.0001; rkglOrthoScaleH( g_cam, *g_scale, g_znear, g_zfar ); break;
  case '*': *g_scale -= 0.0001; rkglOrthoScaleH( g_cam, *g_scale, g_znear, g_zfar ); break;
  case 'g':
    if( g_main->ghost_info.mode == GHOST_MODE_ON ){
      update_alljoint_by_IK_with_ghost();
      switch_ghost_mode( false );
    } else{
      switch_ghost_mode( g_main->ghost_info.mode == GHOST_MODE_OFF ); /* OFF <-> READY */
    }
    break;
  case 'h': move_link(-zDeg2Rad(5) ); break;
  case 'c':
    switch_collision_avoidance( g_main->is_collision_avoidance );
    break;
  case 'q': case 'Q': case '\033':
    g_is_exit = true;
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

void set_view_params(void* cam, double* scale, void* light, void* shadow)
{
  g_cam = (rkglCamera*)( cam );
  g_light = (rkglLight*)( light );
  g_shadow = (rkglShadow*)( shadow );
  g_scale = scale;
}

bool create_view_params(void** cam, double** scale, void** light, void** shadow)
{
  if( !( *cam = (void*)( zAlloc( rkglCamera, 1 ) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( rkglCamera, 1 ) )." );
    return false;
  }

  if( !( *scale = zAlloc( double, 1 ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( double, 1) )." );
    return false;
  }

  if( !( *light = (void*)( zAlloc( rkglLight, 1 ) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( rkglLight, 1 )." );
    return false;
  }

  if( !( *shadow = (void*)( zAlloc( rkglShadow, 1 ) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( rkglShadow, 1 )." );
    return false;
  }
  **scale = 0.001;

  return true;
}

void free_view_params(void* cam, double* scale, void* light, void* shadow)
{
  zFree( cam );
  zFree( light );
  zFree( shadow );
  zFree( scale );
}

void setDefaultCallbackParam(void)
{
  rkglSetDefaultCallbackParam( g_cam, 1.0, g_znear, g_zfar, 1.0, 5.0 );
}

void copyFromDefaultCamera(void)
{
  rkglCameraCopyDefault( g_cam );
}

void init_camera_pose(void)
{
  rkglBGSet( g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( g_cam, 0, 0, 0, 45, -30, 0 );
}

void init_light(void)
{
  if( rkglLightNum() == 0 ){
    glEnable(GL_LIGHTING);
    rkglLightCreate( g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
    rkglLightMove( g_light, 3, 5, 9 );
    rkglLightSetAttenuationConst( g_light, 1.0 );
  }
}

bool init(void)
{
  int chain_id;

  if( g_main->modelfiles == NULL ){
    g_main->chainNUM = 1;
    g_main->modelfiles = zAlloc( char*, g_main->chainNUM );
    g_main->modelfiles[0] = zStrClone( "../model/puma.ztk" );
  }
  g_main->gcs = zAlloc( rkglChainBlock, g_main->chainNUM );
  for( chain_id=0; chain_id < g_main->chainNUM; chain_id++ ){
    printf( "modelfile[%d] = %s\n", chain_id, g_main->modelfiles[chain_id] );
    if( !extend_rkChainReadZTK( &g_main->gcs[chain_id].chain, g_main->modelfiles[chain_id] ) ){
      ZRUNWARN( "Failed extend_rkChainReadZTK()" );
      return false;
    }
    if( !rkglChainLoad_for_rkglChainBlock( &g_main->gcs[chain_id], g_light ) ){
      ZRUNWARN( "Failed rkglChainLoad_for_rkglLinkInfo2()" );
      return false;
    }
    /* AP */
    create_chain_ap_displaylist( chain_id );

    /* joint angles before step */
    g_main->gcs[chain_id].pre_q = zVecAlloc( rkChainJointSize( &g_main->gcs[chain_id].chain ) );

    /* IK */
    rkChainCreateIK( &g_main->gcs[chain_id].chain );
    rkChainRegisterIKJointAll( &g_main->gcs[chain_id].chain, IK_DRAG_WEIGHT );
  } /* end of for i : 0 -> g_main->chainNUM */

  /* Collision Detection */
  zListInit( &g_main->cplist );
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

  if( !create_view_params( (void**)(&g_cam), &g_scale, (void**)(&g_light), (void**)(&g_shadow) ) )
    return 1;
  setDefaultCallbackParam();
  init_camera_pose();
  init_light();

  if( !init() ){
    glfwTerminate();
    return 1;
  }
  resize( g_window, width, height );
  glfwSwapInterval(1);

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE &&
          !is_exit() ){
    display(g_window);
    glfwPollEvents();
    glfwSwapBuffers( g_window );
    /* for monitoring stdout & stderr */
    fflush(stdout); fflush(stderr);
  }

  destroy_pindragIFData( g_main );
  free_view_params( g_cam, g_scale, g_light, g_shadow );

  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
