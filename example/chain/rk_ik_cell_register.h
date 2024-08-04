#include <roki/rk_chain.h>

/* header .h -------------------------------------------------------------- */
ZDEF_STRUCT( __ROKI_CLASS_EXPORT, rkIKRegSelectClass ){
  void* (*init                )(void**);
  void (*copy                 )(void*,void*);
  void (*free                 )(void**);
  bool (*select_com           )(void*);
  bool (*com                  )(void*);
  bool (*select_link          )(void*);
  bool (*link                 )(void*);
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
  bool (*unselect_force       )(void*);
  bool (*force                )(void*);
  void (*set_name             )(void*,const char*);
  const char* (*get_name      )(void*);
  void (*set_priority         )(void*,int);
  int  (*get_priority         )(void*);
  void (*set_link_id          )(void*,int);
  int  (*get_link_id          )(void*);
  void (*set_ap               )(void*,double,double,double);
  void (*get_ap               )(void*,double*,double*,double*);
  void (*set_weight           )(void*,double,double,double);
  void (*get_weight           )(void*,double*,double*,double*);
  void (*set_sub_link_frame_id)(void*,int);
  int  (*get_sub_link_frame_id)(void*);
  void (*reset                )(void*);
  void* (*reg                 )(void*,void*);
  void* (*from_cell_name      )(void*,const char*);
  bool (*unreg                )(void*,void*);
  bool (*unreg_by_name        )(void*,const char*);
  void* (*from_ztk            )(void*,void*);
  bool (*fprint_ztk           )(FILE*,void*,void*);
};

/* declaration */
void* rkIKRegSelect_init                (void **instance);
void rkIKRegSelect_copy                 (void *src, void *dest);
void rkIKRegSelect_free                 (void **instance);
bool rkIKRegSelect_select_com           (void *instance);
bool rkIKRegSelect_com                  (void *instance);
bool rkIKRegSelect_select_link          (void *instance);
bool rkIKRegSelect_link                 (void *instance);
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
bool rkIKRegSelect_unselect_force       (void *instance);
bool rkIKRegSelect_force                (void *instance);
void rkIKRegSelect_set_name             (void* instance, const char* name);
const char* rkIKRegSelect_get_name      (void* instance);
void rkIKRegSelect_set_priority         (void* instance, int priority);
int  rkIKRegSelect_get_priority         (void* instance);
void rkIKRegSelect_set_link_id          (void *instance, int link_id);
int  rkIKRegSelect_get_link_id          (void *instance);
void rkIKRegSelect_set_ap               (void *instance, double v1, double v2, double v3);
void rkIKRegSelect_get_ap               (void *instance, double *v1, double *v2, double *v3);
void rkIKRegSelect_set_weight           (void *instance, double w1, double w2, double w3);
void rkIKRegSelect_get_weight           (void *instance, double *w1, double *w2, double *w3);
void rkIKRegSelect_set_sub_link_frame_id(void *instance, int sub_link_id);
int  rkIKRegSelect_get_sub_link_frame_id(void *instance);
void rkIKRegSelect_reset                (void *instance);
void* rkIKRegSelect_call_reg_api        (void *instance, void *chain);
void* rkIKRegSelect_from_cell_name      (void* chain, const char* name);
bool rkIKRegSelect_unreg_by_cell        (void *chain, void* cell);
bool rkIKRegSelect_unreg_by_name        (void *chain, const char* name);
void* rkIKRegSelect_fromZTK_constraint_key(void* chain, void* ztk);
bool rkIKRegSelect_fprintZTK_as_constraint_key(FILE *fp, void* chain, void* instance);

static rkIKRegSelectClass rkIKRegSelectClassImpl = {
  rkIKRegSelect_init,
  rkIKRegSelect_copy,
  rkIKRegSelect_free,
  rkIKRegSelect_select_com,
  rkIKRegSelect_com,
  rkIKRegSelect_select_link,
  rkIKRegSelect_link,
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
  rkIKRegSelect_unselect_force,
  rkIKRegSelect_force,
  rkIKRegSelect_set_name,
  rkIKRegSelect_get_name,
  rkIKRegSelect_set_priority,
  rkIKRegSelect_get_priority,
  rkIKRegSelect_set_link_id,
  rkIKRegSelect_get_link_id,
  rkIKRegSelect_set_ap,
  rkIKRegSelect_get_ap,
  rkIKRegSelect_set_weight,
  rkIKRegSelect_get_weight,
  rkIKRegSelect_set_sub_link_frame_id,
  rkIKRegSelect_get_sub_link_frame_id,
  rkIKRegSelect_reset,
  rkIKRegSelect_call_reg_api,
  rkIKRegSelect_from_cell_name,
  rkIKRegSelect_unreg_by_cell,
  rkIKRegSelect_unreg_by_name,
  rkIKRegSelect_fromZTK_constraint_key,
  rkIKRegSelect_fprintZTK_as_constraint_key,
};


/* implement .c (capsuled) ------------------------------------------------ */

/**/
/* int 32bit */
/* User Defined Type (24bit) : Reference Frame Type, Target Type, Quantity Type */
static const int32_t RK_IK_ATTR_TYPE__WORLD_LINK_POS     = 0x010101;
static const int32_t RK_IK_ATTR_TYPE__WORLD_LINK_ATT     = 0x010102;
static const int32_t RK_IK_ATTR_TYPE__WORLD_LINK_AM      = 0x010103;
static const int32_t RK_IK_ATTR_TYPE__WORLD_COM_POS      = 0x010201;
static const int32_t RK_IK_ATTR_TYPE__WORLD_COM_AM       = 0x010203;
static const int32_t RK_IK_ATTR_TYPE__SUB_LINK_LINK_POS  = 0x020101;
static const int32_t RK_IK_ATTR_TYPE__SUB_LINK_LINK_ATT  = 0x020102;
/* static const int32_t RK_IK_ATTR_TYPE__SUB_LINK_LINK_AM   = 0x020103; */
/* static const int32_t RK_IK_ATTR_TYPE__SUB_LINK_COM_POS   = 0x020201; */
/* static const int32_t RK_IK_ATTR_TYPE__SUB_LINK_COM_AM    = 0x020203; */
/* Reference Frame Type for Selecting */
static const int32_t RK_IK_ATTR_TYPE_REF_FRAME           = 0xff0000;
static const int32_t RK_IK_ATTR_TYPE_REF_FRAME__WORLD    = 0x010000;
static const int32_t RK_IK_ATTR_TYPE_REF_FRAME__SUB_LINK = 0x020000;
/* Target Type for Selecting */
static const int32_t RK_IK_ATTR_TYPE_TARGET              = 0x00ff00;
static const int32_t RK_IK_ATTR_TYPE_TARGET__LINK        = 0x000100;
static const int32_t RK_IK_ATTR_TYPE_TARGET__COM         = 0x000200;
/* Quantity Type for Selecting */
static const int32_t RK_IK_ATTR_TYPE_QUANTITY           = 0x00000ff;
static const int32_t RK_IK_ATTR_TYPE_QUANTITY__POS      = 0x0000001;
static const int32_t RK_IK_ATTR_TYPE_QUANTITY__ATT      = 0x0000002;
static const int32_t RK_IK_ATTR_TYPE_QUANTITY__AM       = 0x0000003;


void* rkIKRegSelect_init(void** instance)
{
  *instance = (void*)rkIKCellAlloc();
  return instance;
}

void rkIKRegSelect_copy(void* src, void* dest)
{
  zNameFree( &((rkIKCell*)dest)->data );
  zCopy( rkIKCell, (rkIKCell*)(src), (rkIKCell*)(dest) );
  zNameSet( &((rkIKCell*)dest)->data, ((rkIKCell*)src)->data.name );
}

void rkIKRegSelect_free(void **instance)
{
  rkIKCellDestroy( (rkIKCell*)(*instance) );
  free( (rkIKCell*)(*instance) );
  *instance = NULL;
}

bool rkIKRegSelect_select_link(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_TARGET);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_TARGET__LINK;
  return true;
}

bool rkIKRegSelect_link(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  int target = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_TARGET;
  return (target == RK_IK_ATTR_TYPE_TARGET__LINK);
}

bool rkIKRegSelect_select_com(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_TARGET);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_TARGET__COM;
  return true;
}

bool rkIKRegSelect_com(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  int target = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_TARGET;
  return (target == RK_IK_ATTR_TYPE_TARGET__COM);
}

bool rkIKRegSelect_select_pos(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_QUANTITY);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_QUANTITY__POS;
  return true;
}

bool rkIKRegSelect_pos(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  int quantity = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_QUANTITY;
  return (quantity == RK_IK_ATTR_TYPE_QUANTITY__POS);
}

bool rkIKRegSelect_select_att(void* instance){
  rkIKCell* cell;
  if( rkIKRegSelect_com( instance ) ) return false; /* validation */
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_QUANTITY);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_QUANTITY__ATT;
  return true;
}

bool rkIKRegSelect_att(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  int quantity = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_QUANTITY;
  return (quantity == RK_IK_ATTR_TYPE_QUANTITY__ATT);
}

bool rkIKRegSelect_select_am(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_QUANTITY);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_QUANTITY__AM;
  return true;
}

bool rkIKRegSelect_am(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  int quantity = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_QUANTITY;
  return (quantity == RK_IK_ATTR_TYPE_QUANTITY__AM);
}

bool rkIKRegSelect_select_wld_frame(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_REF_FRAME);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_REF_FRAME__WORLD;
  return true;
}

bool rkIKRegSelect_wld_frame(void* instance){
  rkIKCell* cell;
  int ref_frame;
  cell = (rkIKCell*)(instance);
  ref_frame = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_REF_FRAME;
  return (ref_frame == RK_IK_ATTR_TYPE_REF_FRAME__WORLD);
}

bool rkIKRegSelect_select_sub_link_frame(void* instance){
  rkIKCell* cell;
  if( rkIKRegSelect_com( instance ) ) return false; /* validation. but maybe change */
  if( rkIKRegSelect_am( instance ) ) return false; /* validation */
  cell = (rkIKCell*)(instance);
  cell->data.attr.user_defined_type &= (~RK_IK_ATTR_TYPE_REF_FRAME);
  cell->data.attr.user_defined_type |= RK_IK_ATTR_TYPE_REF_FRAME__SUB_LINK;
  return true;
}

bool rkIKRegSelect_sub_link_frame(void* instance){
  rkIKCell* cell;
  int ref_frame;
  cell = (rkIKCell*)(instance);
  ref_frame = cell->data.attr.user_defined_type & RK_IK_ATTR_TYPE_REF_FRAME;
  return (ref_frame == RK_IK_ATTR_TYPE_REF_FRAME__SUB_LINK);
}

bool rkIKRegSelect_select_force(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.priority = RK_IK_MAX_PRIORITY;
  return true;
}

bool rkIKRegSelect_unselect_force(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.priority = 0;
  return true;
}

bool rkIKRegSelect_force(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  return cell->data.priority == RK_IK_MAX_PRIORITY;
}

/**/

void rkIKRegSelect_set_name(void* instance, const char* name){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  zNameFree( &cell->data );
  zNameSet( &cell->data, name );
}

const char* rkIKRegSelect_get_name(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  return cell->data.name;
}

void rkIKRegSelect_set_priority(void* instance, int priority){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  if( priority < 0 )
    priority = 0; /* validation */
  if( priority >= RK_IK_MAX_PRIORITY )
    cell->data.priority = RK_IK_MAX_PRIORITY - 1;
  else
    cell->data.priority = priority;
}

int rkIKRegSelect_get_priority(void *instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  return cell->data.priority;
}

void rkIKRegSelect_set_link_id(void* instance, int link_id){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.id = link_id;
}

int rkIKRegSelect_get_link_id(void *instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  return cell->data.attr.id;
}

void rkIKRegSelect_set_ap(void* instance, double v1, double v2, double v3){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  rkIKAttrSetAttentionPoint( &cell->data.attr, v1, v2, v3 );
}

void rkIKRegSelect_get_ap(void *instance, double *v1, double *v2, double *v3){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  *v1 = cell->data.attr.attention_point.c.x;
  *v2 = cell->data.attr.attention_point.c.y;
  *v3 = cell->data.attr.attention_point.c.z;
}

void rkIKRegSelect_set_weight(void* instance, double w1, double w2, double w3){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  rkIKAttrSetWeight( &cell->data.attr, w1, w2, w3 );
}

void rkIKRegSelect_get_weight(void *instance, double *w1, double *w2, double *w3){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  *w1 = cell->data.attr.weight.c.x;
  *w2 = cell->data.attr.weight.c.y;
  *w3 = cell->data.attr.weight.c.z;
}

void rkIKRegSelect_set_sub_link_frame_id(void* instance, int sub_link_id){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  cell->data.attr.id_sub = sub_link_id;
}

int rkIKRegSelect_get_sub_link_frame_id(void *instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  return cell->data.attr.id_sub;
}

/**/

void rkIKRegSelect_reset(void *instance){
  rkIKCell* cell;
  rkIKAttr blank_attr;
  cell = (rkIKCell*)(instance);
  rkIKAttrInit( &blank_attr );
  blank_attr.user_defined_type = 0;
  zCopy( rkIKAttr, &blank_attr, &cell->data.attr );
  cell->data.constraint = NULL;
  cell->data.priority = 0;
  cell->data.mode = RK_IK_CELL_MODE_XYZ;
}

const rkIKConstraint* constraint_factory(void* instance){
  rkIKCell* cell;
  cell = (rkIKCell*)(instance);
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__WORLD_LINK_POS ){
    return rkIKConstraintFind( "world_pos" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__WORLD_LINK_ATT ){
    return rkIKConstraintFind( "world_att" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__SUB_LINK_LINK_POS ){
    return rkIKConstraintFind( "l2l_pos" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__SUB_LINK_LINK_ATT ){
    return rkIKConstraintFind( "l2l_att" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__WORLD_COM_POS ) {
    return rkIKConstraintFind( "com" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__WORLD_LINK_AM ) {
    return rkIKConstraintFind( "angular_momentum" );
  } else
  if( cell->data.attr.user_defined_type == RK_IK_ATTR_TYPE__WORLD_COM_AM ) {
    return rkIKConstraintFind( "angular_momentum_about_com" );
  } else {
    return NULL;
  }
}

const int32_t get_user_defined_type(const char* type)
{
  if( strcmp( type, "world_pos" ) == 0 ){
    return RK_IK_ATTR_TYPE__WORLD_LINK_POS;
  } else
  if( strcmp( type, "world_att" ) == 0 ){
    return RK_IK_ATTR_TYPE__WORLD_LINK_ATT;
  } else
  if( strcmp( type, "l2l_pos" ) == 0 ){
    return RK_IK_ATTR_TYPE__SUB_LINK_LINK_POS;
  } else
  if( strcmp( type, "l2l_att" ) == 0 ){
    return RK_IK_ATTR_TYPE__SUB_LINK_LINK_ATT;
  } else
  if( strcmp( type, "com" ) == 0 ){
    return RK_IK_ATTR_TYPE__WORLD_COM_POS;
  } else
  if( strcmp( type, "angular_momentum" ) == 0 ){
    return RK_IK_ATTR_TYPE__WORLD_LINK_AM;
  } else
  if( strcmp( type, "angular_momentum_about_com" ) == 0 ){
    return RK_IK_ATTR_TYPE__WORLD_COM_AM;
  }
  return 0;
}


ubyte mask_factory(void* instance){
  ubyte mask = RK_IK_ATTR_MASK_NONE;
  if( rkIKRegSelect_link( instance ) )
    mask |= RK_IK_ATTR_MASK_ID | RK_IK_ATTR_MASK_ATTENTION_POINT;
  if( rkIKRegSelect_sub_link_frame( instance ) )
    mask |= RK_IK_ATTR_MASK_ID_SUB;
  mask |= RK_IK_ATTR_MASK_WEIGHT;

  return mask;
}

void* rkIKRegSelect_call_reg_api(void* instance, void* chain){
  rkIKCell* cell;
  rkIKCell* ret_cell;
  const rkIKConstraint *constraint;
  ubyte mask;
  if( !( cell = (rkIKCell*)( instance ) ) )
    return NULL;
  if( !cell->data.constraint ||
      get_user_defined_type( cell->data.constraint->typestr ) == 0 ){
    if( !( constraint = constraint_factory( instance ) ) )
      return NULL;
    cell->data.constraint = constraint;
  }
  mask = mask_factory( instance );
  cell->data.attr.mask = mask;
  if( !( ret_cell = rkChainAddIKCell( (rkChain*)(chain), cell ) ) ){
    eprintf( "Invalid rkIKAttr Setting Pattern \n" );
    ZRUNERROR( RK_ERR_IK_CELL_NOTFOUND, cell->data.constraint->typestr );
  }

  return (void*)(ret_cell);
}

void* rkIKRegSelect_from_cell_name(void* chain, const char* name)
{
  return (void*)rkChainFindIKCellByName( (rkChain*)(chain), name );
}

/* just wrapper for encapsulating types */
bool rkIKRegSelect_unreg_by_cell(void *chain, void *cell){
  return rkChainUnregisterIKCell( (rkChain*)(chain), (rkIKCell*)(cell) ) ? true : false;
}

bool rkIKRegSelect_unreg_by_name(void *chain, const char* name)
{
  rkIKCell* cell;
  if( !( cell = rkChainFindIKCellByName( (rkChain*)(chain), name ) ) )
    return false;
  return rkChainUnregisterIKCell( (rkChain*)(chain), (rkIKCell*)(cell) ) ? true : false;
}

void* rkIKRegSelect_fromZTK_constraint_key(void* chain, void* ztk)
{
  rkIKCell* cell;
  const rkIKConstraint *constraint;
  rkIKAttr attr;
  ubyte mask = RK_IK_ATTR_MASK_NONE;
  int priority;
  const char *nameptr;
  const char *typestr;
  priority = ZTKInt((ZTK*)ztk);
  nameptr = ZTKVal((ZTK*)ztk);
  rkIKAttrInit( &attr );
  ZTKValNext( (ZTK*)ztk );
  typestr = ZTKVal((ZTK*)ztk);
  if( !( constraint = rkIKConstraintFind( typestr ) ) ) return NULL;
  ZTKValNext( (ZTK*)ztk );
  if( !constraint->fromZTK( (rkChain*)chain, &attr, &mask, (ZTK*)ztk ) ){
    ZRUNERROR( "in persing constraint %s", nameptr );
    return NULL;
  }
  /* set */
  rkIKRegSelect_init( (void**)(&cell) );
  rkIKRegSelect_set_name( (void*)cell, nameptr );
  zCopy( rkIKAttr, &attr, &cell->data.attr );
  cell->data.attr.user_defined_type = get_user_defined_type( typestr );
  rkIKRegSelect_set_priority( (void*)cell, priority );
  cell->data.constraint = constraint;

  return (void*)(cell);
}

bool rkIKRegSelect_fprintZTK_as_constraint_key(FILE *fp, void* chain, void* instance)
{
  rkIKCell* cell;
  const rkIKConstraint *constraint;
  ubyte mask;
  if( !( cell = (rkIKCell*)( instance ) ) )
    return false;
  if( !cell->data.constraint ||
      get_user_defined_type( cell->data.constraint->typestr ) == 0 ){
    if( !( constraint = constraint_factory( instance ) ) )
      return false;
    cell->data.constraint = constraint;
  }
  mask = mask_factory( instance );
  cell->data.attr.mask = mask;
  fprintf( fp, "constraint: %d %s %s", rkIKCellPriority(cell), rkIKCellName(cell), cell->data.constraint->typestr );
  cell->data.constraint->fprintZTK( fp, (rkChain*)chain, cell );

  return true;
}
