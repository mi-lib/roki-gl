#include <roki_gl/roki_glfw.h>

/* This is suggstion code for rkgl_chain.c **************************************/
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

int rkglChainCreatePhantomDisplay(rkChain* chain, double alpha, zOpticalInfo **oi_alt, rkglLight* light)
{
  int i, display_id;
  rkglChain display_gr;
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
  if( !rkglChainLoad( &display_gr, chain, &attr, light ) ){
    ZRUNWARN( "Failed rkglChainLoad(&display_gr)" );
    return -1;
  }
  display_id = rkglChainDrawOpticalAlt( &display_gr, alpha, &oi_alt[0], light);
  for( i=0; i < rkChainLinkNum( chain ); i++ ){
    if( oi_alt[i] ) zOpticalInfoDestroy( oi_alt[i] );
  }
  rkglChainUnload( &display_gr );

  return display_id;
}
/* end of suggstion code for rkgl_chain.c ***************************************/

GLFWwindow* g_window;

/* rkglLinkInfo2.pin information */
typedef enum{
  PIN_LOCK_OFF=-1,
  PIN_LOCK_6D,
  PIN_LOCK_POS3D
} pinStatus;

typedef struct{
  pinStatus pin;
  double w[2]; /* weight of pos & att */
  double dw[2];
  double d2w[2];
} pinInfo;

typedef struct{
  double s;
  double ds;
  double d2s;
} keyFeedRate;

typedef struct{
  zFrame3D root;
  zVec q;
  pinInfo *pinfo;
  int display_id;
  keyFeedRate feedrate;
} keyFrameInfo;
zArrayClass( keyFrameInfoArray, keyFrameInfo );

keyFrameInfoArray g_keyframe_array;

/* Test Dataset ********************************************************************/
#define TEST_KEYFRAME_SIZE 2
#define TEST_JOINT_SIZE 6
#define TEST_LINK_SIZE 7
#define TEST_FRAME_DOF_SIZE 6
/* x, y, z, aax, aay, aaz */
double test_root_frame[TEST_KEYFRAME_SIZE][TEST_FRAME_DOF_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { 0.0, 0.5, 0.0,  0.0, 0.0, 0.0 } };
/* q0 ... q5 */
double test_q[TEST_KEYFRAME_SIZE][TEST_JOINT_SIZE] =
  { { 0.0, 0.0, 0.0,  0.0, 0.0, 0.0 },
    { zDeg2Rad(45.0), zDeg2Rad(-60.0), zDeg2Rad(-5.0), 0.0, zDeg2Rad(90.0), 0.0 } };
/* pin */
int test_pin[TEST_KEYFRAME_SIZE][TEST_LINK_SIZE] =
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
typedef struct{
} rkglChainBlock;

/* p2p path */
typedef struct{
  zPexIP weight_path[2]; /* size = pos & att */
  bool is_pin_path;
  rkIKCell *cell[2];
  int cell_size; /* size of weight & cell = 2 (pos & att) */
} pathIKInfo;

zArrayClass( zPexIPArray, zPexIP );
typedef struct{
  pathIKInfo* ik_info; /* size = link size */
  int link_size;
  zPexIPArray qref_path; /* size = joint size */
  int joint_size;
} p2pPathInfo;
zArrayClass( p2pPathArray, p2pPathInfo );
p2pPathArray g_p2p_array;

zNURBS3D g_nurbs; /* size = specific pin num(=1 test) x specific segment num of keyframe-1 */
int g_selected_cp = -1;
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


#define NAME_NURBS 100
#define INTERMEDIATE_CP_NUM 2
#define SIZE_CP 10.0

int createPinInfoDisplayList(rkChain* chain, pinInfo pinfo[], double alpha, rkglLight* light)
{
  int i, display_id;
  zOpticalInfo **oi_alt;
  rkglChainAttr attr;

  rkglChainAttrInit( &attr );
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

  display_id = rkglChainCreatePhantomDisplay( chain, alpha, &oi_alt[0], light);

  for( i=0; i < rkChainLinkNum( chain ); i++ )
    if( oi_alt[i] ) zOpticalInfoDestroy( oi_alt[i] );

  return display_id;
}

/* define as user application */
bool rkglChainLoadKeyframeInfo(rkChain *chain)
{
  int keyframe_size, i, j;
  rkglChainAttr attr;

  printf("size of link = %d\n", rkChainLinkNum(chain) );
  printf("size of joint = %d\n", rkChainJointSize(chain) );

  if( zArraySize( &g_keyframe_array ) > 0 )
    zArrayFree( &g_keyframe_array );
  zArrayAlloc( &g_keyframe_array, keyFrameInfo, TEST_KEYFRAME_SIZE );
  keyframe_size = zArraySize( &g_keyframe_array ); /* = TEST_KEYFRAME_SIZE */

  rkglChainAttrInit( &attr );
  for( i=0; i < keyframe_size; i++ ){
    keyFrameInfo* kf = zArrayElemNC( &g_keyframe_array, i );
    if( !( kf->pinfo = zAlloc( pinInfo, rkChainLinkNum(chain) ) ) ){
      ZALLOCERROR();
      ZRUNERROR( "Failed to zAlloc( pinInfo, rkChainLinkNum(chain) )." );
      return false;
    }
    for( j=0; j < rkChainLinkNum(chain); j++ ){
      kf->pinfo[j].pin = test_pin[i][j];
      switch( kf->pinfo[j].pin ){
      case PIN_LOCK_6D:
        kf->pinfo[j].w[0] = IK_PIN_WEIGHT; /* init weight pos */
        kf->pinfo[j].w[1] = IK_PIN_WEIGHT; /* goal weight pos */
        break;
      case PIN_LOCK_POS3D:
        kf->pinfo[j].w[0] = IK_NO_WEIGHT;  /* init weight pos */
        kf->pinfo[j].w[1] = IK_PIN_WEIGHT; /* goal weight pos */
        break;
      case PIN_LOCK_OFF:
        kf->pinfo[j].w[0] = IK_NO_WEIGHT; /* init weight pos */
        kf->pinfo[j].w[1] = IK_NO_WEIGHT; /* goal weight pos */
        break;
      default: ;
      }
      kf->pinfo[j].dw[0]  = 0.0; /* default init weight vel */
      kf->pinfo[j].d2w[0] = 0.0; /* default init weight acc */
      kf->pinfo[j].dw[1]  = 0.0; /* default goal weight vel */
      kf->pinfo[j].d2w[1] = 0.0; /* default goal weight acc */
    }
    for( j=0; j < rkChainJointSize(chain); j++ ){
      printf( "test_q[%d][%d] = %f\n", i, j, test_q[i][j] );
    }
    printf( "\n" );
    if( !( kf->q = zVecCloneArray( test_q[i], rkChainJointSize(chain) ) ) ){
      ZRUNERROR( "Failed to zVecCloneArray( test_q[%d], rkChainJointSize(chain) ).", i );
      return false;
    };
    zFrame3DFromAA( &kf->root,
                    test_root_frame[i][0], test_root_frame[i][1],
                    test_root_frame[i][2], test_root_frame[i][3],
                    test_root_frame[i][4], test_root_frame[i][5] );

    /* rkChainSetRootFrame( chain, &kf->root ); */
    zFrame3DCopy( &kf->root, rkChainOrgFrame(chain) );
    rkChainFK( chain, kf->q );
    kf->display_id = createPinInfoDisplayList( chain, kf->pinfo, g_keyframe_alpha, &g_light );
    if( kf->display_id < 0 ){
      return false;
    }
  } /* end of for( zArraySize( &g_keyframe_array ) ) */

  return true;
}


void rkglChainUnloadKeyframeInfo()
{
  int i;
  for( i=0; i < zArraySize( &g_keyframe_array ); i++ ){
    keyFrameInfo* kf = zArrayElemNC( &g_keyframe_array, i );
    zVecFree( kf->q );
    zFree( kf->pinfo );
  }
  zArrayFree( &g_keyframe_array );
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


double accumulate_normalized_joint_norm(rkChain* chain, double s, zVec q1, zVec q2)
{
  int link_id, j, jid;
  double min[6], max[6], jsize, range, ret_s;
  zVec nq1, nq2;

  nq1 = zVecAlloc( zVecSize(q1) );
  nq2 = zVecAlloc( zVecSize(q2) );
  jid = 0.0;
  for( link_id=0; link_id < rkChainLinkNum(chain); link_id++ ){
    jsize = rkChainLinkJointSize(chain, link_id);
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
  int i, size;

  size = zArraySize( keyframe_array );
  zArrayElemNC(keyframe_array, 0)->feedrate.s = 0.0;
  for( i=0; i < size-1; i++ ){
    keyFrameInfo* kf1 = zArrayElemNC(keyframe_array, i);
    keyFrameInfo* kf2 = zArrayElemNC(keyframe_array, i+1);
    calc_p2p_feedrate( chain, kf1, kf2 );
  }
}

void init_qref_path(rkChain *chain, zPexIPArray* qref_path, int joint_size)
{
  if( zArraySize( qref_path ) > 0 )
    zArrayFree( qref_path );
  zArrayAlloc( qref_path, zPexIP, joint_size );
}

bool init_path_ik_info(rkChain *chain, pathIKInfo** path_ik_info, int link_size)
{
  if( *path_ik_info != NULL )
    zFree( *path_ik_info );
  if( !( *path_ik_info = zAlloc( pathIKInfo, link_size ) ) ){
    ZALLOCERROR();
    ZRUNERROR( "Failed to zAlloc( pathIKInfo, rkChainLinkNum(chain) )." );
    return false;
  }
  (*path_ik_info)->cell_size = 2;

  return true;
}

bool init_p2p_array(rkChain *chain, p2pPathArray *p2p_array, int array_size)
{
  int i;

  if( zArraySize( &g_p2p_array ) > 0 )
    zArrayFree( &g_p2p_array );
  zArrayAlloc( &g_p2p_array, p2pPathInfo, array_size );
  for( i=0; i < array_size; i++ ){
    p2pPathInfo* p2p_buf = zArrayElemNC( p2p_array, i );
    p2p_buf->joint_size = rkChainJointSize(chain);
    p2p_buf->link_size = rkChainLinkNum(chain);
    init_qref_path( chain, &p2p_buf->qref_path, p2p_buf->joint_size );
    if( !init_path_ik_info( chain, &p2p_buf->ik_info, p2p_buf->link_size ) ) return false;
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

void interpolate_p2p_ik_weight_path(rkChain* chain, pathIKInfo* path_ik_info, keyFrameInfo* kf1, keyFrameInfo* kf2)
{
  int link_id, i;
  double term = kf2->feedrate.s - kf1->feedrate.s;
  for( link_id=0; link_id < rkChainLinkNum(chain); link_id++ ){
    for( i=0; i < path_ik_info[link_id].cell_size; i++ ){
      zPexIPCreateBoundary( &path_ik_info[link_id].weight_path[i],
                            term,
                            kf1->pinfo[link_id].w[i], 0.0, 0.0, /* x1, v1, a1 */
                            kf2->pinfo[link_id].w[i], 0.0, 0.0, /* x2, v2, a2 */
                            NULL );
    } /* end of for loop i of weight pos & att num times */
  } /* end of for loop link_id of link num times */
}

/* 2 points, start keyframe index, rkChain*, zNURBS3D*, start keyFrameInfo end keyFrameInfo */
void interpolate_p2p_nurbs_cp(int kf1idx, rkChain *chain, zNURBS3D* nurbs, keyFrameInfo* kf1, keyFrameInfo* kf2)
{
  int j;

  /* start */
  zFrame3DCopy( &kf1->root, rkChainOrgFrame(chain) );
  rkChainFK( chain, kf1->q );
  zVec3D start;
  zVec3DCopy( rkChainLinkWldPos(chain, 6), &start ); /* 6 is test */
  zVec3DCopy( &start, zNURBS3D1CP(nurbs, kf1idx) );
  /* end */
  zFrame3DCopy( &kf2->root, rkChainOrgFrame(chain) );
  rkChainFK( chain, kf2->q );
  zVec3D end;
  zVec3DCopy( rkChainLinkWldPos(chain, 6), &end );
  zVec3DCopy( &end, zNURBS3D1CP(nurbs, kf1idx + INTERMEDIATE_CP_NUM + 1 ) );
  /* itermediate points */
  for( j=kf1idx+1; j<zNURBS3D1CPNum(nurbs)-INTERMEDIATE_CP_NUM; j++ ){
    zVec3DCreate( zNURBS3D1CP(nurbs, j),
                  zRandF(start.c.x - 1.0, start.c.x + 1.0),
                  zRandF(start.c.x - 1.0, start.c.x + 1.0),
                  zRandF(start.c.x - 1.0, start.c.x + 1.0) );
    zVec3DCreate( zNURBS3D1CP(nurbs, j + INTERMEDIATE_CP_NUM - 1),
                  zRandF(end.c.x - 1.0, end.c.x + 1.0),
                  zRandF(end.c.x - 1.0, end.c.x + 1.0),
                  zRandF(end.c.x - 1.0, end.c.x + 1.0) );
  }
}


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

void print_header_label_of_ik_weight_path(FILE *fp, int link_size, int cell_size)
{
  int link_id, i;

  for( link_id=0; link_id < link_size; link_id++ )
    for( i=0; i<cell_size; i++)
      fprintf( fp, "weight[%d][%d] ", i, link_id);
}

void print_interpolated_qref_path(FILE *fp, double s, zPexIPArray* qref_path)
{
  int jid;

  for( jid=0; jid < zArraySize( qref_path ); jid++ )
    fprintf( fp, "%.10f ", zPexIPVal( &qref_path->buf[jid], s ) );
}

void print_interpolated_ik_weight_path(FILE *fp, double s, pathIKInfo *path_ik_info, int link_size)
{
  int link_id, i;

  for( link_id=0; link_id < link_size; link_id++ )
    for( i=0; i<path_ik_info[link_id].cell_size; i++)
      fprintf( fp, "%.10f ", zPexIPVal( &path_ik_info[link_id].weight_path[i], s ) );
}

void print_interpolated_path(FILE *fp, rkChain* chain, keyFrameInfoArray* keyframe_array, p2pPathArray *p2p_array, int s_slice_num)
{
  int i, keyframe_size, joint_size, link_size, cell_size, path_size, sid;
  keyFrameInfo* end_kf;
  double s, end_s;

  keyframe_size = zArraySize( keyframe_array );
  end_kf = zArrayElemNC(keyframe_array, keyframe_size-1);
  end_s = end_kf->feedrate.s;
  joint_size = zArraySize( &p2p_array->buf[0].qref_path );
  link_size = p2p_array->buf[0].link_size;
  cell_size = p2p_array->buf[0].ik_info[0].cell_size;
  path_size = zArraySize( p2p_array );

  print_header_label_feedrate( fp, end_s );
  print_header_label_of_qref_path( fp, joint_size );
  print_header_label_of_ik_weight_path( fp, link_size, cell_size );
  fprintf( fp, "\n" );

  for( i=0; i < path_size; i++){
    for( sid=0; sid<=(int)(s_slice_num); sid++ ){
      s = (double)(sid) * end_s / s_slice_num;
      fprintf( fp, "%d %.10f ", sid, s );
      p2pPathInfo* p2p_buf = &p2p_array->buf[i];
      print_interpolated_qref_path( fp, s, &p2p_buf->qref_path );
      print_interpolated_ik_weight_path( fp, s, p2p_buf->ik_info, link_size );
      fprintf( fp, "\n" );
    }
  }
}


bool interpolate_path(rkChain* chain, keyFrameInfoArray* keyframe_array, p2pPathArray* p2p_array, zNURBS3D* nurbs)
{
  int i, size;
  zOpticalInfo oi;

  size = zArraySize( keyframe_array );
  if( size < 2 ) {
    ZRUNERROR( "too small keyframe_array size = %d (must be >= 2)", size );
    return false;
  }

  /* the number of intermediate control points */
  zNURBS3D1Alloc( nurbs, size + INTERMEDIATE_CP_NUM, 3 );
  zNURBS3D1SetSliceNum( nurbs, SLICE_NUM );

  for( i=0; i < size-1; i++ ){
    keyFrameInfo* kf1 = zArrayElemNC(keyframe_array, i);
    keyFrameInfo* kf2 = zArrayElemNC(keyframe_array, i+1);
    p2pPathInfo* p2p_buf = &p2p_array->buf[i];
    interpolate_p2p_qref_path( chain, &p2p_buf->qref_path, kf1, kf2 );
    interpolate_p2p_ik_weight_path( chain, p2p_buf->ik_info, kf1, kf2 );
    interpolate_p2p_nurbs_cp( i, chain, nurbs, kf1, kf2 );
  } /* end of for loop i of keyframe num times */

  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );

  return true;
}

void release_path()
{
  int i, link_id, j, jid;

  for( i=0; i < zArraySize(&g_p2p_array); i++ ){
    p2pPathInfo* p2p_buf = zArrayElemNC( &g_p2p_array, i );
    /* release weight_path[2] */
    for( link_id=0; link_id < p2p_buf->link_size; link_id++ ){
      pathIKInfo* ik_info = &p2p_buf->ik_info[link_id];
      for( j=0; j < ik_info->cell_size; j++ )
        zPexIPFree( &ik_info->weight_path[j] );
    }
    /* release qref_path[] */
    for( jid=0; jid < p2p_buf->joint_size; jid++ )
      zPexIPFree( &p2p_buf->qref_path.buf[jid] );
    if( zArraySize( &p2p_buf->qref_path ) > 0 )
      zArrayFree( &p2p_buf->qref_path );
  }
  if( zArraySize( &g_p2p_array ) > 0 )
    zArrayFree( &g_p2p_array );
  if( g_nurbs.knot != NULL )
    zNURBS3DDestroy( &g_nurbs );

}

void run_test(void)
{
  /* interpolate path of weight & qref_& specific pin(NURBS) */
  interpolate_path( &g_chain, &g_keyframe_array, &g_p2p_array, &g_nurbs );

  FILE *fp;
  /* fp = stdout; */
  fp = fopen( "motion_path_view.dat", "w" );
  print_interpolated_path( fp, &g_chain, &g_keyframe_array, &g_p2p_array, SLICE_NUM);
  fclose( fp );
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

  calc_feedrate( &g_chain, &g_keyframe_array );
  if( !init_p2p_array( &g_chain, &g_p2p_array, zArraySize( &g_keyframe_array ) - 1 ) )
    return false;

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
  run_test();

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


