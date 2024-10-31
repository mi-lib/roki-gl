/* with a courtesy to Mr. Daishi Kaneta. */
#include <roki_gl/roki_glut.h>

/* zMat3D(absolute) -> point of the end of XYZ frame array on sphere surface */
void zMat3DToSphereXYZ(zMat3D* mat, zVec3D *sphere_center, double sphere_radius, zVec3D *out_x, zVec3D *out_y, zVec3D *out_z )
{
  zVec3D xarray_pos, yarray_pos, zarray_pos;
  zVec3DCat( sphere_center, sphere_radius, ZVEC3DX, &xarray_pos );
  zVec3DCat( sphere_center, sphere_radius, ZVEC3DY, &yarray_pos );
  zVec3DCat( sphere_center, sphere_radius, ZVEC3DZ, &zarray_pos );
  zMulMat3DVec3D( mat, &xarray_pos, out_x );
  zMulMat3DVec3D( mat, &yarray_pos, out_y );
  zMulMat3DVec3D( mat, &zarray_pos, out_z );
}

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
rkglFrameHandle g_fh;
double g_feedrate_s;
typedef struct{
  zShape3D shape;
  int display_id[2]; /* wire off/on */
} rkglShapeSphereData;
rkglShapeSphereData g_sphere;

typedef struct{
  zRotIp rotip;
  zVec3D start_draw_unitvec;
  int display_id;
} sphereXYZ;
sphereXYZ g_rot_curve;

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

/* convert a unit vector to the drawing point on the sphere from the rotation matrix and a radius of the sphere */
/* And get a radius vector from the axis of the rotation matrix */
zVec3D* getAxisAndDrawPointAndRadiusUnitVec(zMat3D* mat, zVec3D* start_draw_unitvec, double radius, zVec3D *out_unit_axis, zVec3D *out_drawp, zVec3D *out_radius_unitvec)
{
  zVec3D aa, axis, rotated_unitvec, radius_vec;
  double angle, cosine;
  zMulMat3DVec3D( mat, start_draw_unitvec, &rotated_unitvec );
  zVec3DMul( &rotated_unitvec, radius, out_drawp );
  zMat3DToAA( mat, &aa );
  zAA2AngleAndAxis( &aa, &angle, &axis );
  zVec3DNormalize( &axis, out_unit_axis );
  cosine = zVec3DInnerProd( &rotated_unitvec, out_unit_axis );
  zVec3DCat( &rotated_unitvec, -cosine, out_unit_axis, &radius_vec );
  zVec3DNormalize( &radius_vec, out_radius_unitvec );
  return out_radius_unitvec;
}

static int _rkglRotationTorusVertNorm(zRotIp *rotip, zVec3D* start_draw_unitvec, zVec3D *center, double radius, double tube_radius, int div, zMesh3D *vert, zMesh3D *norm)
{
  /* double u, v; */
  double u;
  int i, j;
  zMat3D mat;
  zVec3D rot_axis, aa2, drawp, tube_radius_vec, radius_unitvec, tube_axis;

  zMesh3DAlloc( vert, rotip->ns+1, div+1 );
  zMesh3DAlloc( norm, rotip->ns+1, div+1 );
  if( zArray2RowSize(vert) == 0 || zArray2RowSize(norm) == 0 ) return -1;
  for( i=0; i<=rotip->ns; i++ ){
    u = zRotIpKnotSlice( rotip, i );
    zRotIpPopMat3D( rotip, u, &mat );
    getAxisAndDrawPointAndRadiusUnitVec( &mat, start_draw_unitvec, radius, &rot_axis, &drawp, &radius_unitvec );
    zVec3DOuterProd( &radius_unitvec, &rot_axis, &tube_axis );
    zVec3DNormalizeDRC( &tube_axis ); /* no need in ideal case */
    for( j=0; j<=div; j++ ){
      zVec3DMul( &tube_axis, 2*zPI*j/div, &aa2 );
      zVec3DRot( &radius_unitvec, &aa2, zArray2ElemNC(norm,i,j) );
      zVec3DMul( zArray2ElemNC(norm,i,j), tube_radius, &tube_radius_vec );
      zVec3DAdd( &tube_radius_vec, &drawp, zArray2ElemNC(vert,i,j) );
      zVec3DAddDRC( zArray2ElemNC(vert,i,j), center );
    }
  }
  return zArray2RowSize(vert);
}


static void _rkglRotationTorusFace(zRotIp *rotip, int div, zMesh3D *vert, zMesh3D *norm)
{
  int i, j;

  glShadeModel( GL_SMOOTH );
  for( i=0; i<rotip->ns; i++ ){
    for( j=0; j<div; j++ ){
      glBegin( GL_TRIANGLE_STRIP );
      rkglNormal( zArray2ElemNC(norm,i,j) );     rkglVertex( zArray2ElemNC(vert,i,j) );
      rkglNormal( zArray2ElemNC(norm,i+1,j) );   rkglVertex( zArray2ElemNC(vert,i+1,j) );
      rkglNormal( zArray2ElemNC(norm,i,j+1) );   rkglVertex( zArray2ElemNC(vert,i,j+1) );
      rkglNormal( zArray2ElemNC(norm,i+1,j+1) ); rkglVertex( zArray2ElemNC(vert,i+1,j+1) );
      glEnd();
    }
  }
}

static void _rkglRotationTorusWireframe(zRotIp *rotip, int div, zMesh3D *vert)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  rkglColorWhite();
  for( i=0; i<=rotip->ns; i++ ){
    glBegin( GL_LINE_STRIP );
    for( j=0; j<=div; j++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  for( j=0; j<=div; j++ ){
    glBegin( GL_LINE_STRIP );
    for( i=0; i<=rotip->ns; i++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

void rkglRotationTorus(zRotIp *rotip, zVec3D* start_draw_unitvec, zVec3D *center, double radius, double tube_radius, int div, ubyte disptype, zRGB *rgb)
{
  zMesh3D vert, norm;

  if( _rkglRotationTorusVertNorm( rotip, start_draw_unitvec, center, radius, tube_radius, div, &vert, &norm ) > 0 ){
    glPushName( -1 );
    if( disptype & RKGL_FACE )
      _rkglRotationTorusFace( rotip, div, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglRotationTorusWireframe( rotip, div, &vert );
    glPopName();
  }
  zArray2Free( &vert );
  zArray2Free( &norm );
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

/* draw part */

void draw_framehandle(void)
{
  rkglFrameHandleDraw( &g_fh );
}

void draw_sphere(void)
{
  glPushMatrix();
  rkglTranslate( rkglFrameHandlePos( &g_fh ) );
  glPushName( 0 );
  g_dispswitch ? glCallList( g_sphere.display_id[0] ) : glCallList( g_sphere.display_id[1] );
  glPopName();
  glPopMatrix();
}

void draw_frame_rot_curve(void)
{
  zRGB rgb;
  glPushMatrix();
  zRGBSet( &rgb, 1.0, 1.0, 1.0 );
  glLineWidth( 3 );
  rkglRotationCurve( &g_rot_curve.rotip, &g_rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_fh ), g_LENGTH, &rgb);
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  glLineWidth( 1 );
  rkglRotationCurveCP(&g_rot_curve.rotip, &g_rot_curve.start_draw_unitvec, rkglFrameHandlePos( &g_fh ), g_LENGTH, g_CP_SIZE, &rgb);
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

/* end of draw part */

/* callback event */

void mouse(int button, int state, int x, int y)
{
  rkglSelectionBuffer sb;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      if( rkglSelectNearest( &sb, &g_cam, draw_framehandle, x, y, 1, 1 ) ){
        rkglFrameHandleAnchor( &g_fh, &sb, &g_cam, x, y );
      }
    } else
    if( state == GLUT_UP ){
      rkglFrameHandleUnselect( &g_fh );
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default: ;
  }
  if( rkglFrameHandleIsUnselected( &g_fh ) )
    rkglMouseFuncGLUT( button, state, x, y );
}

void motion(int x, int y)
{
  if( !rkglFrameHandleIsUnselected( &g_fh ) ){
    rkglFrameHandleMove( &g_fh, &g_cam, x, y );
  } else
    rkglMouseDragFuncGLUT( x, y );
}

void resize(int w, int h)
{
  rkglVPCreate( &g_cam, 0, 0, w, h );
  rkglOrthoScaleH( &g_cam, g_scale, g_znear, g_zfar );
}

void update_framehandle_att(double s)
{
  zMat3D mat;
  g_feedrate_s = s;
  double start_s = zRotIpKnotSlice( &g_rot_curve.rotip, 0 );
  double end_s = zRotIpKnotSlice( &g_rot_curve.rotip, zRotIpSliceNum(&g_rot_curve.rotip) );
  if( g_feedrate_s < start_s ) g_feedrate_s = start_s;
  if( g_feedrate_s > end_s ) g_feedrate_s = end_s;
  zRotIpPopMat3D( &g_rot_curve.rotip, s, &mat );
  zMat3DCopy( &mat, rkglFrameHandleAtt( &g_fh ) );
}

void keyboard(unsigned char key, int x, int y)
{
  double ds = zRotIpKnotOneSlice( &g_rot_curve.rotip );
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
  case 'f': update_framehandle_att( g_feedrate_s + ds); break; /* forward */
  case 'b': update_framehandle_att( g_feedrate_s - ds); break; /* backward */
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
  rkglFrameHandleCreate( &g_fh, 0, g_LENGTH, g_MAGNITUDE );
  /* sphere */
  int div = 8;
  zShape3DSphereCreate( &g_sphere.shape, rkglFrameHandlePos( &g_fh ), g_LENGTH, div );
  double r=1.0, g=1.0, b=1.0;
  zOpticalInfo oi_alt;
  zOpticalInfoCreate( &oi_alt, r, g, b, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SHAPE_SPHERE_ALPHA, NULL );
  g_sphere.display_id[0] = rkglEntryShape( &g_sphere.shape, &oi_alt, RKGL_FACE, &g_light );
  g_sphere.display_id[1] = rkglEntryShape( &g_sphere.shape, &oi_alt, (RKGL_FACE | RKGL_WIREFRAME), &g_light );
  /**/
  zVec3DCreate( &g_rot_curve.start_draw_unitvec, 1.0, 0.0, 0.0 );
  zRotIpInit( &g_rot_curve.rotip );
  zRotIpAlloc( &g_rot_curve.rotip, g_CP_NUM );
  zRotIpSetSliceNum( &g_rot_curve.rotip, g_SLICE_NUM );
  /**/
  g_feedrate_s = 0.0;
  /**/
  zMat3D m0, m1, m2;
  zMat3DFromZYX( &m0, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zMat3DFromZYX( &m1, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zMat3DFromZYX( &m2, zRandF(-zPI,zPI), zRandF(-zPI,zPI), zRandF(-zPI,zPI) );
  zRotIpSetCP( &g_rot_curve.rotip, 0, &m0 );
  zRotIpSetCP( &g_rot_curve.rotip, 1, &m1 );
  zRotIpSetCP( &g_rot_curve.rotip, 2, &m2 );
}

int main(int argc, char *argv[])
{
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
  return 0;
}
