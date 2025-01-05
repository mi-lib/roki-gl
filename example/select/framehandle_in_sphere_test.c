#include <roki_gl/roki_glfw.h>

/* zRotIp defenition */

ZDEF_STRUCT( __ZEO_CLASS_EXPORT, zRotIpCPCell ){
  zMat3D cp; /*!< control point */
  double w;  /*!< weight */
};

zArrayClass( zRotIpCPArray, zRotIpCPCell );

ZDEF_STRUCT( __ZEO_CLASS_EXPORT, zRotIp ){
  zBSplineParam param; /*!< \brief B-spline parameter */
  /*! \cond */
  zRotIpCPArray cparray; /* an array of control points */
  /*! \endcond */
};

#define zRotIpOrder(r)          (r)->param.order
#define zRotIpKnotNum(r)        zBSplineParamKnotNum( &(r)->param )
#define zRotIpKnot(r,i)         zBSplineParamKnot( &(r)->param, i )
#define zRotIpSetKnot(r,v)      zBSplineParamSetKnot( &(r)->param, v )
#define zRotIpKnotS(r)          zBSplineParamKnotS( &(r)->param )
#define zRotIpKnotE(r)          zBSplineParamKnotE( &(r)->param )
#define zRotIpKnotS(r)          zBSplineParamKnotS( &(r)->param )
#define zRotIpKnotE(r)          zBSplineParamKnotE( &(r)->param )
#define zRotIpKnotOneSlice(r)   ( ( zRotIpKnotE(r) - zRotIpKnotS(r) ) / (r)->param.slice )
#define zRotIpKnotSlice(r,k)    zBSplineParamKnotSlice( &(r)->param, k )
#define zRotIpSlice(r)          (r)->param.slice
/*! \brief set numbers of slices of a curve. */
#define zRotIpSetSlice(r,snum) do{\
  zBSplineParamSetSlice( &(r)->param, snum );\
} while(0)

#define zRotIpCPNum(r)         zArraySize(&(r)->cparray)
#define zRotIpWeight(r,i)      ( zArrayElemNC(&(r)->cparray,i)->w )
#define zRotIpSetWeight(r,i,v) ( zRotIpWeight(r,i) = (v) )
#define zRotIpCP(r,i)          ( &zArrayElemNC(&(r)->cparray,i)->cp )
#define zRotIpSetCP(r,i,v)     zMat3DCopy( v, zRotIpCP(r,i) )

/* allocate a zRotIp curve. */
bool zRotIpAlloc(zRotIp *rotip, int order, int size)
{
  int i;
  if( order > 0 )
    zBSplineParamAlloc( &rotip->param, order, size, 0 ); /* NURBS */
  else
    zBSplineParamAlloc( &rotip->param, 0, size-1, 0 ); /* Slerp */
  zArrayAlloc( &rotip->cparray, zRotIpCPCell, size );
  zBSplineParamKnotInit( &rotip->param );
  for( i=0; i<size; i++ ){
    zRotIpSetWeight( rotip, i, 1.0 );
    zMat3DZero( zRotIpCP( rotip, i ) );
  }
  return true;
}

/* initialize a Rotation curve. */
zRotIp *zRotIpInit(zRotIp *rotip)
{
  zBSplineParamInit( &rotip->param );
  zArrayInit( &rotip->cparray );
  return rotip;
}

/* destroy a Rotation curve. */
void zRotIpDestroy(zRotIp *rotip)
{
  zBSplineParamFree( &rotip->param );
  zArrayFree( &rotip->cparray );
  zRotIpInit( rotip );
}

/* compute a 3D matrix on a NURBS curve / surface. */
zMat3D *zRotIpNURBS3D(const zRotIp *rotip, double s, zMat3D *out_mat)
{
  int seg_idx, i0, i;
  double bs, den;
  zMat3D *m0, tmp;
  zVec3D omega;

  seg_idx = zBSplineParamSeg( &rotip->param, s );
  for( den=0, i=0; i<zRotIpCPNum(rotip); i++ ){
    den += zRotIpWeight(rotip,i) * zBSplineParamBasis( &rotip->param, s, i, zRotIpOrder(rotip), seg_idx );
  }
  i0 = seg_idx - zRotIpOrder(rotip);
  zMat3DCopy( zRotIpCP( rotip, i0 ), out_mat );
  for( i=i0; i<=seg_idx; i++ ){
    m0 = zRotIpCP( rotip, i );
    bs = zRotIpWeight(rotip,i) * zBSplineParamBasis( &rotip->param, s, i, zRotIpOrder(rotip), seg_idx ) / den;
    /* zMat3DInterDiv( out_mat, m0, bs, &tmp ); */
    zMat3DError( m0, out_mat, &omega );
    zMat3DRotCat( out_mat, &omega, bs, &tmp );
    zMat3DCopy( &tmp, out_mat );
  }

  return zIsTiny(den) ? NULL : out_mat;
}


zMat3D* zRotIpPopMat3D(const zRotIp *rotip, double s, zMat3D* out_mat)
{
  if( zRotIpOrder(rotip) > 0 )
    return zRotIpNURBS3D( rotip, s, out_mat );

  /* Slerp */
  double s0, s1, t;
  zMat3D *m0, *m1;
  zEP ep0, ep1, ep;
  int seg_idx;
  seg_idx = zBSplineParamSeg( &rotip->param, s );
  s0 = zRotIpKnot( rotip, seg_idx );
  s1 = zRotIpKnot( rotip, seg_idx+1 );
  /**/
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
  zRotIp rotip_slerp;
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
static const int g_ORDER = 3;
static const int g_CP_NUM = 4;
static const double g_WEIGHT = 1.0;

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
  for( i=0; i<=zRotIpSlice(rotip); i++ ){
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
  glEnd();
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
  double end_s = zRotIpKnotSlice( &g_main->rot_curve.rotip, zRotIpSlice(&g_main->rot_curve.rotip) );
  if( g_main->feedrate_s < start_s ) g_main->feedrate_s = start_s;
  if( g_main->feedrate_s > end_s ) g_main->feedrate_s = end_s;
  zRotIpPopMat3D( &g_main->rot_curve.rotip, s, &mat );
  printf("s=%f : pop mat=", s); zMat3DPrint( &mat );
  zMat3DCopy( &mat, rkglFrameHandleAtt( &g_main->fh ) );
}


/* draw part */

bool g_is_exit = false;

bool is_exit(void)
{
  return g_is_exit;
}

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
  rkglRotationCurve( &g_main->rot_curve.rotip_slerp, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, &rgb);
  rkglRotationCurveCP(&g_main->rot_curve.rotip, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, g_CP_SIZE, &rgb);
  glPopMatrix();
}

void draw_scene(void)
{
  draw_framehandle();
  draw_sphere();
  draw_frame_rot_curve();
}

void display(GLFWwindow* window)
{
  rkglCALoad( &g_cam );
  rkglLightPut( &g_light );
  rkglClear();
  draw_scene();
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

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  rkglMouseFuncGLFW( window, button, state, mods );

  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      rkglSelect( &g_main->sb, &g_cam, draw_frame_rot_curve, rkgl_mouse_x, rkgl_mouse_y, g_CP_SIZE, g_CP_SIZE );
      if( find_rot_cp( &g_main->sb ) >= 0 ){
        rkglFrameHandleUnselect( &g_main->fh );
        zMat3DCopy( zRotIpCP( &g_main->rot_curve.rotip, g_main->selected_rot_cp ), &g_main->selected_rot_cp_anchor_att );
        getDrawPoint( &g_main->selected_rot_cp_anchor_att, &g_main->rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_main->fh ), g_LENGTH, &g_main->selected_rot_cp_anchor_pos );
        eprintf( "Selected rotation control point [%d]\n", g_main->selected_rot_cp );
      } else
      if( g_main->selected_rot_cp < 0 &&
          rkglSelectNearest( &g_main->sb, &g_cam, draw_framehandle, rkgl_mouse_x, rkgl_mouse_y, 1, 1 ) ){
        rkglFrameHandleAnchor( &g_main->fh, &g_main->sb, &g_cam, rkgl_mouse_x, rkgl_mouse_y );
      }
    } else
    if( state == GLFW_RELEASE ){
      g_main->sb.hits = 0;
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
    }
  }
}

void motion(GLFWwindow* window, double x, double y)
{
  zVec3D destp, axis, r0, dr, *center, aa;
  zVec3D dur, r0prod2dr;
  double dis_dr, angle0, angle1, angle;
  double r0_cos, dis_r0, dis_r0prod2dr;
  zMat3D dest_rot;

  zRotIp* rotip_slerp = &g_main->rot_curve.rotip_slerp;
  zRotIp* rotip = &g_main->rot_curve.rotip;
  if( g_main->sb.hits == 0 ){
    rkglMouseDragFuncGLFW( window, x, y );
    return;
  }
  if( rkgl_mouse_button == GLFW_MOUSE_BUTTON_LEFT ){
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
        zRotIpSetCP( rotip_slerp, g_main->selected_rot_cp, &dest_rot );
        zRotIpSetCP( rotip,       g_main->selected_rot_cp, &dest_rot );
        update_framehandle_att( g_main->feedrate_s );
      }
    } else
    if( !rkglFrameHandleIsUnselected( &g_main->fh ) ){
      rkglFrameHandleMove( &g_main->fh, &g_cam, x, y );
    }
  }

  rkglMouseStoreXY( floor(x), floor(y) );
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
    g_is_exit = true;
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
  zRotIpInit( &g_main->rot_curve.rotip_slerp );
  zRotIpAlloc( &g_main->rot_curve.rotip_slerp, -1,      g_CP_NUM ); /* Slerp */
  zRotIpAlloc( &g_main->rot_curve.rotip,       g_ORDER, g_CP_NUM ); /* NURBS */
  zRotIpSetSlice( &g_main->rot_curve.rotip_slerp, g_SLICE_NUM );
  zRotIpSetSlice( &g_main->rot_curve.rotip,       g_SLICE_NUM );
  /**/
  g_main->feedrate_s = 0.0;
  /**/
  int i;
  zMat3D m;
  for( i=0; i < g_CP_NUM; i++ ){
    zMat3DFromZYX( &m, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
    zRotIpSetCP( &g_main->rot_curve.rotip,       i, &m );
    zRotIpSetCP( &g_main->rot_curve.rotip_slerp, i, &m );
    zRotIpSetWeight( &g_main->rot_curve.rotip,       i, g_WEIGHT );
    zRotIpSetWeight( &g_main->rot_curve.rotip_slerp, i, g_WEIGHT );
  }
}


GLFWwindow* g_window;

int main(int argc, char *argv[])
{
  void* main_ptr = NULL;
  create_empty_motionPathViewData( &main_ptr );
  set_motionPathViewData( main_ptr );

  int width;
  int height;
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

  init();

  resize( g_window, width, height );
  glfwSwapInterval(1);

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE &&
          !is_exit() ){
    display(g_window);
    glfwPollEvents();
    glfwSwapBuffers( g_window );
  }

  destroy_motionPathViewData( main_ptr );
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
