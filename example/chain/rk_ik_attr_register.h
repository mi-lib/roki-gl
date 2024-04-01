#include <roki/rk_chain.h>

/* header .h -------------------------------------------------------------- */
ZDEF_STRUCT( __ROKI_CLASS_EXPORT, rkIKRegSelectClass ){
  void* (*init                )(void**);
  void (*select_com           )(void*);
  bool (*com                  )(void*);
  void (*select_link          )(void*);
  bool (*link                 )(void*);
  bool (*select_link_ap       )(void*);
  bool (*link_ap              )(void*);
  bool (*select_pos           )(void*);
  bool (*pos                  )(void*);
  bool (*select_att           )(void*);
  bool (*att                  )(void*);
  bool (*select_am            )(void*);
  bool (*am                   )(void*);
  bool (*select_wld_frame     )(void*);
  bool (*wld_frame            )(void*);
  bool (*select_sub_link_frame)(void*);
  bool (*sub_link_frame       )(void*);
  bool (*select_force         )(void*);
  bool (*force                )(void*);
  bool (*select_weight        )(void*);
  bool (*weight               )(void*);
  void (*set_link_id          )(void*,int);
  int  (*get_link_id          )(void*);
  void (*set_ap               )(void*,double,double,double);
  void (*get_ap               )(void*,double*,double*,double*);
  void (*set_weight           )(void*,double,double,double);
  void (*get_weight           )(void*,double*,double*,double*);
  void (*set_sub_link_frame_id)(void*,int);
  int  (*get_sub_link_frame_id)(void*);
  void (*reset                )(void*);
  void* (*call_api            )(void*,void*);
};

/* declaration */
void* rkIKRegSelect_init                (void **instance);
void rkIKRegSelect_select_com           (void *instance);
bool rkIKRegSelect_com                  (void *instance);
void rkIKRegSelect_select_link          (void *instance);
bool rkIKRegSelect_link                 (void *instance);
bool rkIKRegSelect_select_link_ap       (void *instance);
bool rkIKRegSelect_link_ap              (void *instance);
bool rkIKRegSelect_select_pos           (void *instance);
bool rkIKRegSelect_pos                  (void *instance);
bool rkIKRegSelect_select_att           (void *instance);
bool rkIKRegSelect_att                  (void *instance);
bool rkIKRegSelect_select_am            (void *instance);
bool rkIKRegSelect_am                   (void *instance);
bool rkIKRegSelect_select_wld_frame     (void *instance);
bool rkIKRegSelect_wld_frame            (void *instance);
bool rkIKRegSelect_select_sub_link_frame(void *instance);
bool rkIKRegSelect_sub_link_frame       (void *instance);
bool rkIKRegSelect_select_force         (void *instance);
bool rkIKRegSelect_force                (void *instance);
bool rkIKRegSelect_select_weight        (void *instance);
bool rkIKRegSelect_weight               (void *instance);
void rkIKRegSelect_set_link_id          (void *instance, int link_id);
int  rkIKRegSelect_get_link_id          (void *instance);
void rkIKRegSelect_set_ap               (void *instance, double v1, double v2, double v3);
void rkIKRegSelect_get_ap               (void *instance, double *v1, double *v2, double *v3);
void rkIKRegSelect_set_weight           (void *instance, double w1, double w2, double w3);
void rkIKRegSelect_get_weight           (void *instance, double *w1, double *w2, double *w3);
void rkIKRegSelect_set_sub_link_frame_id(void *instance, int sub_link_id);
int  rkIKRegSelect_get_sub_link_frame_id(void *instance);
void rkIKRegSelect_reset                (void *instance);
void* rkIKRegSelect_call_api            (void *instance, void *chain);

static rkIKRegSelectClass rkIKRegSelectClassImpl = {
  rkIKRegSelect_init,
  rkIKRegSelect_select_com,
  rkIKRegSelect_com,
  rkIKRegSelect_select_link,
  rkIKRegSelect_link,
  rkIKRegSelect_select_link_ap,
  rkIKRegSelect_link_ap,
  rkIKRegSelect_select_pos,
  rkIKRegSelect_pos,
  rkIKRegSelect_select_att,
  rkIKRegSelect_att,
  rkIKRegSelect_select_am,
  rkIKRegSelect_am,
  rkIKRegSelect_select_wld_frame,
  rkIKRegSelect_wld_frame,
  rkIKRegSelect_select_sub_link_frame,
  rkIKRegSelect_sub_link_frame,
  rkIKRegSelect_select_force,
  rkIKRegSelect_force,
  rkIKRegSelect_select_weight,
  rkIKRegSelect_weight,
  rkIKRegSelect_set_link_id,
  rkIKRegSelect_get_link_id,
  rkIKRegSelect_set_ap,
  rkIKRegSelect_get_ap,
  rkIKRegSelect_set_weight,
  rkIKRegSelect_get_weight,
  rkIKRegSelect_set_sub_link_frame_id,
  rkIKRegSelect_get_sub_link_frame_id,
  rkIKRegSelect_reset,
  rkIKRegSelect_call_api
};

/* implement .c (capsuled) ------------------------------------------------ */

typedef enum{
  RK_IK_TARGET_NONE,
  RK_IK_TARGET_COM,    /* Center Of Mass */
  RK_IK_TARGET_LINK,   /* the origin of link frame */
  RK_IK_TARGET_LINK_AP /* the attented point of link */
} rkIKTargetType;

typedef enum{
  RK_IK_TARGET_QUANTITY_NONE,
  RK_IK_TARGET_QUANTITY_POS,  /* position */
  RK_IK_TARGET_QUANTITY_ATT,  /* attitude */
  RK_IK_TARGET_QUANTITY_AM,   /* angular momentum */
} rkIKTargetQuantityType;

typedef enum{
  RK_IK_REF_FRAME_NONE,
  RK_IK_REF_FRAME_WLD,      /* on world frame */
  RK_IK_REF_FRAME_SUB_LINK, /* on sub link frame */
} rkIKReferenceFrameType;

typedef enum{
  RK_IK_PRIORITY_NONE,
  RK_IK_PRIORITY_FORCE, /* weight is forced to be max */
  RK_IK_PRIORITY_WEIGHT
} rkIKPriorityType;

ZDEF_STRUCT( __ROKI_CLASS_EXPORT, rkIKRegSelectable ){
  /* target */
  rkIKTargetType _target;
  rkIKTargetQuantityType _quantity;
  rkIKReferenceFrameType _ref_frame;
  rkIKPriorityType _priority;
};

ZDEF_STRUCT( __ROKI_CLASS_EXPORT, rkIKRegister ){
  /* arguments for call api */
  rkIKRegSelectable _sel;
  rkIKAttr _attr;
};

/**/

void* rkIKRegSelect_init(void** instance)
{
  rkIKRegister* reg = (rkIKRegister*)(*instance);
  reg = zAlloc( rkIKRegister, 1 );
  *instance = (void*)(reg);
  return reg;
}

void rkIKRegSelect_select_com(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_target = RK_IK_TARGET_COM;
}

bool rkIKRegSelect_com(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_target == RK_IK_TARGET_COM);
}

void rkIKRegSelect_select_link(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_target = RK_IK_TARGET_LINK;
}

bool rkIKRegSelect_link(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_target == RK_IK_TARGET_LINK);
}

bool rkIKRegSelect_select_link_ap(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_target = RK_IK_TARGET_LINK_AP;
  return true;
}

bool rkIKRegSelect_link_ap(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_target == RK_IK_TARGET_LINK_AP);
}

bool rkIKRegSelect_select_pos(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_quantity = RK_IK_TARGET_QUANTITY_POS;
  return true;
}

bool rkIKRegSelect_pos(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_quantity == RK_IK_TARGET_QUANTITY_POS);
}

bool rkIKRegSelect_select_att(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  if( sel->_target == RK_IK_TARGET_COM ) return false; /* validation */
  sel->_quantity = RK_IK_TARGET_QUANTITY_ATT;
  return true;
}

bool rkIKRegSelect_att(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_quantity == RK_IK_TARGET_QUANTITY_ATT);
}

bool rkIKRegSelect_select_am(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_quantity = RK_IK_TARGET_QUANTITY_AM;
  return true;
}

bool rkIKRegSelect_am(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_quantity == RK_IK_TARGET_QUANTITY_AM);
}

bool rkIKRegSelect_select_wld_frame(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_ref_frame = RK_IK_REF_FRAME_WLD;
  return true;
}

bool rkIKRegSelect_wld_frame(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_ref_frame == RK_IK_REF_FRAME_WLD);
}

bool rkIKRegSelect_select_sub_link_frame(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  if( sel->_target == RK_IK_TARGET_COM ) return false; /* validation */
  if( sel->_quantity == RK_IK_TARGET_QUANTITY_AM ) return false; /* validation */
  sel->_ref_frame = RK_IK_REF_FRAME_SUB_LINK;
  return true;
}

bool rkIKRegSelect_sub_link_frame(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_ref_frame == RK_IK_REF_FRAME_SUB_LINK);
}

bool rkIKRegSelect_select_force(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_priority = RK_IK_PRIORITY_FORCE;
  return true;
}

bool rkIKRegSelect_force(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_priority == RK_IK_PRIORITY_FORCE);
}

bool rkIKRegSelect_select_weight(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  sel->_priority = RK_IK_PRIORITY_WEIGHT;
  return true;
}

bool rkIKRegSelect_weight(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  return (sel->_priority == RK_IK_PRIORITY_WEIGHT);
}

/**/

void rkIKRegSelect_set_link_id(void* instance, int link_id){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  attr->id = link_id;
}

int rkIKRegSelect_get_link_id(void *instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  return reg->_attr.id;
}

void rkIKRegSelect_set_ap(void* instance, double v1, double v2, double v3){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  rkIKAttrSetAP( attr, v1, v2, v3 );
}

void rkIKRegSelect_get_ap(void *instance, double *v1, double *v2, double *v3){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  *v1 = attr->ap.c.x;
  *v2 = attr->ap.c.y;
  *v3 = attr->ap.c.z;
}

void rkIKRegSelect_set_weight(void* instance, double w1, double w2, double w3){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  rkIKAttrSetWeight( attr, w1, w2, w3 );
}

void rkIKRegSelect_get_weight(void *instance, double *w1, double *w2, double *w3){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  *w1 = attr->w.c.x;
  *w2 = attr->w.c.y;
  *w3 = attr->w.c.z;
}

void rkIKRegSelect_set_sub_link_frame_id(void* instance, int sub_link_id){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKAttr* attr = &reg->_attr;
  attr->id_sub = sub_link_id;
}

int rkIKRegSelect_get_sub_link_frame_id(void *instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  return reg->_attr.id_sub;
}

/**/

void rkIKRegSelect_reset(void *instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable defalut_sel = {RK_IK_TARGET_NONE, RK_IK_TARGET_QUANTITY_NONE, RK_IK_REF_FRAME_NONE, RK_IK_PRIORITY_NONE};
  zCopy( rkIKRegSelectable, &defalut_sel, &reg->_sel );
  rkIKAttr blank_attr;
  zCopy( rkIKAttr, &blank_attr, &reg->_attr );
}

/**/

ZDEF_STRUCT( __ROKI_CLASS_EXPORT, _rkIKLookup ){
  rkIKCell *(*reg_ik_cell)(rkChain*,rkIKAttr*,int);
};

static _rkIKLookup api_world_pos = { rkChainRegIKCellWldPos };
static _rkIKLookup api_world_att = { rkChainRegIKCellWldAtt };
static _rkIKLookup api_l2l_pos   = { rkChainRegIKCellL2LPos };
static _rkIKLookup api_l2l_att   = { rkChainRegIKCellL2LAtt };
static _rkIKLookup api_com       = { rkChainRegIKCellCOM    };
static _rkIKLookup api_am        = { rkChainRegIKCellAM     };
static _rkIKLookup api_amcom     = { rkChainRegIKCellAMCOM  };

_rkIKLookup* api_factory(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  _rkIKLookup *lookup = NULL;
  if( sel->_target == RK_IK_TARGET_LINK &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_POS &&
      sel->_ref_frame == RK_IK_REF_FRAME_WLD ){
    lookup = &api_world_pos;
  } else
  if( sel->_target == RK_IK_TARGET_LINK &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_ATT &&
      sel->_ref_frame == RK_IK_REF_FRAME_WLD ) {
    lookup = &api_world_att;
  } else
  if( sel->_target == RK_IK_TARGET_LINK &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_POS &&
      sel->_ref_frame == RK_IK_REF_FRAME_SUB_LINK ) {
    lookup = &api_l2l_pos;
  } else
  if( sel->_target == RK_IK_TARGET_LINK &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_ATT &&
      sel->_ref_frame == RK_IK_REF_FRAME_SUB_LINK ) {
    lookup = &api_l2l_att;
  } else
  if( sel->_target == RK_IK_TARGET_COM &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_POS &&
      sel->_ref_frame == RK_IK_REF_FRAME_WLD ) {
    lookup = &api_com;
  } else
  if( sel->_target == RK_IK_TARGET_LINK &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_AM &&
      sel->_ref_frame == RK_IK_REF_FRAME_WLD ) {
    lookup = &api_am;
  } else
  if( sel->_target == RK_IK_TARGET_COM &&
      sel->_quantity == RK_IK_TARGET_QUANTITY_AM &&
      sel->_ref_frame == RK_IK_REF_FRAME_WLD ) {
    lookup = &api_amcom;
  } else {
    return NULL;
  }

  return lookup;
}

int mask_factory(void* instance){
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKRegSelectable* sel = &reg->_sel;
  int mask = RK_IK_ATTR_NONE;
  if( sel->_target == RK_IK_TARGET_LINK_AP )
    mask |= RK_IK_ATTR_AP;
  if( sel->_priority == RK_IK_PRIORITY_WEIGHT )
    mask |= RK_IK_ATTR_WEIGHT;
  if( sel->_priority == RK_IK_PRIORITY_FORCE )
    mask |= RK_IK_ATTR_FORCE;
  if( sel->_target == RK_IK_TARGET_LINK )
    mask |= RK_IK_ATTR_ID;
  if( sel->_ref_frame == RK_IK_REF_FRAME_SUB_LINK )
    mask |= RK_IK_ATTR_ID_SUB;

  return mask;
}

void* rkIKRegSelect_call_api(void* instance, void* chain){
  _rkIKLookup *lookup = api_factory( instance );
  if( lookup == NULL )
    return NULL;
  int mask = mask_factory( instance );
  rkIKRegister* reg = (rkIKRegister*)(instance);
  rkIKCell *cell = lookup->reg_ik_cell( chain, &reg->_attr, mask );
  if( cell == NULL )
    ZRUNERROR( RK_ERR_IK_UNKNOWN, "Invalid rkIKAttr Setting Pattern" );

  return (void*)(cell);
}
