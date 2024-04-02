#include <roki_gl/roki_glfw.h>
#include "rk_ik_attr_register.h"

GLFWwindow* g_window;

#define IK_PIN_CELL_SIZE 2 /* size = pos & att */

rkIKRegSelectClass *g_ik_reg_cls = &rkIKRegSelectClassImpl;

typedef enum{
  PIN_LOCK_OFF=-1,
  PIN_LOCK_6D,
  PIN_LOCK_POS3D
} pinStatus;

/* This value corresponds to the index (cell_id) of rkIKCell array c[2] */
typedef enum{
  IK_CELL_TYPE_WLD_POS=0,
  IK_CELL_TYPE_WLD_ATT
} ikCellType;

typedef struct{
  bool is_constrained;
  ikCellType cell_type;
  double w; /* weight of pos & att */
  double dw;
  double d2w;
} pinIKInfo;

typedef struct{
  pinStatus pin;
  pinIKInfo c[IK_PIN_CELL_SIZE]; /* size = pos & att */
  int cell_size; /* = 2 (pos & att), the size of pinIKInfo c[] */
} linkPinIKInfo;

zArrayClass( allLinkPinIKInfo, linkPinIKInfo );

typedef struct{
  double s;
  double ds;
  double d2s;
} feedRate;

typedef struct{
  zFrame3D root;
  zVec q;
  allLinkPinIKInfo all_link_pin_ik_info; /* size = all link size of a chain */
  int display_id;
  feedRate feedrate;
} keyFrameInfo;
zArrayClass( keyFrameInfoArray, keyFrameInfo );

/* keyframe */
keyFrameInfoArray g_keyframe_array;

/* the weight of joint & pin & path link for IK */
#define IK_JOINT_WEIGHT 0.01
#define IK_PIN_WEIGHT 1.0
#define IK_PATH_WEIGHT 1.0
#define IK_NO_WEIGHT 0.0

/* Test Dataset ********************************************************************/
#define TEST_KEYFRAME_SIZE 2
#define TEST_IK_NUM_PATH01 1
#define TEST_JOINT_SIZE 6
#define TEST_LINK_SIZE 7
#define TEST_FRAME_DOF_SIZE 6
#define TEST_PATH_TARGET_SIZE 1

/* x, y, z, aax, aay, aaz */
double test_root_frame[TEST_KEYFRAME_SIZE][TEST_FRAME_DOF_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 } };
double tmp_root_frame[TEST_FRAME_DOF_SIZE] =
  { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 };

double tmp_s = 0.08;
/* q0 ... q5 */
double test_q[TEST_KEYFRAME_SIZE][TEST_JOINT_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { zDeg2Rad(45.0), zDeg2Rad(-60.0), zDeg2Rad(-5.0), 0.0, zDeg2Rad(90.0), 0.0 } };
/* pin */
int test_pin[TEST_KEYFRAME_SIZE][TEST_LINK_SIZE] =
  /* { { PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_OFF, */
  /*     PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_POS3D, PIN_LOCK_POS3D }, */
  /*   { PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_6D, */
  /*     PIN_LOCK_POS3D, PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF }}; */
  { { PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF,
      PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_POS3D },
    { PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF,
      PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_OFF, PIN_LOCK_POS3D }};

typedef struct{
  int ik_num;
  rkIKRegister* ik_reg_array; /* array pointer, ik_num = cell_size */
} refPathInput;

/* _target, _quantity, _ref_frame, _priority */
const rkIKRegSelectable TEST_IK_REG_SELCTABLE__PATH01_IK01 =
  {RK_IK_TARGET_LINK, RK_IK_TARGET_QUANTITY_POS, RK_IK_REF_FRAME_WLD, RK_IK_PRIORITY_WEIGHT};
const int    TEST_IK_ATTR_ID_SUB = 0;
const zVec3D TEST_IK_ATTR_AP     = { {0, 0, 0} };
const byte   TEST_IK_ATTR_MODE   = 0x0;
const zVec3D TEST_IK_ATTR_WEIGHT = { {IK_PATH_WEIGHT, IK_PATH_WEIGHT, IK_PATH_WEIGHT} };
/* id, id_sub, ap, mode, w */
const rkIKAttr TEST_IK_ATTR__PATH01_IK01 =
  { 6, TEST_IK_ATTR_ID_SUB, TEST_IK_ATTR_AP, TEST_IK_ATTR_MODE, TEST_IK_ATTR_WEIGHT };
/* { rkIKAttrSelectable, rkIKAttr } */
const rkIKRegister TEST_IK_REG__PATH01_IK01 =
    { TEST_IK_REG_SELCTABLE__PATH01_IK01, TEST_IK_ATTR__PATH01_IK01 };
rkIKRegister TEST_IK_REG_ARRAY_PATH01[TEST_IK_NUM_PATH01] =
  { TEST_IK_REG__PATH01_IK01 }; /* TEST_IK_NUM_01 */
refPathInput test_ref_path_input[TEST_KEYFRAME_SIZE-1] =
  {
    /* { ik_reg, num } */
    { TEST_IK_NUM_PATH01, TEST_IK_REG_ARRAY_PATH01 }
  }; /* TEST_KEYFRAME_SIZE-1 (= PATH_SIZE) */
/* End of Test Dataset *************************************************************/

refPathInput* g_ref_path_input_array; /* path size */

zArrayClass( zPexIPArray, zPexIP );

/* path constrained cell info for IK */
typedef struct{
  bool is_constrained;
  ikCellType cell_type;
  zPexIP weight_path;
  rkIKCell *cell;
} pinPathIKInfo;
/* p2p path */
typedef struct{
  pinPathIKInfo c[IK_PIN_CELL_SIZE]; /* size = pos & att */
  int cell_size; /* = 2 (pos & att), the size of pinPathIKInfo c[] ( weight & cell ) */
} linkPinPathIKInfo;

zArrayClass( allLinkPinPathIKInfo, linkPinPathIKInfo );

/* position/attitude path form */
typedef struct{
  rkIKRegister ik_reg;
  rkIKCell *cell;
  zNURBS3D nurbs;
  /* Circle Path, Other Form (for pos/att)...etc. */
} refPath;

zArrayClass( refPathArray, refPath );

typedef struct{
  feedRate start_feedrate;
  feedRate goal_feedrate;
  zPexIPArray qref_path; /* size = joint size */
  int joint_size;
  allLinkPinPathIKInfo all_link_pin_path_ik_info; /* size = all link size of a chain */
  int all_link_size;
  refPathArray ref_path_array; /* size = ik_num */
  int ik_num; /* = the size of ref_path_array */
} p2pPathInfo;

zArrayClass( p2pPathArray, p2pPathInfo );
p2pPathArray g_p2p_array; /* size = keyframe size */

/* the main targets of this sample code */
rkChain g_chain;
rkglChain gr;

int g_selected_cp = -1;
int g_selected_path_id = -1;
int g_popped_path_id = -1;
int g_selected_ik_id = -1;
rkglSelectionBuffer g_sb;

#define SLICE_NUM 50

/* keyframe_alpha */
double g_keyframe_alpha = 0.3;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
rkglShadow g_shadow;

static const GLdouble g_znear = -1000.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.001;

GLuint *g_tex_id; /* color buffer texture id */
GLuint g_fb_id; /* frame buffer object id */
GLuint g_rb_id; /* render buffer texture id */

#define NAME_NURBS 100
#define P2P_KEYFRAME_NUM 2
#define INTERMEDIATE_CP_NUM 2
#define SIZE_CP 10.0

int createPinInfoDisplayList(rkChain* chain, allLinkPinIKInfo *all_link_pin_ik_info, double alpha, rkglLight* light)
{
  int link_id, display_id;
  zOpticalInfo **oi_alt;
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
  if( !( oi_alt = zAlloc( zOpticalInfo*, rkChainLinkNum(chain) ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( zOpticalInfo, rkChainLinkNum(&g_chain) )." );
    return -1;
  }
  /* pin link color changed */
  for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ ){
    if( all_link_pin_ik_info->buf[link_id].pin == PIN_LOCK_6D ){
      /* Red */
      oi_alt[link_id] = zAlloc( zOpticalInfo, 1 );
      zOpticalInfoCreate( oi_alt[link_id], 1.0, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, g_keyframe_alpha, NULL );
    } else if( all_link_pin_ik_info->buf[link_id].pin == PIN_LOCK_POS3D ){
      /* Yellow */
      oi_alt[link_id] = zAlloc( zOpticalInfo, 1 );
      zOpticalInfoCreate( oi_alt[link_id], 1.0, 8.0, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, g_keyframe_alpha, NULL );
    } else {
      /* Default */
      oi_alt[link_id] = NULL;
    }
    rkglChainAlternateLinkOptic( &gr, link_id, oi_alt[link_id], light );
  } /* end of pin link color changed */

  display_id = rkglBeginList();
  rkglChainPhantomize( &gr, alpha, light );
  glEndList();

  for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ )
    if( oi_alt[link_id] ) zOpticalInfoDestroy( oi_alt[link_id] );

  return display_id;
}


/* keyframe setting ---------------------------------------------------------------- */

/* define as user application */
void rkglChainUnloadKeyframeInfo()
{
  int i;
  for( i=0; i < zArraySize( &g_keyframe_array ); i++ ){
    keyFrameInfo* kf = zArrayElemNC( &g_keyframe_array, i );
    zVecFree( kf->q );
    zArrayFree( &kf->all_link_pin_ik_info );
  }
  zArrayFree( &g_keyframe_array );
}

bool rkglChainLoadKeyframeInfo(rkChain *chain, const int input_keyframe_size, double** input_q, int** input_pin)
{
  int key_id, link_id, jid, cell_id; /* cell_id : constrained cell id for IK */
  rkglChainAttr attr;

  printf("size of link = %d\n", rkChainLinkNum(chain) );
  printf("size of joint = %d\n", rkChainJointSize(chain) );

  if( zArraySize( &g_keyframe_array ) > 0 )
    rkglChainUnloadKeyframeInfo();
  zArrayAlloc( &g_keyframe_array, keyFrameInfo, input_keyframe_size );

  rkglChainAttrInit( &attr );
  for( key_id=0; key_id < input_keyframe_size; key_id++ ){
    keyFrameInfo* kf = zArrayElemNC( &g_keyframe_array, key_id );
    zArrayAlloc( &kf->all_link_pin_ik_info, linkPinIKInfo, rkChainLinkNum(chain) );
    if( zArraySize( &kf->all_link_pin_ik_info ) <= 0 ){
      ZALLOCERROR();
      ZRUNERROR( "Failed to zAlloc( linkPinIKInfo, rkChainLinkNum(chain) )." );
      return false;
    }
    for( link_id=0; link_id < rkChainLinkNum(chain); link_id++ ){
      kf->all_link_pin_ik_info.buf[link_id].pin = (pinStatus)(input_pin[key_id][link_id]);
      kf->all_link_pin_ik_info.buf[link_id].cell_size = IK_PIN_CELL_SIZE;
      switch( kf->all_link_pin_ik_info.buf[link_id].pin ){
      case PIN_LOCK_6D:
        kf->all_link_pin_ik_info.buf[link_id].c[0].w = IK_PIN_WEIGHT; /* goal weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[0].is_constrained = true;
        kf->all_link_pin_ik_info.buf[link_id].c[0].cell_type = IK_CELL_TYPE_WLD_POS;
        kf->all_link_pin_ik_info.buf[link_id].c[1].w = IK_PIN_WEIGHT; /* init weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[1].is_constrained = true;
        kf->all_link_pin_ik_info.buf[link_id].c[1].cell_type = IK_CELL_TYPE_WLD_ATT;
        break;
      case PIN_LOCK_POS3D:
        kf->all_link_pin_ik_info.buf[link_id].c[0].w = IK_PIN_WEIGHT; /* goal weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[0].is_constrained = true;
        kf->all_link_pin_ik_info.buf[link_id].c[0].cell_type = IK_CELL_TYPE_WLD_POS;
        kf->all_link_pin_ik_info.buf[link_id].c[1].w = IK_NO_WEIGHT;  /* init weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[1].is_constrained = false;
        kf->all_link_pin_ik_info.buf[link_id].c[1].cell_type = IK_CELL_TYPE_WLD_ATT;
        break;
      case PIN_LOCK_OFF:
        kf->all_link_pin_ik_info.buf[link_id].c[0].w = IK_NO_WEIGHT; /* goal weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[0].is_constrained = false;
        kf->all_link_pin_ik_info.buf[link_id].c[0].cell_type = IK_CELL_TYPE_WLD_POS;
        kf->all_link_pin_ik_info.buf[link_id].c[1].w = IK_NO_WEIGHT; /* init weight pos */
        kf->all_link_pin_ik_info.buf[link_id].c[1].is_constrained = false;
        kf->all_link_pin_ik_info.buf[link_id].c[1].cell_type = IK_CELL_TYPE_WLD_ATT;
        break;
      default: ;
      }
      for( cell_id=0; cell_id < kf->all_link_pin_ik_info.buf[link_id].cell_size; cell_id++ ){
        kf->all_link_pin_ik_info.buf[link_id].c[cell_id].dw  = 0.0; /* default init weight vel */
        kf->all_link_pin_ik_info.buf[link_id].c[cell_id].d2w = 0.0; /* default init weight acc */
      }
    }
    for( jid=0; jid < rkChainJointSize(chain); jid++ ){
      printf( "test_q[%d][%d] = %f\n", key_id, jid, input_q[key_id][jid]*180.0/zPI );
    }
    printf( "\n" );
    if( !( kf->q = zVecCloneArray( input_q[key_id], rkChainJointSize(chain) ) ) ){
      ZRUNERROR( "Failed to zVecCloneArray( test_q[%d], rkChainJointSize(chain) ).", key_id );
      return false;
    };
    zFrame3DFromAA( &kf->root,
                    test_root_frame[key_id][0], test_root_frame[key_id][1],
                    test_root_frame[key_id][2], test_root_frame[key_id][3],
                    test_root_frame[key_id][4], test_root_frame[key_id][5] );

    /* debug print ----------------------------------------------------------------------*/
    rkChainSetJointDisAll( chain, kf->q );
    rkChainUpdateFK( chain );
    for( link_id=0; link_id < rkChainLinkNum( chain ); link_id++ ){
      if( kf->all_link_pin_ik_info.buf[link_id].pin != PIN_LOCK_OFF ){
        printf("- PIN_LOCK : link[%d] %s ---------------------\n",
               link_id, zName( rkChainLink(chain, link_id) ) );
        printf("att :\n");   zMat3DPrint( rkChainLinkWldAtt( chain, link_id ) );
        printf("pos : "); zVec3DPrint( rkChainLinkWldPos( chain, link_id ) );
        printf("\n");
      }
    }
    /* end of debug print ----------------------------------------------------------------*/

    /* rkChainSetRootFrame( chain, &kf->root ); */
    zFrame3DCopy( &kf->root, rkChainOrgFrame(chain) );
    rkChainFK( chain, kf->q );
    kf->display_id = createPinInfoDisplayList( chain, &kf->all_link_pin_ik_info, g_keyframe_alpha, &g_light );
    if( kf->display_id < 0 ){
      return false;
    }
  } /* end of for( zArraySize( &g_keyframe_array ) ) */

  return true;
}

bool clone_and_set_keyframelist(const int input_keyframe_size, double** input_q, int** input_pin)
{
  if( !rkglChainLoadKeyframeInfo( &g_chain, input_keyframe_size, input_q, input_pin ) ){
    ZRUNWARN( "Failed rkglChainLoadKeyframeInfo()" );
    return false;
  }
  return true;
}

/* end keyframe setting------------------------------------------------------------- */

/* all_link_pin_path_ik_info setting ----------------------------------------------- */

void free_ref_path_array(refPathArray* ref_path_array)
{
  int ik_id, ik_num;

  if( ( ik_num = zArraySize( ref_path_array ) ) > 0 ){
    for( ik_id=0; ik_id < ik_num; ik_id++ ){
      if( ref_path_array->buf[ik_id].nurbs.knot != NULL )
        zNURBS3DDestroy( &ref_path_array->buf[ik_id].nurbs );
    }
    zArrayFree( ref_path_array );
  }
}

bool clone_and_set_ref_path_input(int path_size, refPathInput* src_ref_path_input_array, p2pPathArray* p2p_array)
{
  int path_id, ik_id;

  for( path_id=0; path_id < path_size; path_id++ ){
    refPathInput* src = &src_ref_path_input_array[path_id];
    p2pPathInfo* p2p_buf = &p2p_array->buf[path_id];
    refPathArray* ref_path_array = &p2p_buf->ref_path_array;
    free_ref_path_array( ref_path_array );
    p2p_buf->ik_num = src->ik_num;
    zArrayAlloc( ref_path_array, refPath, src->ik_num );
    for( ik_id=0; ik_id < src->ik_num; ik_id++ ){
      zCopy( rkIKRegister, &src->ik_reg_array[ik_id], &ref_path_array->buf[ik_id].ik_reg );
    }
  }

  return true;
}




/* end path_info setting ----------------------------------------------------------- */


/* interpolate path -------------------------------------------------------------- */

int find_cp(rkglSelectionBuffer *sb)
{
  int path_size, path_id, ik_num, ik_id;
  int i;

  path_size = zArraySize( &g_p2p_array );

  rkglSelectionRewind( sb );
  g_selected_cp = -1;
  g_selected_path_id = -1;
  g_selected_ik_id = -1;

  for( path_id=0; path_id < path_size; path_id++ ){
    p2pPathInfo* p2p_buf = &g_p2p_array.buf[path_id];
    ik_num = p2p_buf->ik_num;
    for( ik_id=0; ik_id < ik_num; ik_id++ ){
      zNURBS3D* nurbs = &p2p_buf->ref_path_array.buf[ik_id].nurbs;
      rkIKRegister* ik_reg = &p2p_buf->ref_path_array.buf[ik_id].ik_reg;
      if( g_ik_reg_cls->pos( ik_reg ) ){
        for( i=0; i<sb->hits; i++ ){
          if( rkglSelectionName(sb,0) == (NAME_NURBS + 100*NAME_NURBS*path_id + ik_id) &&
              rkglSelectionName(sb,1) >= 0 &&
              rkglSelectionName(sb,1) < zNURBS3D1CPNum(nurbs) ){
            g_selected_cp = rkglSelectionName(sb,1);
            g_selected_path_id = path_id;
            g_selected_ik_id = ik_id;
            break;
          }
          rkglSelectionNext( sb );
        }
      }
    }

  }
  return g_selected_cp;
}

double accumulate_normalized_joint_norm(rkChain* chain, double s, zVec q1, zVec q2)
{
  int link_id, j, jid;
  double min[6], max[6], jsize, range, ret_s;
  zVec nq1, nq2;

  nq1 = zVecAlloc( zVecSize(q1) );
  nq2 = zVecAlloc( zVecSize(q2) );
  jid = 0.0;
  for( link_id=0; link_id < rkChainLinkNum(chain); link_id++ ){
    jsize = rkChainLinkJointDOF(chain, link_id);
    if( jsize < 1.0 ) continue;
    rkLinkJointGetMin( rkChainLink( chain, link_id ), min );
    rkLinkJointGetMax( rkChainLink( chain, link_id ), max );
    for( j=0; j<jsize; j++ ){
      /* printf( "q[%02d] min : max = %f : %f\n", jid, min[j], max[j] ); */
      range = fabs( max[j] - min[j] );
      if( range > DBL_MIN ){
        zVecElemNC(nq1, jid) = ( zVecElemNC(q1, jid) - min[j] ) / range;
        zVecElemNC(nq2, jid) = ( zVecElemNC(q2, jid) - min[j] ) / range;
      } else{
        zVecElemNC(nq1, jid) = 0.0;
        zVecElemNC(nq2, jid) = 0.0;
      }
      jid++;
    }
  }
  ret_s = s + zVecSqrDist( nq1, nq2 );
  /* printf("q1 = "); zVecPrint(q1); */
  /* printf("q2 = "); zVecPrint(q2); */
  /* printf("nq1 = "); zVecPrint(nq1); */
  /* printf("nq2 = "); zVecPrint(nq2); */
  printf("ret_s = %f\n\n", ret_s);
  zVecFree( nq1 );
  zVecFree( nq2 );

  return ret_s;
}

void calc_p2p_feedrate(rkChain *chain, keyFrameInfo* kf1, keyFrameInfo* kf2)
{
  kf2->feedrate.s = accumulate_normalized_joint_norm( chain, kf1->feedrate.s, kf1->q, kf2->q );
}

void calc_feedrate(rkChain *chain, keyFrameInfoArray* keyframe_array)
{
  int path_id, size;

  size = zArraySize( keyframe_array );
  zArrayElemNC(keyframe_array, 0)->feedrate.s = 0.0;
  for( path_id=0; path_id < size-1; path_id++ ){
    keyFrameInfo* kf1 = zArrayElemNC(keyframe_array, path_id);
    keyFrameInfo* kf2 = zArrayElemNC(keyframe_array, path_id+1);
    calc_p2p_feedrate( chain, kf1, kf2 );
  }
}

void free_qref_path(zPexIPArray* qref_path)
{
  if( zArraySize( qref_path ) > 0 )
    zArrayFree( qref_path );
}

void init_qref_path(zPexIPArray* qref_path, int joint_size)
{
  free_qref_path( qref_path );
  zArrayAlloc( qref_path, zPexIP, joint_size );
}

void init_one_link_pin_path_ik_info(linkPinPathIKInfo* link_pin_path_ik_info, int link_id, int ik_constrained_cell_size){
  int cell_id; /* cell_id : constrained cell id for IK */

  /* set cell size */
  link_pin_path_ik_info->cell_size = ik_constrained_cell_size;
  for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ ){
    link_pin_path_ik_info->c[cell_id].is_constrained = false;
  }
}

void free_all_link_pin_path_ik_info(allLinkPinPathIKInfo* all_link_pin_path_ik_info)
{
  if( zArraySize( all_link_pin_path_ik_info ) > 0 )
    zArrayFree( all_link_pin_path_ik_info );
}

bool init_all_link_pin_path_ik_info(allLinkPinPathIKInfo* all_link_pin_path_ik_info, int all_link_size, int ik_constrained_cell_size)
{
  int link_id;

  free_all_link_pin_path_ik_info( all_link_pin_path_ik_info );
  zArrayAlloc( all_link_pin_path_ik_info, linkPinPathIKInfo, all_link_size );

  for( link_id=0; link_id < all_link_size; link_id++){
    init_one_link_pin_path_ik_info( &all_link_pin_path_ik_info->buf[link_id], link_id, ik_constrained_cell_size );
  }

  return true;
}

bool init_p2p_array(rkChain *chain, p2pPathArray *p2p_array, int p2p_num, int ik_constrained_cell_size)
{
  int pre_path_size, path_id;

  if( (pre_path_size = zArraySize( p2p_array )) > 0 ){
    for( path_id=0; path_id < pre_path_size; path_id++ ){
      /* @TODO */
      p2pPathInfo* p2p_buf = zArrayElemNC( p2p_array, path_id );
      free_qref_path( &p2p_buf->qref_path );
      free_all_link_pin_path_ik_info( &p2p_buf->all_link_pin_path_ik_info );
      refPathArray* ref_path_array = &p2p_buf->ref_path_array;
      free_ref_path_array( ref_path_array );
    }
    zArrayFree( p2p_array );
  }
  zArrayAlloc( &g_p2p_array, p2pPathInfo, p2p_num );
  for( path_id=0; path_id < p2p_num; path_id++ ){
    p2pPathInfo* p2p_buf = zArrayElemNC( p2p_array, path_id );
    p2p_buf->joint_size = rkChainJointSize(chain);
    p2p_buf->all_link_size = rkChainLinkNum(chain);
    init_qref_path( &p2p_buf->qref_path, p2p_buf->joint_size );
    if( !init_all_link_pin_path_ik_info( &p2p_buf->all_link_pin_path_ik_info, p2p_buf->all_link_size, ik_constrained_cell_size ) ){
      return false;
    }
    /* @TODO */
    /* initilaize ref_path_array */
  }

  return true;
}

void interpolate_p2p_qref_path(rkChain* chain, zPexIPArray* qref_path, keyFrameInfo* kf1, keyFrameInfo* kf2)
{
  int jid;
  double term = kf2->feedrate.s - kf1->feedrate.s;
  for( jid=0; jid < zArraySize( qref_path ); jid++ ){
    zPexIPCreateBoundary( &qref_path->buf[jid],
                          term,
                          zVecElemNC(kf1->q, jid), 0.0, 0.0, /* x1, v1, a1 */
                          zVecElemNC(kf2->q, jid), 0.0, 0.0, /* x2, v2, a2 */
                          NULL );
  } /* end of for loop link_id of link num times */
}

void interpolate_p2p_ik_weight_path(rkChain* chain, allLinkPinPathIKInfo* all_link_pin_path_ik_info, keyFrameInfo* kf1, keyFrameInfo* kf2)
{
  int link_id, cell_id; /* cell_id : constrained cell id for IK */
  double term = kf2->feedrate.s - kf1->feedrate.s;
  for( link_id=0; link_id < rkChainLinkNum(chain); link_id++ ){
    linkPinPathIKInfo* link_pin_path_ik_info = &all_link_pin_path_ik_info->buf[link_id];
    for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ ){
      zPexIPCreateBoundary( &link_pin_path_ik_info->c[cell_id].weight_path,
                            term,
                            kf1->all_link_pin_ik_info.buf[link_id].c[cell_id].w, 0.0, 0.0, /* x1, v1, a1 */
                            kf2->all_link_pin_ik_info.buf[link_id].c[cell_id].w, 0.0, 0.0, /* x2, v2, a2 */
                            NULL );
    } /* end of for loop i of weight pos & att num times */
  } /* end of for loop link_id of link num times */
}

/* 2 points, start keyframe index, zNURBS3D*, start 3D position, end 3D position */
void interpolate_p2p_nurbs_cp(int kf1idx, rkChain *chain, zNURBS3D* nurbs, zVec3D* start, zVec3D* end)
{
  int j;
  /* start */
  zVec3DCopy( start, zNURBS3D1CP(nurbs, kf1idx) );
  /* end */
  zVec3DCopy( end, zNURBS3D1CP(nurbs, kf1idx + INTERMEDIATE_CP_NUM + 1 ) );
  /* intermediate points */
  zVec3D diff3D;
  zVec3DSub( end, start, &diff3D );
  zVec3D cp1;
  zVec3DCat( start, 0.25, &diff3D, &cp1);
  zVec3D cp2;
  zVec3DCat( end, -0.25, &diff3D, &cp2);
  for( j=kf1idx+1; j<zNURBS3D1CPNum(nurbs)-INTERMEDIATE_CP_NUM; j++ ){
    zVec3DCreate( zNURBS3D1CP(nurbs, j),
                  cp1.c.x,
                  cp1.c.y,
                  cp1.c.z );
    zVec3DCreate( zNURBS3D1CP(nurbs, j + INTERMEDIATE_CP_NUM - 1),
                  cp2.c.x,
                  cp2.c.y,
                  cp2.c.z );
  }
}

bool interpolate_path(rkChain* chain, keyFrameInfoArray* keyframe_array, p2pPathArray* p2p_array)
{
  int path_id, keyframe_size, ik_id, ref_link_id;
  zVec3D start_pos, end_pos, ref_ap;
  zOpticalInfo oi;

  keyframe_size = zArraySize( keyframe_array );
  if( keyframe_size < 2 ) {
    ZRUNERROR( "too small keyframe_array size = %d (must be >= 2)", keyframe_size );
    return false;
  }

  for( path_id=0; path_id < keyframe_size - 1; path_id++ ){
    keyFrameInfo* kf1 = zArrayElemNC(keyframe_array, path_id);
    keyFrameInfo* kf2 = zArrayElemNC(keyframe_array, path_id+1);
    p2pPathInfo* p2p_buf = &p2p_array->buf[path_id];
    p2p_buf->start_feedrate.s = kf1->feedrate.s;
    p2p_buf->start_feedrate.ds = kf1->feedrate.ds;
    p2p_buf->start_feedrate.d2s = kf1->feedrate.d2s;
    p2p_buf->goal_feedrate.s = kf2->feedrate.s;
    p2p_buf->goal_feedrate.ds = kf2->feedrate.ds;
    p2p_buf->goal_feedrate.d2s = kf2->feedrate.d2s;
    interpolate_p2p_qref_path( chain, &p2p_buf->qref_path, kf1, kf2 );
    interpolate_p2p_ik_weight_path( chain, &p2p_buf->all_link_pin_path_ik_info, kf1, kf2 );

    /* start */
    zFrame3DCopy( &kf1->root, rkChainOrgFrame(chain) );
    rkChainFK( chain, kf1->q );

    /* path */
    for( ik_id=0; ik_id < p2p_buf->ik_num; ik_id++ ){
      rkIKRegister* ik_reg = &p2p_buf->ref_path_array.buf[ik_id].ik_reg;
      if( g_ik_reg_cls->com( ik_reg ) ){
        /* start */
        zFrame3DCopy( &kf1->root, rkChainOrgFrame(chain) );
        rkChainFK( chain, kf1->q );
        zVec3DCopy( rkChainWldCOM(chain), &start_pos );
        /* end */
        zFrame3DCopy( &kf2->root, rkChainOrgFrame(chain) );
        rkChainFK( chain, kf2->q );
        zVec3DCopy( rkChainWldCOM(chain), &end_pos );
      } else
      if( g_ik_reg_cls->link( ik_reg ) ){
        ref_link_id = g_ik_reg_cls->get_link_id( ik_reg );
        /* start */
        zFrame3DCopy( &kf1->root, rkChainOrgFrame(chain) );
        rkChainFK( chain, kf1->q );
        zVec3DCopy( rkChainLinkWldPos(chain, ref_link_id), &start_pos );
        /* end */
        zFrame3DCopy( &kf2->root, rkChainOrgFrame(chain) );
        rkChainFK( chain, kf2->q );
        zVec3DCopy( rkChainLinkWldPos(chain, ref_link_id), &end_pos );
      } else
      if( g_ik_reg_cls->link_ap( ik_reg ) ){
        ref_link_id = g_ik_reg_cls->get_link_id( ik_reg );
        g_ik_reg_cls->get_ap( ik_reg, &ref_ap.c.x, &ref_ap.c.y, &ref_ap.c.z );
        /* @TODO */
        /* Transform AP pose reference with Link -> World */
        /* Get AP start & end pose ( position & orientation ) */
      }
      zNURBS3D* nurbs;
      if( g_ik_reg_cls->pos( ik_reg ) ){
        nurbs = &p2p_buf->ref_path_array.buf[ik_id].nurbs;
        /* the number of intermediate control points */
        zNURBS3D1Alloc( nurbs, P2P_KEYFRAME_NUM + INTERMEDIATE_CP_NUM, 3 );
        zNURBS3D1SetSliceNum( nurbs, SLICE_NUM );
        interpolate_p2p_nurbs_cp( path_id, chain, nurbs, &start_pos, &end_pos );
      }
    }
  } /* end of for loop i of keyframe num times */

  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );

  return true;
}

/* end interpolate path ----------------------------------------------------------------- */


/* print path --------------------------------------------------------------------------- */

void print_header_label_feedrate(FILE *fp, double end_s)
{
  fprintf( fp, "#No. s(0.0-%f) ", end_s );
}

void print_header_label_of_qref_path(FILE *fp, int joint_size)
{
  int jid;

  for( jid=0; jid < joint_size; jid++ )
    fprintf( fp, "q[%d] ", jid );
}

void print_header_label_of_ik_weight_path(FILE *fp, int all_link_size, int cell_size)
{
  int link_id, cell_id; /* cell_id : constrained cell id for IK */

  for( link_id=0; link_id < all_link_size; link_id++ )
    for( cell_id=0; cell_id<cell_size; cell_id++)
      fprintf( fp, "weight[%d][%d] ", link_id, cell_id);
}

void print_headr_label_of_ik_nurbs_path(FILE *fp, int cell_size)
{
  int cell_id; /* cell_id : constrained cell id for IK */
  for( cell_id=0; cell_id<cell_size; cell_id++)
    fprintf( fp, "cell[%d].x cell[%d].y cell[%d].z ", cell_id, cell_id, cell_id );
}

void print_interpolated_qref_path(FILE *fp, double s, zPexIPArray* qref_path)
{
  int jid;

  for( jid=0; jid < zArraySize( qref_path ); jid++ )
    fprintf( fp, "%.10f ", zPexIPVal( &qref_path->buf[jid], s ) );
}

void print_interpolated_ik_weight_path(FILE *fp, double s, zPexIP *weight_path)
{
  fprintf( fp, "%.10f ", zPexIPVal( weight_path, s ) );
}

void print_interpolated_nurbs_path_3d_position(FILE *fp, double s, zNURBS3D *nurbs, bool is_path)
{
  zVec3D v;
  if( is_path ){
    zNURBS3D1Vec( nurbs, s, &v );
    fprintf( fp, "%.10f %.10f %.10f ", v.c.x, v.c.y, v.c.z );
  }
  else
    fprintf( fp, "0.0 0.0 0.0 " ); /* fprintf( fp, "NaN NaN NaN " ); */
}

void print_interpolated_path(FILE *fp, rkChain* chain, p2pPathArray *p2p_array, int s_slice_num)
{
  int joint_size, all_link_size, cell_size, path_size, path_id, sid, link_id, cell_id, ik_id;
  double s, end_s;

  joint_size = zArraySize( &p2p_array->buf[0].qref_path );
  all_link_size = p2p_array->buf[0].all_link_size;
  cell_size = p2p_array->buf[0].all_link_pin_path_ik_info.buf[0].cell_size;
  path_size = zArraySize( p2p_array );

  end_s = p2p_array->buf[path_size-1].goal_feedrate.s;

  print_header_label_feedrate( fp, end_s );
  print_header_label_of_qref_path( fp, joint_size );
  print_header_label_of_ik_weight_path( fp, all_link_size, cell_size );
  print_headr_label_of_ik_nurbs_path( fp, cell_size );
  fprintf( fp, "\n" );

  for( path_id=0; path_id < path_size; path_id++){
    p2pPathInfo* p2p_buf = &p2p_array->buf[path_id];
    for( sid=0; sid<=(int)(s_slice_num); sid++ ){
      s = (double)(sid) * end_s / s_slice_num;
      fprintf( fp, "%d %.10f ", sid, s );
      print_interpolated_qref_path( fp, s, &p2p_buf->qref_path );
      for( link_id=0; link_id < all_link_size; link_id++ ){
        for( cell_id=0; cell_id < cell_size; cell_id++){
          pinPathIKInfo* cell_info = &p2p_buf->all_link_pin_path_ik_info.buf[link_id].c[cell_id];
          print_interpolated_ik_weight_path( fp, s, &cell_info->weight_path );
        }
      }
      for( ik_id=0; ik_id < p2p_buf->ik_num; ik_id++ ){
        refPath* ref_path = &p2p_buf->ref_path_array.buf[ik_id];
        bool is_path = true;
        print_interpolated_nurbs_path_3d_position( fp, (s / end_s), &ref_path->nurbs, is_path );
      }
      fprintf( fp, "\n" );
    }
  }
}

/* end print path ------------------------------------------------------------------- */


/* ik ------------------------------------------------------------------------------- */

bool set_constrained_mode_for_each_p2p_path(keyFrameInfoArray* keyframe_array, p2pPathArray *p2p_array)
{
  int path_id, link_id, cell_id; /* cell_id : constrained cell id for IK */

  if( zArraySize( p2p_array ) + 1 != zArraySize( keyframe_array ) ){
    ZRUNERROR("Invalid size between path and keyframe. Expected (path size) + 1 == (keyframe_size).");
    return false;
  }
  for( path_id=0; path_id < zArraySize( p2p_array ); path_id++ ){
    p2pPathInfo* p2p_buf = &p2p_array->buf[path_id];
    keyFrameInfo* kf1 = &keyframe_array->buf[path_id];
    keyFrameInfo* kf2 = &keyframe_array->buf[path_id + 1];
    for( link_id=0; link_id < zArraySize( &p2p_buf->all_link_pin_path_ik_info ); link_id++ ){
      linkPinPathIKInfo* link_pin_path_ik_info = &p2p_buf->all_link_pin_path_ik_info.buf[link_id];
      for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ ){
        link_pin_path_ik_info->c[cell_id].cell_type = kf1->all_link_pin_ik_info.buf[link_id].c[cell_id].cell_type;
        link_pin_path_ik_info->c[cell_id].is_constrained =
          ( ( kf1->all_link_pin_ik_info.buf[link_id].c[cell_id].is_constrained )
            ||
            ( kf2->all_link_pin_ik_info.buf[link_id].c[cell_id].is_constrained ) );
      }
    }
  }

  return true;
}

void register_cell_in_one_link_for_IK(double s, rkChain* chain, int path_id, int link_id)
{
  int cell_id;/* cell_id : constrained cell id for IK */
  double weight;
  rkIKAttr attr;
  attr.id = link_id;
  linkPinPathIKInfo* link_pin_path_ik_info = &g_p2p_array.buf[path_id].all_link_pin_path_ik_info.buf[link_id];

  for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ ){
    if( link_pin_path_ik_info->c[cell_id].is_constrained ){
      if( link_pin_path_ik_info->c[cell_id].cell_type == IK_CELL_TYPE_WLD_ATT )
        link_pin_path_ik_info->c[cell_id].cell = rkChainRegIKCellWldAtt( chain, &attr, RK_IK_ATTR_ID );
      if( link_pin_path_ik_info->c[cell_id].cell_type == IK_CELL_TYPE_WLD_POS )
        link_pin_path_ik_info->c[cell_id].cell = rkChainRegIKCellWldPos( chain, &attr, RK_IK_ATTR_ID | RK_IK_ATTR_AP );
      weight = zPexIPVal( &link_pin_path_ik_info->c[cell_id].weight_path, s );
      rkIKCellSetWeight( link_pin_path_ik_info->c[cell_id].cell, weight, weight, weight );
    }
  }
}

void unregister_cell_in_one_link_for_IK(rkChain* chain, int path_id, int link_id)
{
  int cell_id;/* cell_id : constrained cell id for IK */
  linkPinPathIKInfo* link_pin_path_ik_info = &g_p2p_array.buf[path_id].all_link_pin_path_ik_info.buf[link_id];
  for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ )
    if( link_pin_path_ik_info->c[cell_id].is_constrained )
      rkChainUnregIKCell( chain, link_pin_path_ik_info->c[cell_id].cell );
}

void register_all_constrained_links_for_IK(double s, rkChain* chain, int path_id)
{
  int link_id;
  p2pPathInfo* p2p_buf = &g_p2p_array.buf[path_id];
  for( link_id=0; link_id < zArraySize( &p2p_buf->all_link_pin_path_ik_info ); ++link_id ){
    register_cell_in_one_link_for_IK( s, chain, path_id, link_id);
  }
}

void unregister_all_constrained_links_for_IK(rkChain* chain, int path_id)
{
  int link_id;
  p2pPathInfo* p2p_buf = &g_p2p_array.buf[path_id];
  for( link_id=0; link_id < zArraySize( &p2p_buf->all_link_pin_path_ik_info ); ++link_id ){
    unregister_cell_in_one_link_for_IK( chain, path_id, link_id );
  }
}

void set_one_link_reference_of_3D_translate_position_for_IK(int path_id, int ik_id, zVec3D* ref_pos)
{
  rkIKCellSetRefVec( g_p2p_array.buf[path_id].ref_path_array.buf[ik_id].cell, ref_pos );
}

void set_one_link_reference_of_3D_attitude_position_for_IK(int path_id, int ik_id, zMat3D* ref_att)
{
  zVec3D zyx;
  zMat3DToZYX( ref_att, &zyx );
  rkIKCellSetRefVec( g_p2p_array.buf[path_id].ref_path_array.buf[ik_id].cell, &zyx );
}

/* inverse kinematics */
void update_alljoint_by_IK(rkChain* chain, zVec init_joints)
{
  /* prepare IK */
  /* rkChainDeactivateIK( chain ); */
  /* rkChainBindIK( chain ); */
  if( init_joints != NULL ) {
    rkChainSetJointDisAll( chain, init_joints );
    rkChainUpdateFK( chain );
  }
  /* IK */
  printf("pre IK Joint[deg]  = "); zVecPrint(zVecMulDRC(zVecClone(init_joints),180.0/zPI));
  int iter = 100;
  double ztol = zTOL;
  /* backup in case the result of rkChainIK() is NaN */
  rkChain clone_chain;
  rkChainClone( chain, &clone_chain );
  zVec dis = zVecAlloc( rkChainJointSize( chain ) ); /* IK output */
  rkChainIK( chain, dis, ztol, iter );
  if( zVecIsNan(dis) ){
    printf("the result of rkChainIK() is NaN\n");
    rkChainCopyState( &clone_chain, chain );
  }
  printf("post IK Joint[deg] = "); zVecPrint(zVecMulDRC(zVecClone(dis),180.0/zPI));
  rkChainDestroy( &clone_chain );
  zVecFree(dis);
}

zVec pop_qref(double s, zPexIPArray* qref_path)
{
  int jid;
  zVec out_qref;

  out_qref = zVecAlloc( zArraySize( qref_path ) );
  for( jid=0; jid < zArraySize( qref_path ); jid++ ){
    out_qref->buf[jid] = zPexIPVal( &qref_path->buf[jid], s );
  }
  return out_qref;
}

void pop_nurbs_point(double s, zNURBS3D *nurbs, zVec3D* out_point)
{
  zNURBS3D1Vec( nurbs, s, out_point );
}

bool pop_pose(double s, rkChain* chain, p2pPathArray *p2p_array)
{
  int path_id;
  path_id=0;
  bool is_find = false;
  p2pPathInfo* p2p_buf;

  while( path_id < zArraySize( p2p_array ) ){
    p2p_buf = &p2p_array->buf[path_id];
    if( p2p_buf->start_feedrate.s <= s && s < p2p_buf->goal_feedrate.s ){
      is_find = true;
      break;
    }
    path_id++;
  }
  if( !is_find ){
    ZRUNERROR("Out of range feedrate s = %f which must be within %f <= s < %f.", s, p2p_array->buf[0].start_feedrate.s, p2p_buf->goal_feedrate.s);
    return false;
  }
  zVec qref = pop_qref( s, &p2p_buf->qref_path );
  /* no path free joint */
  {
    rkChainSetJointDisAll( chain, qref );
    rkChainUpdateFK( chain );
  }
  if( g_popped_path_id != -1 && g_popped_path_id != path_id ){
    /* for previous popped path */
    unregister_all_constrained_links_for_IK(chain, g_popped_path_id);
    /* @TODO unregister ref_path_array[ik_num]->ik_reg */
    g_popped_path_id = path_id; /* keep for next calling pop_pose*/
  }
  register_all_constrained_links_for_IK( s, chain, path_id );

  rkChainDeactivateIK( chain );
  rkChainBindIK( chain );

  /* set reference */
  zVec3D ref_pos;

  int ik_id;
  for( ik_id=0; ik_id < p2p_buf->ik_num; ik_id++ ){
    refPath* ref_path  = &p2p_buf->ref_path_array.buf[ik_id];
     ref_path->cell = g_ik_reg_cls->call_api( &ref_path->ik_reg, chain );
    if( g_ik_reg_cls->pos( &ref_path->ik_reg ) ){
      pop_nurbs_point( (s / p2p_buf->goal_feedrate.s),
                       &p2p_buf->ref_path_array.buf[ik_id].nurbs,
                       &ref_pos );
      set_one_link_reference_of_3D_translate_position_for_IK( path_id, ik_id, &ref_pos );
      /* @TODO : attitude */
      /* set_one_link_reference_of_3D_attitude_position_for_IK( path_id, ik_id, &ref_att ); */
    }
  }
  /* IK */
  update_alljoint_by_IK( chain, qref );
  zVecFree( qref );

  return true;
}


/* ---------------------------------------------------------------------------------- */

void release_path()
{
  int path_size, path_id, link_id, cell_id, jid; /* cell_id : constrained cell id for IK */

  path_size = zArraySize(&g_p2p_array);
  for( path_id=0; path_id < path_size; path_id++ ){
    p2pPathInfo* p2p_buf = zArrayElemNC( &g_p2p_array, path_id );
    /* release weight_path[2] */
    for( link_id=0; link_id < p2p_buf->all_link_size; link_id++ ){
      linkPinPathIKInfo* link_pin_path_ik_info = &p2p_buf->all_link_pin_path_ik_info.buf[link_id];
      for( cell_id=0; cell_id < link_pin_path_ik_info->cell_size; cell_id++ ){
        zPexIPFree( &link_pin_path_ik_info->c[cell_id].weight_path );
      }
    }
    /* release all_link_pin_path_ik_info */
    if( zArraySize( &p2p_buf->all_link_pin_path_ik_info ) > 0 )
      zArrayFree( &p2p_buf->all_link_pin_path_ik_info );
    /* release qref_path[] */
    for( jid=0; jid < p2p_buf->joint_size; jid++ )
      zPexIPFree( &p2p_buf->qref_path.buf[jid] );
    if( zArraySize( &p2p_buf->qref_path ) > 0 )
      zArrayFree( &p2p_buf->qref_path );
    /* release ref_path_array[] */
    refPathArray* ref_path_array = &p2p_buf->ref_path_array;
    free_ref_path_array( ref_path_array );
  } /* end for path_id=0->path_size */
  if( zArraySize( &g_p2p_array ) > 0 )
    zArrayFree( &g_p2p_array );
  if( zArraySize( &g_keyframe_array ) > 0 )
    rkglChainUnloadKeyframeInfo();
}

void change_pose(double s)
{
  pop_pose( s, &g_chain, &g_p2p_array );
}

/* 1. init_p2p_array */
/* 1, clone_and_set_keyframelist() */
/* 2. clone_and_set_ref_path_input() */
/* 3. run_test() */
double run_test(void)
{
  calc_feedrate( &g_chain, &g_keyframe_array );
  /* if( !init_p2p_array( &g_chain, &g_p2p_array, zArraySize( &g_keyframe_array ) - 1, IK_PIN_CELL_SIZE ) ) */
  /*   return -1.0; */

  set_constrained_mode_for_each_p2p_path( &g_keyframe_array, &g_p2p_array );

  /* interpolate path of weight & qref_& specific pin(NURBS) */
  interpolate_path( &g_chain, &g_keyframe_array, &g_p2p_array );

  FILE *fp;
  /* fp = stdout; */
  fp = fopen( "motion_path_view.dat", "w" );
  print_interpolated_path( fp, &g_chain, &g_p2p_array, SLICE_NUM);
  fclose( fp );

  zFrame3D tmp_root;
  zFrame3DFromAA( &tmp_root,
                  tmp_root_frame[0], tmp_root_frame[1],
                  tmp_root_frame[2], tmp_root_frame[3],
                  tmp_root_frame[4], tmp_root_frame[5] );
  zFrame3DCopy( &tmp_root, rkChainOrgFrame(&g_chain) );

  double s = tmp_s;
  pop_pose( s, &g_chain, &g_p2p_array );

  int path_size = zArraySize( &g_p2p_array );
  double end_s = g_p2p_array.buf[path_size-1].goal_feedrate.s;
  return end_s;
}


/******************************************************************************************/

void draw_chain(void)
{
  int i;

  for( i=0; i < 1; i++ ){
    rkglChainSetName( &gr, i ); /* NAME_CHAIN = i */
    rkglChainDraw( &gr );
  }
}

void draw_nurbs(void)
{
  int path_size, path_id, ik_id, ik_num;
  zRGB rgb;

  path_size = zArraySize( &g_p2p_array );
  for( path_id=0; path_id < path_size; path_id++ ){
    ik_num = g_p2p_array.buf[path_id].ik_num;
    for( ik_id=0; ik_id < ik_num; ik_id++ ){
      refPath* ref_path = &g_p2p_array.buf[path_id].ref_path_array.buf[ik_id];
      if( g_ik_reg_cls->pos( &ref_path->ik_reg ) ){
        glPushMatrix();
        zRGBSet( &rgb, 1.0, 1.0, 1.0 );
        glLoadName( NAME_NURBS + 100*NAME_NURBS*path_id + ik_id );
        glLineWidth( 3 );
        rkglNURBSCurve( &ref_path->nurbs, &rgb );
        zRGBSet( &rgb, 0.5, 1.0, 0.5 );
        glLineWidth( 1 );
        rkglNURBSCurveCP( &ref_path->nurbs, SIZE_CP, &rgb );
        glPopMatrix();
      }
    }
  }
}

void draw_scene(void)
{
  int i;
  draw_chain();
  for( i=0; i < zArraySize( &g_keyframe_array ); i++ ){
    glCallList( zArrayElemNC(&g_keyframe_array, i)->display_id );
  }
  draw_nurbs();
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();

  draw_scene();
}

void display_to_frame_buffer(GLFWwindow* window)
{
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, g_fb_id );
  display( window );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void clear_display(void)
{
  rkglClear();
}

void motion(GLFWwindow* window, double x, double y)
{
  zVec3D p;

  if( g_sb.hits == 0 ){
    rkglMouseDragFuncGLFW( window, x, y );
    return;
  }
  zNURBS3D* nurbs = &g_p2p_array.buf[g_selected_path_id].ref_path_array.buf[g_selected_ik_id].nurbs;
  if( g_selected_cp >= 0 && g_selected_cp < zNURBS3D1CPNum(nurbs) ){
    rkglUnproject( &g_cam, x, y, rkglSelectionZnearDepth(&g_sb), &p );
    zVec3DCopy( &p, zNURBS3D1CP(nurbs, g_selected_cp) );
  }
  rkglMouseStoreXY( floor(x), floor(y) );
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      rkglSelect( &g_sb, &g_cam, draw_nurbs, rkgl_mouse_x, rkgl_mouse_y, SIZE_CP, SIZE_CP );
      if( find_cp( &g_sb ) >= 0 )
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

void updata_texture_and_frame_buffer(int w, int h)
{
  if( g_tex_id == NULL )
    return;
  *g_tex_id = rkglTextureAssign( w, h, NULL );
  g_rb_id = rkglFramebufferAttachRenderbuffer( w, h );
  g_fb_id = rkglFramebufferAttachTexture( *g_tex_id );
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
    release_path();

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

void set_modelfiles(char* modelfile)
{
  g_modelfile = modelfile;
}

void setDefaultCallbackParam(void){
  rkglSetDefaultCallbackParam( &g_cam, 1.0, g_znear, g_zfar, 1.0, 5.0 );
}

void set_texture_id(GLuint* tex_id)
{
  g_tex_id = tex_id;
}

bool init(void)
{
  rkglSetDefaultCallbackParam( &g_cam, 1.0, 1.0, 20.0, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 0, 0, 0, 45, -30, 0 );

  if( rkglLightNum() == 0 ){
    glEnable(GL_LIGHTING);
    rkglLightCreate( &g_light, 0.5, 0.5, 0.5, 0.6, 0.6, 0.6, 0.2, 0.2, 0.2 );
    rkglLightMove( &g_light, 3, 5, 9 );
    rkglLightSetAttenuationConst( &g_light, 1.0 );
  }

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
  /* IK */
  rkChainCreateIK( &g_chain );
  rkChainRegIKJointAll( &g_chain, IK_JOINT_WEIGHT );

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

  /* double array -> double pointer */
  double** q_ptr;
  q_ptr = zAlloc( double*, TEST_KEYFRAME_SIZE );
  int** pin_ptr;
  pin_ptr = zAlloc( int*, TEST_KEYFRAME_SIZE );
  int key_id, link_id, jid;
  for( key_id=0; key_id < TEST_KEYFRAME_SIZE; key_id++ ){
    q_ptr[key_id] = zAlloc( double, TEST_JOINT_SIZE );
    for( jid=0; jid < rkChainJointSize(&g_chain); jid++ ){
      q_ptr[key_id][jid] = test_q[key_id][jid];
    }
    pin_ptr[key_id] = zAlloc( int, TEST_LINK_SIZE );
    for( link_id=0; link_id < rkChainLinkNum(&g_chain); link_id++ ){
      pin_ptr[key_id][link_id] = test_pin[key_id][link_id];
    }
  }
  /**/
  clone_and_set_keyframelist( TEST_KEYFRAME_SIZE, q_ptr, pin_ptr );
  /* free used double pointer */
  for( key_id=0; key_id < TEST_KEYFRAME_SIZE; key_id++ ){
    zFree(pin_ptr[key_id]);
    zFree(q_ptr[key_id]);
  }
  zFree(pin_ptr);
  zFree(q_ptr);
  /**/

  if( !init_p2p_array( &g_chain, &g_p2p_array, TEST_KEYFRAME_SIZE - 1, IK_PIN_CELL_SIZE ) )
    return -1.0;

  /**/
  clone_and_set_ref_path_input( TEST_KEYFRAME_SIZE-1, test_ref_path_input, &g_p2p_array );
  /**/

  if( run_test() < 0 ){
    ZRUNWARN( "Failed run_test()" );
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


