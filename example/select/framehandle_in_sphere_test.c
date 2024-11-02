/* with a courtesy to Mr. Daishi Kaneta. */
#include <roki_gl/roki_glut.h>

/* zRotIp defenition */

ZDEF_STRUCT( __ZEO_CLASS_EXPORT, zRotIpCPCell ){
  zMat3D cp; /*!< control point */
};

zArrayClass( zRotIpCPArray, zRotIpCPCell );

ZDEF_STRUCT( __ZEO_CLASS_EXPORT, zRotIp ){
  int order;     /*!< \brief orders of a curve */
  zVec knot;     /*!< \brief knot vectors */
  int ns;        /*!< \brief number of slices in sphere angle axis */
  /*! \cond */
  zRotIpCPArray cparray; /* an array of control points */
  /*! \endcond */
};

#define zRotIpKnotNum(r)        zVecSizeNC((r)->knot)
#define zRotIpKnot(r,i)         zVecElemNC((r)->knot,i)
#define zRotIpSetKnot(r,i,v)    ( zRotIpKnot(r,i) = (v) )
#define zRotIpKnotS(r)          zRotIpKnot(r,0)
#define zRotIpKnotE(r)          zRotIpKnot(r,zRotIpCPNum(r)-1)
#define zRotIpKnotOneSlice(r)   ( ( zRotIpKnotE(r) - zRotIpKnotS(r) ) / (r)->ns )
#define zRotIpKnotSlice(r,i)    ( zRotIpKnotOneSlice(r) * i + zRotIpKnotS(r) )
#define zRotIpSliceNum(r)       (r)->ns
/*! \brief set numbers of slices of a NURBS curve / surface. */
#define zRotIpSetSliceNum(r,snum) do{ \
  (r)->ns = (snum);\
} while(0)

#define zRotIpCPNum(r)       zArraySize(&(r)->cparray)
#define zRotIpCP(r,i)        ( &zArrayElemNC(&(r)->cparray,i)->cp )
#define zRotIpSetCP(r,i,v)   zMat3DCopy( v, zRotIpCP(r,i) )

/* allocate a zRotIp curve. */
bool zRotIpAlloc(zRotIp *rotip, int size)
{
  int i;
  rotip->knot = zVecAlloc( size );
  zArrayAlloc( &rotip->cparray, zRotIpCPCell, size );
  i=0; zRotIpSetKnot( rotip, i, 0 );
  for( i=0; i<= ( size - 1 ); i++ )
    zRotIpSetKnot( rotip, i, (double)(i)/(double)( size - 1 ) );

  return true;
}

/* initialize a Rotation curve. */
zRotIp *zRotIpInit(zRotIp *rotip)
{
  zRotIpSetSliceNum( rotip, 0 );
  rotip->knot = NULL;
  zArrayInit( &rotip->cparray );
  return rotip;
}

/* destroy a Rotation curve. */
void zRotIpDestroy(zRotIp *rotip)
{
  zVecFree( rotip->knot );
  zArrayFree( &rotip->cparray );
  zRotIpInit( rotip );
}

/* find a knot segment that includes the given parameter. */
static int _zRotIpSeg(zRotIp *rotip, double t)
{
  int i, j, k;

  for( i=0, j=zRotIpCPNum(rotip)-1; ; ){
    if( j <= i + 1 ) break;
    k = ( i + j ) / 2;
    if( zRotIpKnot(rotip,k) > t )
      j = k;
    else
      i = k;
  }
  return i;
}

zMat3D* zRotIpPopMat3D(zRotIp *rotip, double s, zMat3D* out_mat)
{
  double s0, s1, t;
  zMat3D *m0, *m1;
  zEP ep0, ep1, ep;
  int seg_idx;
  seg_idx = _zRotIpSeg( rotip, s );
  s0 = zRotIpKnot( rotip, seg_idx );
  s1 = zRotIpKnot( rotip, seg_idx+1 );
  /* Slerp */
  t  = (s - s0) / (s1 - s0);
  m0 = zRotIpCP( rotip, seg_idx );
  m1 = zRotIpCP( rotip, seg_idx+1 );
  zMat3DToEP( m0, &ep0 );
  zMat3DToEP( m1, &ep1 );
  zEPInterDiv( &ep0, &ep1, t, &ep );
  zMat3DFromEP( out_mat, &ep );

  return out_mat;
}

/* end of zRotIp definition */


/* data defenition */
typedef struct{
  zShape3D shape;
  int display_id[2]; /* wire off/on */
} rkglShapeSphereData;

typedef struct{
  zRotIp rotip;
  zVec3D start_draw_unitvec;
  int display_id;
} sphereXYZ;

typedef struct{
  rkglFrameHandle fh;
  double feedrate_s;
  rkglShapeSphereData sphere;
  sphereXYZ rot_curve;
  int selected_rot_cp;
  zVec3D selected_rot_cp_anchor_pos;
  zMat3D selected_rot_cp_anchor_att;
  rkglSelectionBuffer sb;
} motionPathViewData;

motionPathViewData* g_main;

static const int g_SLICE_NUM = 100;
static const int g_CP_NUM = 3;

static ubyte g_dispswitch = 1;

/* viewing parameters */
rkglCamera g_cam;
rkglLight g_light;
static const GLdouble g_znear = -100.0;
static const GLdouble g_zfar  = 100.0;
static double g_scale = 0.01;

/* FrameHandle shape property */
static const double g_LENGTH = 2.0;
static const double g_MAGNITUDE = 1.0;
static const double g_CP_SIZE = 10.0;

#define NAME_ROTIP 1000
#define SHAPE_SPHERE_ALPHA 0.3

/* end of data defenition */


/* rotation curve drawing */

zArray2Class( zMesh3D, zVec3D ); /* imitatational definition from rkgl_hape.c */
#define zMesh3DAlloc(mesh,row,col) zArray2Alloc( mesh, zVec3D, row, col )

zVec3D* getDrawPoint(zMat3D* mat, zVec3D* start_draw_unitvec, zVec3D* center, double radius, zVec3D* out_drawp)
{
  zVec3D rotated_unitvec;
  zMulMat3DVec3D( mat, start_draw_unitvec, &rotated_unitvec );
  zVec3DMul( &rotated_unitvec, radius, out_drawp );
  zVec3DAddDRC( out_drawp, center );

  return out_drawp;
}

void rkglRotationCurve(zRotIp *rotip, zVec3D* start_draw_unitvec, zVec3D *center, double radius, zRGB *rgb)
{
  int i;
  zMat3D mat;
  zVec3D vert;
  double u;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  rkglRGB( rgb );
  glPushName( -1 );
  glBegin( GL_LINE_STRIP );
  for( i=0; i<=zRotIpSliceNum(rotip); i++ ){
    u = zRotIpKnotSlice( rotip, i );
    zRotIpPopMat3D( rotip, u, &mat );
    getDrawPoint( &mat, start_draw_unitvec, center, radius, &vert );
    rkglVertex( &vert );
  }
  glEnd();
  glPopName();
  rkglLoadLighting( lighting_is_enabled );
}

void rkglRotationCurveCP(zRotIp *rotip, zVec3D* start_draw_unitvec, zVec3D* center, double radius, GLfloat size, zRGB *rgb)
{
  int i;
  bool lighting_is_enabled;
  zVec3D cp;

  glPointSize( size );
  rkglSaveLighting( &lighting_is_enabled );
  rkglRGB( rgb );
  glPushName( 0 );
  for( i=0; i<zRotIpCPNum(rotip); i++ ){
    glLoadName( i );
    glBegin( GL_POINTS );
    getDrawPoint( zRotIpCP(rotip,i), start_draw_unitvec, center, radius, &cp );
    rkglVertex( &cp );
    glEnd();
  }
  glPopName();
  glPushName( -1 );
  for( i=0; i<zRotIpCPNum(rotip); i++ ){
    if( i > 0 ){
      glBegin(GL_LINES);
      getDrawPoint( zRotIpCP(rotip,i), start_draw_unitvec, center, radius, &cp );
      rkglVertex( &cp );
      getDrawPoint( zRotIpCP(rotip,i-1), start_draw_unitvec, center, radius, &cp );
      rkglVertex( &cp );
      glEnd();
    }
  }
  glPopName();
  rkglLoadLighting( lighting_is_enabled );
}

/* end of rotation curve drawing */

/* data handling */

bool create_empty_motionPathViewData(void** src)
{
  motionPathViewData** main_ptr = (motionPathViewData**)( src );
  if( !( *main_ptr = zAlloc( motionPathViewData, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  (*main_ptr)->selected_rot_cp = -1;

  return true;
}

void set_motionPathViewData(void* src)
{
  g_main = (motionPathViewData*)( src );
}

void* get_motionPathViewData(void)
{
  return (void *)( g_main );
}

void destroy_motionPathViewData(void* src)
{
  motionPathViewData* main_ptr = (motionPathViewData*)( src );
  zFree( main_ptr );
}

/* end of data handling */

void update_framehandle_att(double s)
{
  zMat3D mat;
  g_main->feedrate_s = s;
  double start_s = zRotIpKnotSlice( &g_main->rot_curve.rotip, 0 );
  double end_s = zRotIpKnotSlice( &g_main->rot_curve.rotip, zRotIpSliceNum(&g_main->rot_curve.rotip) );
  if( g_main->feedrate_s < start_s ) g_main->feedrate_s = start_s;
  if( g_main->feedrate_s > end_s ) g_main->feedrate_s = end_s;
  zRotIpPopMat3D( &g_main->rot_curve.rotip, s, &mat );
  zMat3DCopy( &mat, rkglFrameHandleAtt( &g_main->fh ) );
}

/* draw part */

void draw_framehandle(void)
{
  rkglFrameHandleDraw( &g_main->fh );
}

void draw_sphere(void)
{
  glPushMatrix();
  rkglTranslate( rkglFrameHandlePos( &g_main->fh ) );
  glPushName( 0 );
  g_dispswitch ? glCallList( g_main->sphere.display_id[0] ) : glCallList( g_main->sphere.display_id[1] );
  glPopName();
  glPopMatrix();
}

void draw_frame_rot_curve(void)
{
  zRGB rgb;
  glPushMatrix();
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  glLoadName( NAME_ROTIP );
  glLineWidth( 3 );
  rkglRotationCurve( &g_main->rot_curve.rotip, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, &rgb);
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  glLineWidth( 1 );
  rkglRotationCurveCP(&g_main->rot_curve.rotip, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, g_CP_SIZE, &rgb);
  glPopMatrix();
}

void draw_scene(void)
{
  draw_framehandle();
  draw_sphere();
  draw_frame_rot_curve();
}

void display(void)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
  glutSwapBuffers();
}

int find_rot_cp(rkglSelectionBuffer *sb)
{
  int i, selected_rotip_id_tmp, selected_rot_cp_tmp;

  g_main->selected_rot_cp = -1;
  for( i=0; i<sb->hits; i++ ){
    selected_rotip_id_tmp = rkglSelectionName(sb,0) - NAME_ROTIP;
    if( selected_rotip_id_tmp == 0 ){
      zRotIp* rotip = &g_main->rot_curve.rotip;
      selected_rot_cp_tmp = rkglSelectionName(sb,1);
      if( selected_rot_cp_tmp >= 0 &&
          selected_rot_cp_tmp < zRotIpCPNum(rotip) ){
        g_main->selected_rot_cp = selected_rot_cp_tmp;
        break;
      }
      rkglSelectionNext( sb );
    }
    eprintf(" sp->hits=%d : i=%d : rotip_id_tmp=%d, rkglSeleionName(sb,0)=%d, Name(sb,1)=%d\n", sb->hits, i, selected_rotip_id_tmp, rkglSelectionName(sb,0), rkglSelectionName(sb,1));
  }

  return g_main->selected_rot_cp;
}

/* end of draw part */

/* callback event */

void mouse(int button, int state, int x, int y)
{
  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      rkglClear();
      rkglSelect( &g_main->sb, &g_cam, draw_frame_rot_curve, x, y, g_CP_SIZE, g_CP_SIZE );
      if( find_rot_cp( &g_main->sb ) >= 0 ){
        zMat3DCopy( zRotIpCP( &g_main->rot_curve.rotip, g_main->selected_rot_cp ), &g_main->selected_rot_cp_anchor_att );
        /* rkglUnproject( &g_cam, x, y, rkglSelectionZnearDepth(&g_main->sb), &g_main->selected_rot_cp_anchor_pos ); */
        getDrawPoint( &g_main->selected_rot_cp_anchor_att, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, &g_main->selected_rot_cp_anchor_pos );
        eprintf( "Selected rotation control point [%d]\n", g_main->selected_rot_cp );
        break;
      }
      if( g_main->selected_rot_cp < 0 &&
          rkglSelectNearest( &g_main->sb, &g_cam, draw_framehandle, x, y, 1, 1 ) ){
        rkglFrameHandleAnchor( &g_main->fh, &g_main->sb, &g_cam, x, y );
      }
    } else
    if( state == GLUT_UP ){
      rkglFrameHandleUnselect( &g_main->fh );
      g_main->sb.hits = 0;
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
  if( g_main->selected_rot_cp < 0 &&
      rkglFrameHandleIsUnselected( &g_main->fh ) )
    rkglMouseFuncGLUT( button, state, x, y );
}

void motion(int x, int y)
{
  zVec3D destp, axis, r0, dr, *center, aa;
  zVec3D dur, r0prod2dr;
  double dis_dr, angle0, angle1, angle;
  double r0_cos, dis_r0, dis_r0prod2dr;
  zMat3D dest_rot;

  zRotIp* rotip = &g_main->rot_curve.rotip;
  if( g_main->selected_rot_cp >= 0 && g_main->selected_rot_cp < zRotIpCPNum( rotip ) ){
    /* cp dragging control */
    center = rkglFrameHandlePos( &g_main->fh );
    zVec3DSub( &g_main->selected_rot_cp_anchor_pos, center, &r0 );
    dis_r0 = zVec3DNorm( &r0 );
    rkglUnproject( &g_cam, x, y, rkglSelectionZnearDepth(&g_main->sb), &destp );
    zVec3DSub( &destp, &g_main->selected_rot_cp_anchor_pos, &dr );
    dis_dr = zVec3DNorm( &dr );
    if( !zIsTiny( dis_dr ) ){
      zVec3DNormalizeNC( &dr, &dur );
      r0_cos = zVec3DInnerProd( &r0, &dur );
      zVec3DMul( &dur, r0_cos, &r0prod2dr );
      dis_r0prod2dr = zVec3DNorm( &r0prod2dr );
      angle0 = acos( r0_cos / dis_r0 );
      if( angle0 > 0.5*zPI ) angle0 = 0.5*zPI;
      if( r0_cos >= 0 ){
        if( dis_r0 - dis_r0prod2dr - dis_dr > zTOL ){
          angle1 = acos( ( dis_r0prod2dr + dis_dr ) / dis_r0 );
          angle = angle0 - angle1;
        } else
          angle = angle0;
      } else{
        if( dis_r0 - ( dis_dr - dis_r0prod2dr ) > zTOL ){
          angle1 = acos( fabs( dis_dr - dis_r0prod2dr ) / dis_r0 );
          if( dis_dr >= dis_r0prod2dr ){
            angle = zPI - angle0 - angle1;
          } else
            angle = angle1 - angle0;
        } else
          angle = zPI - angle0;
      }
      zVec3DOuterProd( &r0, &destp, &axis );
      zVec3DNormalizeNCDRC( &axis );
      zVec3DMul( &axis, angle, &aa );
      zMat3DRot( &g_main->selected_rot_cp_anchor_att, &aa, &dest_rot );
      zRotIpSetCP( rotip, g_main->selected_rot_cp, &dest_rot );
      update_framehandle_att( g_main->feedrate_s );
    }
  } else
  if( !rkglFrameHandleIsUnselected( &g_main->fh ) ){
    rkglFrameHandleMove( &g_main->fh, &g_cam, x, y );
  } else
    rkglMouseDragFuncGLUT( x, y );
}

void resize(int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar );
}

void keyboard(unsigned char key, int x, int y)
{
  double ds = zRotIpKnotOneSlice( &g_main->rot_curve.rotip );
  switch( key ){
  case 'u': rkglCALockonPTR( &g_cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &g_cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &g_cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &g_cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &g_cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &g_cam, 0, 0,-5 ); break;
  case '8': g_scale += 0.001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '*': g_scale -= 0.001; rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar ); break;
  case '9': rkglCARelMove( &g_cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &g_cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &g_cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &g_cam, 0, 0,-0.05 ); break;
  case 'f': update_framehandle_att( g_main->feedrate_s + ds); break; /* forward */
  case 'b': update_framehandle_att( g_main->feedrate_s - ds); break; /* backward */
  case 'w': g_dispswitch = 1 - g_dispswitch; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

/* end of callback event */

void init(void)
{
  rkglSetDefaultCallbackParam( &g_cam, 0, 0, 0, 0, 0 );
  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 5, 0, 2, 0, -20, 0 );
  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.4, 0.4, 0.4, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &g_light, 8, 0, 8 );
  /* frame handle */
  rkglFrameHandleCreate( &g_main->fh, 0, g_LENGTH, g_MAGNITUDE );
  /* sphere */
  int div = 8;
  zShape3DSphereCreate( &g_main->sphere.shape, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, div );
  double r=1.0, g=1.0, b=1.0;
  zOpticalInfo oi_alt;
  zOpticalInfoCreate( &oi_alt, r, g, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SHAPE_SPHERE_ALPHA, NULL );
  g_main->sphere.display_id[0] = rkglEntryShape( &g_main->sphere.shape, &oi_alt, RKGL_FACE, &g_light );
  g_main->sphere.display_id[1] = rkglEntryShape( &g_main->sphere.shape, &oi_alt, (RKGL_FACE | RKGL_WIREFRAME), &g_light );
  /**/
  zVec3DCreate( &g_main->rot_curve.start_draw_unitvec, 1.0, 0.0, 0.0 );
  zRotIpInit( &g_main->rot_curve.rotip );
  zRotIpAlloc( &g_main->rot_curve.rotip, g_CP_NUM );
  zRotIpSetSliceNum( &g_main->rot_curve.rotip, g_SLICE_NUM );
  /**/
  g_main->feedrate_s = 0.0;
  /**/
  zMat3D m0, m1, m2;
  zMat3DFromZYX( &m0, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zMat3DFromZYX( &m1, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zMat3DFromZYX( &m2, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zRotIpSetCP( &g_main->rot_curve.rotip, 0, &m0 );
  zRotIpSetCP( &g_main->rot_curve.rotip, 1, &m1 );
  zRotIpSetCP( &g_main->rot_curve.rotip, 2, &m2 );
}

int main(int argc, char *argv[])
{
  void* main_ptr = NULL;
  create_empty_motionPathViewData( &main_ptr );
  set_motionPathViewData( main_ptr );

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutMotionFunc( motion );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();

  destroy_motionPathViewData( main_ptr );

  return 0;
}
