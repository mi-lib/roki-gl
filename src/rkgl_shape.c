/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shape - visualization of shapes.
 */

#include <stdarg.h>
#include <roki_gl/rkgl_shape.h>
#include <zeo/zeo_bv3d.h>

/* a 3D point. */
void rkglPoint(zVec3D *p)
{
  glBegin( GL_POINTS );
    rkglVertex( p );
  glEnd();
}

/* a 3D edge. */
void rkglEdge(zEdge3D *e)
{
  glBegin( GL_LINES );
    rkglVertex( zEdge3DVert(e,0) );
    rkglVertex( zEdge3DVert(e,1) );
  glEnd();
}

/* a 3D triangle face. */
void rkglTriFace(zTri3D *t)
{
  glBegin( GL_TRIANGLES );
    rkglNormal( zTri3DNorm(t) );
    rkglVertex( zTri3DVert(t,0) );
    rkglVertex( zTri3DVert(t,1) );
    rkglVertex( zTri3DVert(t,2) );
    rkglNormalRev( zTri3DNorm(t) );
    rkglVertex( zTri3DVert(t,0) );
    rkglVertex( zTri3DVert(t,2) );
    rkglVertex( zTri3DVert(t,1) );
  glEnd();
}

/* a 3D triangle wireframe. */
void rkglTriWireframe(zTri3D *t)
{
  glBegin( GL_LINE_LOOP );
    rkglVertex( zTri3DVert(t,0) );
    rkglVertex( zTri3DVert(t,1) );
    rkglVertex( zTri3DVert(t,2) );
  glEnd();
}

/* a 3D triangle with texture. */
void rkglTriTexture(zTri3D *t, zTri2D *f)
{
  glBegin( GL_TRIANGLES );
    rkglNormal( zTri3DNorm(t) );
    rkglCoord( zTri2DVert(f,0) ); rkglVertex( zTri3DVert(t,0) );
    rkglCoord( zTri2DVert(f,1) ); rkglVertex( zTri3DVert(t,1) );
    rkglCoord( zTri2DVert(f,2) ); rkglVertex( zTri3DVert(t,2) );
  glEnd();
}

/* a 3D triangle with bump map. */
void rkglTriBump(zTri3D *t, zTri2D *f, zVec3D *lp)
{
  GLdouble m[16];
  zVec3D lp_local, lv;
  int i;

  glGetDoublev( GL_MODELVIEW_MATRIX, m );
  rkglXformInvd( m, lp->e, lp_local.e );
  glBegin( GL_TRIANGLES );
  rkglNormal( zTri3DNorm(t) );
  for( i=0; i<3; i++ ){
    rkglCoord( zTri2DVert(f,i) );
    zVec3DSub( &lp_local, zTri3DVert(t,i), &lv );
    glMultiTexCoord3d( GL_TEXTURE1, lv.c.x, lv.c.y, lv.c.z );
    rkglVertex( zTri3DVert(t,i) );
  }
  glEnd();
}

/* a 3D polygons. */
void rkglPolygon(zVec3D v[], int n, ...)
{
  zVec3D v0, v1, v2, norm;
  va_list args;
  int i;

  if( n < 3 ){
    ZRUNERROR( "cannot create polygon from less than three vertices" );
    return;
  }
  /* normal vector */
  va_start( args, n );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v0 );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v1 );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v2 );
  va_end( args );
  zVec3DSubDRC( &v1, &v0 );
  zVec3DSubDRC( &v2, &v0 );
  zVec3DOuterProd( &v1, &v2, &norm );
  zVec3DNormalizeDRC( &norm );
  glBegin( GL_POLYGON );
    rkglNormal( &norm );
    /* vertices */
    va_start( args, n );
    for( i=0; i<n; i++ )
      rkglVertex( &v[(int)va_arg( args, int )] );
    va_end( args );
  glEnd();
}

/* draw a face of a 3D box (legacy). */
static void _rkglBoxFaceLegacy(zVec3D vert[8])
{
  glShadeModel( GL_FLAT );
  rkglPolygon( vert, 4, 0, 1, 2, 3 );
  rkglPolygon( vert, 4, 7, 6, 5, 4 );
  rkglPolygon( vert, 4, 0, 3, 7, 4 );
  rkglPolygon( vert, 4, 1, 5, 6, 2 );
  rkglPolygon( vert, 4, 0, 4, 5, 1 );
  rkglPolygon( vert, 4, 2, 6, 7, 3 );
}

/* draw a face of a 3D box. */
static void _rkglBoxFace(zVec3D vert[8])
{
  int i;
  zPH3D ph;

  if( !zPH3DAlloc( &ph, 8, 12 ) ) return;
  for( i=0; i<8; i++ )
    zVec3DCopy( &vert[i], zPH3DVert(&ph,i) );
  zTri3DCreate( zPH3DFace(&ph,0),  zPH3DVert(&ph,0), zPH3DVert(&ph,1), zPH3DVert(&ph,2) );
  zTri3DCreate( zPH3DFace(&ph,1),  zPH3DVert(&ph,0), zPH3DVert(&ph,2), zPH3DVert(&ph,3) );
  zTri3DCreate( zPH3DFace(&ph,2),  zPH3DVert(&ph,0), zPH3DVert(&ph,4), zPH3DVert(&ph,5) );
  zTri3DCreate( zPH3DFace(&ph,3),  zPH3DVert(&ph,0), zPH3DVert(&ph,5), zPH3DVert(&ph,1) );
  zTri3DCreate( zPH3DFace(&ph,4),  zPH3DVert(&ph,1), zPH3DVert(&ph,5), zPH3DVert(&ph,6) );
  zTri3DCreate( zPH3DFace(&ph,5),  zPH3DVert(&ph,1), zPH3DVert(&ph,6), zPH3DVert(&ph,2) );
  zTri3DCreate( zPH3DFace(&ph,6),  zPH3DVert(&ph,2), zPH3DVert(&ph,6), zPH3DVert(&ph,7) );
  zTri3DCreate( zPH3DFace(&ph,7),  zPH3DVert(&ph,2), zPH3DVert(&ph,7), zPH3DVert(&ph,3) );
  zTri3DCreate( zPH3DFace(&ph,8),  zPH3DVert(&ph,0), zPH3DVert(&ph,3), zPH3DVert(&ph,7) );
  zTri3DCreate( zPH3DFace(&ph,9),  zPH3DVert(&ph,0), zPH3DVert(&ph,7), zPH3DVert(&ph,4) );
  zTri3DCreate( zPH3DFace(&ph,10), zPH3DVert(&ph,7), zPH3DVert(&ph,6), zPH3DVert(&ph,5) );
  zTri3DCreate( zPH3DFace(&ph,11), zPH3DVert(&ph,7), zPH3DVert(&ph,5), zPH3DVert(&ph,4) );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  for( i=0; i<zPH3DFaceNum(&ph); i++ )
    rkglTriFace( zPH3DFace(&ph,i) );
  zPH3DDestroy(&ph);
}

/* draw a wireframe of a 3D box. */
static void _rkglBoxWireframe(zVec3D vert[8])
{
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glBegin( GL_LINE_LOOP );
    rkglVertex( &vert[0] );
    rkglVertex( &vert[1] );
    rkglVertex( &vert[2] );
    rkglVertex( &vert[3] );
  glEnd();
  glBegin( GL_LINE_LOOP );
    rkglVertex( &vert[4] );
    rkglVertex( &vert[5] );
    rkglVertex( &vert[6] );
    rkglVertex( &vert[7] );
  glEnd();
  glBegin( GL_LINES );
    rkglVertex( &vert[0] );
    rkglVertex( &vert[4] );
    rkglVertex( &vert[1] );
    rkglVertex( &vert[5] );
    rkglVertex( &vert[2] );
    rkglVertex( &vert[6] );
    rkglVertex( &vert[3] );
    rkglVertex( &vert[7] );
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw an axis-aligned 3D box. */
void rkglAABox(zAABox3D *box, ubyte disptype)
{
  zVec3D vert[8];
  int i;

  for( i=0; i<8; i++ )
    zAABox3DVert( box, i, &vert[i] ); /* vertices */
  if( disptype & RKGL_FACE )
    _rkglBoxFace( vert );
  if( disptype & RKGL_WIREFRAME )
    _rkglBoxWireframe( vert );
}

/* draw a 3D box. */
void rkglBox(zBox3D *box, ubyte disptype)
{
  zVec3D vert[8];
  int i;

  for( i=0; i<8; i++ )
    zBox3DVert( box, i, &vert[i] ); /* vertices */
  if( disptype & RKGL_FACE )
    _rkglBoxFace( vert );
  if( disptype & RKGL_WIREFRAME )
    _rkglBoxWireframe( vert );
}
static void _rkglShapeBox(void *box, ubyte disptype){ rkglBox( (zBox3D *)box, disptype ); }

zArray2Class( zMesh3D, zVec3D );
#define zMesh3DAlloc(mesh,row,col) zArray2Alloc( mesh, zVec3D, row, col )

static int _rkglHemisphereVertNorm(zSphere3D *sphere, zVec3D *dir, zMesh3D *vert, zMesh3D *norm)
{
  int i, j, n1;
  zVec3D a, d, s, dr, aa;
  double phi;

  n1 = ( zIsOdd( zSphere3DDiv(sphere) ) ? zSphere3DDiv(sphere)+1 : zSphere3DDiv(sphere) ) / 2;
  zMesh3DAlloc( vert, n1+1, zSphere3DDiv(sphere)+1 );
  zMesh3DAlloc( norm, n1+1, zSphere3DDiv(sphere)+1 );
  if( zArray2RowSize(vert) == 0 || zArray2RowSize(norm) == 0 ) return -1;
  zVec3DNormalize( dir, &a );
  zVec3DMul( &a, zSphere3DRadius(sphere), &d );
  _zVec3DCreate( &s, a.c.y-a.c.z, a.c.z-a.c.x, a.c.x-a.c.y );
  zVec3DMulDRC( &s, zSphere3DRadius(sphere)/zVec3DNorm(&s) );
  for( i=0; i<=n1; i++ ){
    phi = zPI_2 * i / n1;
    zVec3DMul( &d, cos(phi), &dr );
    zVec3DCatDRC( &dr, sin(phi), &s );
    for( j=0; j<=zSphere3DDiv(sphere); j++ ){
      zVec3DMul( &a, zPIx2 * j / zSphere3DDiv(sphere), &aa );
      zVec3DRot( &dr, &aa, zArray2ElemNC(norm,i,j) );
      zVec3DAdd( zSphere3DCenter(sphere), zArray2ElemNC(norm,i,j), zArray2ElemNC(vert,i,j) );
    }
  }
  return n1;
}

static void _rkglHemisphereFace(zMesh3D *vert, zMesh3D *norm, int nl, int ndiv)
{
  int i, i1, j, j1;

  glShadeModel( GL_SMOOTH );
  for( i=1; i<=nl; i++ ){
    i1 = i - 1;
    for( j=1; j<=ndiv; j++ ){
      j1 = j - 1;
      glBegin( GL_TRIANGLE_STRIP );
        /* outer face */
        rkglNormal( zArray2ElemNC(norm,i,j) );      rkglVertex( zArray2ElemNC(vert,i,j) );
        rkglNormal( zArray2ElemNC(norm,i1,j) );     rkglVertex( zArray2ElemNC(vert,i1,j) );
        rkglNormal( zArray2ElemNC(norm,i,j1) );     rkglVertex( zArray2ElemNC(vert,i,j1) );
        rkglNormal( zArray2ElemNC(norm,i1,j1) );    rkglVertex( zArray2ElemNC(vert,i1,j1) );
        /* inner face */
        rkglNormalRev( zArray2ElemNC(norm,i,j1) );  rkglVertex( zArray2ElemNC(vert,i,j1) );
        rkglNormalRev( zArray2ElemNC(norm,i1,j1) ); rkglVertex( zArray2ElemNC(vert,i1,j1) );
        rkglNormalRev( zArray2ElemNC(norm,i,j) );   rkglVertex( zArray2ElemNC(vert,i,j) );
        rkglNormalRev( zArray2ElemNC(norm,i1,j) );  rkglVertex( zArray2ElemNC(vert,i1,j) );
      glEnd();
    }
  }
}

static void _rkglHemisphereWireframe(zMesh3D *vert, int nl, int ndiv)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  for( i=0; i<=nl; i++ ){
    glBegin( GL_LINE_LOOP );
    for( j=0; j<ndiv; j++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  for( j=0; j<ndiv; j++ ){
    glBegin( GL_LINE_STRIP );
    for( i=0; i<=nl; i++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D hemisphere. */
void rkglHemisphere(zSphere3D *sphere, zVec3D *dir, ubyte disptype)
{
  int nl;
  zMesh3D vert, norm;

  if( ( nl = _rkglHemisphereVertNorm( sphere, dir, &vert, &norm ) ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglHemisphereFace( &vert, &norm, nl, zSphere3DDiv(sphere) );
    if( disptype & RKGL_WIREFRAME )
      _rkglHemisphereWireframe( &vert, nl, zSphere3DDiv(sphere) );
  }
  zArray2Free( &vert );
  zArray2Free( &norm );
}

static int _rkglSphereVert(zSphere3D *sphere, zMesh3D *vert)
{
  int i, j, n2;

  n2 = zSphere3DDiv(sphere) * 2;
  zMesh3DAlloc( vert, zSphere3DDiv(sphere)+1, n2+1 );
  if( zArray2RowSize(vert) == 0 ) return -1;
  for( i=0; i<=zSphere3DDiv(sphere); i++ )
    for( j=0; j<=n2; j++ )
      zVec3DCreatePolar( zArray2ElemNC(vert,i,j), zSphere3DRadius(sphere),
        zPI*i/zSphere3DDiv(sphere), zPI*j/zSphere3DDiv(sphere) );
  return n2;
}

static void _rkglSphereFace(zSphere3D *sphere, zMesh3D *vert, int nl)
{
  int i, i1, j, j1;
  zVec3D v;

  glShadeModel( GL_SMOOTH );
  for( i=1; i<=zSphere3DDiv(sphere); i++ )
    for( j=1; j<=nl; j++ ){
      i1 = i - 1; j1 = j - 1;
      glBegin( GL_TRIANGLE_STRIP );
        zVec3DAdd( zArray2ElemNC(vert,i,j),   zSphere3DCenter(sphere), &v );
        rkglNormal( zArray2ElemNC(vert,i,j) );   rkglVertex( &v );
        zVec3DAdd( zArray2ElemNC(vert,i1,j),  zSphere3DCenter(sphere), &v );
        rkglNormal( zArray2ElemNC(vert,i1,j) );  rkglVertex( &v );
        zVec3DAdd( zArray2ElemNC(vert,i,j1),  zSphere3DCenter(sphere), &v );
        rkglNormal( zArray2ElemNC(vert,i,j1) );  rkglVertex( &v );
        zVec3DAdd( zArray2ElemNC(vert,i1,j1), zSphere3DCenter(sphere), &v );
        rkglNormal( zArray2ElemNC(vert,i1,j1) ); rkglVertex( &v );
      glEnd();
    }
}

static void _rkglSphereWireframe(zSphere3D *sphere, zMesh3D *vert, int nl)
{
  int i, j;
  bool lighting_is_enabled;
  zVec3D v;

  rkglSaveLighting( &lighting_is_enabled );
  for( i=0; i<zSphere3DDiv(sphere); i++ ){
    glBegin( GL_LINE_LOOP );
    for( j=0; j<nl; j++ ){
      zVec3DAdd( zArray2ElemNC(vert,i,j), zSphere3DCenter(sphere), &v );
      rkglVertex( &v );
    }
    glEnd();
  }
  for( j=0; j<nl; j++ ){
    glBegin( GL_LINE_STRIP );
    for( i=0; i<=zSphere3DDiv(sphere); i++ ){
      zVec3DAdd( zArray2ElemNC(vert,i,j), zSphere3DCenter(sphere), &v );
      rkglVertex( &v );
    }
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D sphere. */
void rkglSphere(zSphere3D *sphere, ubyte disptype)
{
  int n2;
  zMesh3D vert;

  if( ( n2 = _rkglSphereVert( sphere, &vert ) ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglSphereFace( sphere, &vert, n2 );
    if( disptype & RKGL_WIREFRAME )
      _rkglSphereWireframe( sphere, &vert, n2 );
  }
  zArray2Free( &vert );
}
static void _rkglShapeSphere(void *sphere, ubyte disptype){ rkglSphere( (zSphere3D *)sphere, disptype ); }

static zVec3D *_rkglEllipsNormal(zEllips3D *ellips, zVec3D *v, zVec3D *n)
{
  zVec3D tmp;

  zVec3DCreate( &tmp,
    v->e[zX]/zEllips3DRadiusX(ellips),
    v->e[zY]/zEllips3DRadiusY(ellips),
    v->e[zZ]/zEllips3DRadiusZ(ellips) );
  zMulMat3DVec3D( zFrame3DAtt(&ellips->f), &tmp, n );
  zVec3DNormalizeDRC( n );
  return n;
}

static int _rkglEllipsVertNorm(zEllips3D *ellips, zMesh3D *vert, zMesh3D *norm)
{
  int i, j, n2;
  zVec3D tmp;

  n2 = zEllips3DDiv(ellips) * 2;
  zMesh3DAlloc( vert, zEllips3DDiv(ellips)+1, n2+1 );
  zMesh3DAlloc( norm, zEllips3DDiv(ellips)+1, n2+1 );
  if( zArray2RowSize(vert) == 0 || zArray2RowSize(norm) == 0 ) return -1;
  for( i=0; i<=zEllips3DDiv(ellips); i++ )
    for( j=0; j<=n2; j++ ){
      zVec3DCreatePolar( &tmp, 1.0,
        zPI*i/zEllips3DDiv(ellips), zPI*j/zEllips3DDiv(ellips) );
      _rkglEllipsNormal( ellips, &tmp, zArray2ElemNC(norm,i,j) );
      tmp.e[zX] *= zEllips3DRadiusX(ellips);
      tmp.e[zY] *= zEllips3DRadiusY(ellips);
      tmp.e[zZ] *= zEllips3DRadiusZ(ellips);
      zXform3D( &ellips->f, &tmp, zArray2ElemNC(vert,i,j) );
    }
  return n2;
}

static void _rkglEllipsFace(zMesh3D *vert, zMesh3D *norm, int nl, int ndiv)
{
  int i, i1, j, j1;

  glShadeModel( GL_SMOOTH );
  for( i=1; i<=ndiv; i++ )
    for( j=1; j<=nl; j++ ){
      i1 = i - 1; j1 = j - 1;
      glBegin( GL_TRIANGLE_STRIP );
        rkglNormal( zArray2ElemNC(norm,i ,j ) ); rkglVertex( zArray2ElemNC(vert,i ,j ) );
        rkglNormal( zArray2ElemNC(norm,i1,j ) ); rkglVertex( zArray2ElemNC(vert,i1,j ) );
        rkglNormal( zArray2ElemNC(norm,i ,j1) ); rkglVertex( zArray2ElemNC(vert,i ,j1) );
        rkglNormal( zArray2ElemNC(norm,i1,j1) ); rkglVertex( zArray2ElemNC(vert,i1,j1) );
      glEnd();
    }
}

static void _rkglEllipsWireframe(zMesh3D *vert, zMesh3D *norm, int nl, int ndiv)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  for( i=0; i<ndiv; i++ ){
    glBegin( GL_LINE_LOOP );
    for( j=0; j<nl; j++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  for( j=0; j<nl; j++ ){
    glBegin( GL_LINE_STRIP );
    for( i=0; i<=ndiv; i++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D ellipsoid. */
void rkglEllips(zEllips3D *ellips, ubyte disptype)
{
  int n2;
  zMesh3D vert, norm;

  if( ( n2 = _rkglEllipsVertNorm( ellips, &vert, &norm ) ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglEllipsFace( &vert, &norm, n2, zSphere3DDiv(ellips) );
    if( disptype & RKGL_WIREFRAME )
      _rkglEllipsWireframe( &vert, &norm, n2, zSphere3DDiv(ellips) );
  }
  zArray2Free( &vert );
  zArray2Free( &norm );
}
static void _rkglShapeEllips(void *ellips, ubyte disptype){ rkglEllips( (zEllips3D *)ellips, disptype ); }

static int _rkglTubeVertNorm(zCyl3D *tube, zVec3D *axis, zMesh3D *vert, zVec3DArray *norm)
{
  zVec3D s, aa;
  double l;
  int i;

  zVec3DArrayAlloc( norm, zCyl3DDiv(tube)+1 );
  zMesh3DAlloc( vert, 2, zCyl3DDiv(tube)+1 );
  if( zArraySize( norm ) == 0 || zArray2RowSize( vert ) == 0 ) return -1;
  zCyl3DAxis( tube, axis );
  if( zIsTiny( ( l = zVec3DNorm( axis ) ) ) ){
    ZRUNERROR( "cannot draw a zero-height tube" );
    return -1;
  }
  zVec3DDivDRC( axis, l );
  zVec3DOrthonormal( axis, &s );
  zVec3DMulDRC( &s, zCyl3DRadius(tube) ); /* one radial vector */
  for( i=0; i<=zCyl3DDiv(tube); i++ ){
    zVec3DMul( axis, -2*zPI*i/zCyl3DDiv(tube), &aa );
    zVec3DRot( &s, &aa, zArrayElemNC(norm,i) );
    /* vertices on the top rim */
    zVec3DAdd( zCyl3DCenter(tube,0), zArrayElemNC(norm,i), zArray2ElemNC(vert,0,i) );
    /* vertices on the bottom rim */
    zVec3DAdd( zCyl3DCenter(tube,1), zArrayElemNC(norm,i), zArray2ElemNC(vert,1,i) );
  }
  return zArraySize(norm);
}

static void _rkglTubeFace(zCyl3D *tube, zMesh3D *vert, zVec3DArray *norm)
{
  int i;

  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_TRIANGLE_STRIP );
  for( i=0; i<=zCyl3DDiv(tube); i++ ){
    rkglNormal( zArrayElemNC(norm,i) );
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  /* inner faces */
  for( i=0; i<=zCyl3DDiv(tube); i++ ){
    rkglNormalRev( zArrayElemNC(norm,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
    rkglVertex( zArray2ElemNC(vert,0,i) );
  }
  glEnd();
}

static void _rkglTubeWireframe(zCyl3D *tube, zMesh3D *vert)
{
  int i;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  /* side faces */
  glBegin( GL_LINES );
  for( i=0; i<=zCyl3DDiv(tube); i++ ){
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D tube. */
void rkglTube(zCyl3D *tube, ubyte disptype)
{
  zMesh3D vert;
  zVec3DArray norm;
  zVec3D axis;

  if( _rkglTubeVertNorm( tube, &axis, &vert, &norm ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglTubeFace( tube, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglTubeWireframe( tube, &vert );
  }
  zArrayFree( &norm );
  zArray2Free( &vert );
}

static void _rkglCylFace(zCyl3D *cyl, zVec3D *axis, zMesh3D *vert, zVec3DArray *norm)
{
  int i;

  /* top faces */
  glShadeModel( GL_FLAT );
  rkglNormal( axis );
  glBegin( GL_TRIANGLE_FAN );
  for( i=zCyl3DDiv(cyl)-1; i>=0; i-- )
    rkglVertex( zArray2ElemNC(vert,1,i) );
  glEnd();
  /* bottom faces */
  rkglNormalRev( axis );
  glBegin( GL_TRIANGLE_FAN );
  for( i=0; i<zCyl3DDiv(cyl); i++ )
    rkglVertex( zArray2ElemNC(vert,0,i) );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_TRIANGLE_STRIP );
  for( i=0; i<=zCyl3DDiv(cyl); i++ ){
    rkglNormal( zArrayElemNC(norm,i) );
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  glEnd();
}

static void _rkglCylWireframe(zCyl3D *cyl, zMesh3D *vert)
{
  int i;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  /* top faces */
  glBegin( GL_LINE_LOOP );
  for( i=zCyl3DDiv(cyl)-1; i>=0; i-- )
    rkglVertex( zArray2ElemNC(vert,1,i) );
  glEnd();
  /* bottom faces */
  glBegin( GL_LINE_LOOP );
  for( i=0; i<zCyl3DDiv(cyl); i++ )
    rkglVertex( zArray2ElemNC(vert,0,i) );
  glEnd();
  /* side faces */
  glBegin( GL_LINES );
  for( i=0; i<=zCyl3DDiv(cyl); i++ ){
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D cylinder. */
void rkglCyl(zCyl3D *cyl, ubyte disptype)
{
  zVec3DArray norm;
  zMesh3D vert;
  zVec3D axis;

  if( _rkglTubeVertNorm( cyl, &axis, &vert, &norm ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglCylFace( cyl, &axis, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglCylWireframe( cyl, &vert );
  }
  zArray2Free( &vert );
  zArrayFree( &norm );
}
static void _rkglShapeCyl(void *cyl, ubyte disptype){ rkglCyl( (zCyl3D *)cyl, disptype ); }

/* draw a 3D capsule. */
void rkglCapsule(zCapsule3D *capsule, ubyte disptype)
{
  zVec3D dir;
  zSphere3D hemisphere;

  zSphere3DCreate( &hemisphere, zCapsule3DCenter(capsule,1), zCapsule3DRadius(capsule), zCapsule3DDiv(capsule) );
  zCapsule3DAxis( capsule, &dir );
  rkglHemisphere( &hemisphere, &dir, disptype );
  rkglTube( capsule, disptype );
  zSphere3DCreate( &hemisphere, zCapsule3DCenter(capsule,0), zCapsule3DRadius(capsule), zCapsule3DDiv(capsule) );
  zVec3DRevDRC( &dir );
  rkglHemisphere( &hemisphere, &dir, disptype );
}
static void _rkglShapeCapsule(void *capsule, ubyte disptype){ rkglCapsule( (zCapsule3D *)capsule, disptype ); }

static int _rkglECylVertNorm(zECyl3D *ecyl, zVec3D *axis, zMesh3D *vert, zVec3DArray *norm)
{
  double l, s, c;
  int i;

  zVec3DArrayAlloc( norm, zECyl3DDiv(ecyl)+1 );
  zMesh3DAlloc( vert, 2, zECyl3DDiv(ecyl)+1 );
  if( zArraySize(norm) == 0 || zArray2RowSize(vert) == 0 ) return -1;
  zECyl3DAxis( ecyl, axis );
  if( zIsTiny( ( l = zVec3DNorm( axis ) ) ) ){
    ZRUNERROR( "cannot draw a zero-height elliptic cylinder" );
    return -1;
  }
  zVec3DDivDRC( axis, l );
  /* creation of vertices */
  for( i=0; i<=zECyl3DDiv(ecyl); i++ ){
    zSinCos( -zPIx2*i/zECyl3DDiv(ecyl), &s, &c );
    zVec3DMul( zECyl3DRadVec(ecyl,0), zECyl3DRadius(ecyl,0)*c, zArrayElemNC(norm,i) );
    zVec3DCatDRC( zArrayElemNC(norm,i), zECyl3DRadius(ecyl,1)*s, zECyl3DRadVec(ecyl,1) );
    /* vertices on the top rim */
    zVec3DAdd( zECyl3DCenter(ecyl,0), zArrayElemNC(norm,i), zArray2ElemNC(vert,0,i) );
    /* vertices on the bottom rim */
    zVec3DAdd( zECyl3DCenter(ecyl,1), zArrayElemNC(norm,i), zArray2ElemNC(vert,1,i) );
  }
  return zArraySize(norm);
}

static void _rkglECylFace(zECyl3D *ecyl, zVec3D *axis, zMesh3D *vert, zVec3DArray *norm)
{
  int i;

  /* top faces */
  glShadeModel( GL_FLAT );
  rkglNormal( axis );
  glBegin( GL_TRIANGLE_FAN );
  for( i=zECyl3DDiv(ecyl)-1; i>=0; i-- )
    rkglVertex( zArray2ElemNC(vert,1,i) );
  glEnd();
  /* bottom faces */
  rkglNormalRev( axis );
  glBegin( GL_TRIANGLE_FAN );
  for( i=0; i<zECyl3DDiv(ecyl); i++ )
    rkglVertex( zArray2ElemNC(vert,0,i) );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_TRIANGLE_STRIP );
  for( i=0; i<=zECyl3DDiv(ecyl); i++ ){
    rkglNormal( zArrayElemNC(norm,i) );
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  glEnd();
}

static void _rkglECylWireframe(zECyl3D *ecyl, zMesh3D *vert)
{
  int i;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  /* top faces */
  glBegin( GL_LINE_LOOP );
  for( i=zECyl3DDiv(ecyl)-1; i>=0; i-- )
    rkglVertex( zArray2ElemNC(vert,1,i) );
  glEnd();
  /* bottom faces */
  glBegin( GL_LINE_LOOP );
  for( i=0; i<zECyl3DDiv(ecyl); i++ )
    rkglVertex( zArray2ElemNC(vert,0,i) );
  glEnd();
  /* side faces */
  glBegin( GL_LINES );
  for( i=0; i<=zECyl3DDiv(ecyl); i++ ){
    rkglVertex( zArray2ElemNC(vert,0,i) );
    rkglVertex( zArray2ElemNC(vert,1,i) );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D elliptic cylinder. */
void rkglECyl(zECyl3D *ecyl, ubyte disptype)
{
  zVec3DArray norm;
  zMesh3D vert;
  zVec3D axis;

  if( _rkglECylVertNorm( ecyl, &axis, &vert, &norm ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglECylFace( ecyl, &axis, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglECylWireframe( ecyl, &vert );
  }
  zArrayFree( &norm );
  zArray2Free( &vert );
}
static void _rkglShapeECyl(void *ecyl, ubyte disptype){ rkglECyl( (zECyl3D *)ecyl, disptype ); }

static int _rkglConeVertNorm(zCone3D *cone, zVec3D *axis, zVec3DArray *vert, zVec3DArray *norm)
{
  zVec3D s, tmp, v;
  double l;
  int i;

  zVec3DArrayAlloc( vert, zCone3DDiv(cone)+1 );
  zVec3DArrayAlloc( norm, zCone3DDiv(cone)+1 );
  if( zArraySize(vert) == 0 || zArraySize(norm) == 0 )  return -1;
  zCone3DAxis( cone, axis );
  if( zIsTiny( ( l = zVec3DNorm( axis ) ) ) ){
    ZRUNERROR( "cannot draw a zero-height cone" );
    return -1;
  }
  zVec3DDivDRC( axis, l );
  /* one radial vector */
  zVec3DOrthonormal( axis, &s );
  zVec3DMulDRC( &s, zCone3DRadius(cone) );
  /* creation of vertices */
  for( i=0; i<=zCone3DDiv(cone); i++ ){
    zVec3DMul( axis, -2*zPI*i/zCone3DDiv(cone), &tmp );
    zVec3DRot( &s, &tmp, &v );
    /* vertices on the rim */
    zVec3DAdd( zCone3DCenter(cone), &v, zArrayElemNC(vert,i) );
    /* normal vector at each vertex */
    zVec3DSub( zArrayElemNC(vert,i), zCone3DVert(cone), &tmp );
    zVec3DNormalizeDRC( &tmp );
    zVec3DMulDRC( &tmp, zVec3DInnerProd( &v, &tmp ) );
    zVec3DSub( &v, &tmp, zArrayElem(norm,i) );
  }
  return zArraySize(vert);
}

static void _rkglConeFace(zCone3D *cone, zVec3D *axis, zVec3DArray *vert, zVec3DArray *norm)
{
  int i;

  /* bottom faces */
  glShadeModel( GL_FLAT );
  glBegin( GL_TRIANGLE_FAN );
  rkglNormalRev( axis );
  for( i=0; i<zCone3DDiv(cone); i++ )
    rkglVertex( zArrayElem(vert,i) );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_TRIANGLE_STRIP );
  for( i=zCone3DDiv(cone); i>=0; i-- ){
    rkglNormal( zArrayElemNC(norm,i) );
    rkglVertex( zCone3DVert(cone) );
    rkglVertex( zArrayElemNC(vert,i) );
  }
  glEnd();
}

static void _rkglConeWireframe(zCone3D *cone, zVec3DArray *vert)
{
  int i;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  /* bottom faces */
  glBegin( GL_LINE_LOOP );
  for( i=0; i<zCone3DDiv(cone); i++ )
    rkglVertex( zArrayElemNC(vert,i) );
  glEnd();
  /* side faces */
  glBegin( GL_LINES );
  for( i=zCone3DDiv(cone); i>=0; i-- ){
    rkglVertex( zCone3DVert(cone) );
    rkglVertex( zArrayElem(vert,i) );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D cone. */
void rkglCone(zCone3D *cone, ubyte disptype)
{
  zVec3DArray vert, norm;
  zVec3D axis;

  if( _rkglConeVertNorm( cone, &axis, &vert, &norm ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglConeFace( cone, &axis, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglConeWireframe( cone, &vert );
  }
  zArrayFree( &vert );
  zArrayFree( &norm );
}
static void _rkglShapeCone(void *cone, ubyte disptype){ rkglCone( (zCone3D *)cone, disptype ); }

static int _rkglTorusVertNorm(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, zMesh3D *vert, zMesh3D *norm)
{
  int i, j;
  zVec3D d, aa1, aa2, s, sr, dr, tmp, t;
  double rm, r;

  zMesh3DAlloc( vert, div1+1, div2+1 );
  zMesh3DAlloc( norm, div1+1, div2+1 );
  if( zArray2RowSize(vert) == 0 || zArray2RowSize(norm) == 0 ) return -1;
  if( zVec3DIsTiny( n ) || zIsTiny( r2-r1 ) ){
    ZRUNERROR( "cannot draw a zero-volume torus" );
    return -1;
  }
  if( r1 > r2 ) zSwap( double, r1, r2 ); /* always r2 >= r1 */
  rm = 0.5 * ( r1 + r2 );
  r  = 0.5 * ( r2 - r1 );
  zVec3DNormalize( n, &d );
  zVec3DOrthonormal( &d, &s );
  for( i=0; i<=div1; i++ ){
    zVec3DMul( &d, 2*zPI*i/div1, &aa1 );
    zVec3DRot( &s, &aa1, &tmp );
    zVec3DMul( &tmp, rm, &sr );
    zVec3DOuterProd( &tmp, &d, &t );
    zVec3DNormalizeDRC( &t ); /* no need in ideal case */
    for( j=0; j<=div2; j++ ){
      zVec3DMul( &t, 2*zPI*j/div2, &aa2 );
      zVec3DRot( &tmp, &aa2, zArray2ElemNC(norm,i,j) );
      zVec3DMul( zArray2ElemNC(norm,i,j), r, &dr );
      zVec3DAdd( &dr, &sr, zArray2ElemNC(vert,i,j) );
      zVec3DAddDRC( zArray2ElemNC(vert,i,j), c );
    }
  }
  return zArray2RowSize(vert);
}

static void _rkglTorusFace(zMesh3D *vert, zMesh3D *norm, int div1, int div2)
{
  int i, j;

  glShadeModel( GL_SMOOTH );
  for( i=0; i<div1; i++ ){
    glBegin( GL_TRIANGLE_STRIP );
    for( j=0; j<=div2; j++ ){
      rkglNormal( zArray2ElemNC(norm,i,j) );   rkglVertex( zArray2ElemNC(vert,i,j) );
      rkglNormal( zArray2ElemNC(norm,i+1,j) ); rkglVertex( zArray2ElemNC(vert,i+1,j) );
    }
    glEnd();
  }
}

static void _rkglTorusWireframe(zMesh3D *vert, int div1, int div2)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  for( i=0; i<div1; i++ ){
    glBegin( GL_LINE_LOOP );
    for( j=0; j<div2; j++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  for( j=0; j<div2; j++ ){
    glBegin( GL_LINE_LOOP );
    for( i=0; i<div1; i++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D torus. */
void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, ubyte disptype)
{
  zMesh3D vert, norm;

  if( _rkglTorusVertNorm( c, n, r1, r2, div1, div2, &vert, &norm ) > 0 ){
    if( disptype & RKGL_FACE )
      _rkglTorusFace( &vert, &norm, div1, div2 );
    if( disptype & RKGL_WIREFRAME )
      _rkglTorusWireframe( &vert, div1, div2 );
  }
  zArray2Free( &vert );
  zArray2Free( &norm );
}

static int _rkglNURBSVertNorm(zNURBS3D *nurbs, zMesh3D *vert, zMesh3D *norm)
{
  double u, v;
  int i, j;

  zMesh3DAlloc( vert, zNURBS3DSlice(nurbs,0)+1, zNURBS3DSlice(nurbs,1)+1 );
  zMesh3DAlloc( norm, zNURBS3DSlice(nurbs,0)+1, zNURBS3DSlice(nurbs,1)+1 );
  if( zArray2RowSize(vert) == 0 || zArray2RowSize(norm) == 0 ) return -1;
  for( i=0; i<=zNURBS3DSlice(nurbs,0); i++ ){
    u = zNURBS3DKnotSlice( nurbs, 0, i );
    for( j=0; j<=zNURBS3DSlice(nurbs,1); j++ ){
      v = zNURBS3DKnotSlice( nurbs, 1, j );
      zNURBS3DVecNorm( nurbs, u, v, zArray2ElemNC(vert,i,j), zArray2ElemNC(norm,i,j) );
    }
  }
  return zArray2RowSize(vert);
}

static void _rkglNURBSFace(zNURBS3D *nurbs, zMesh3D *vert, zMesh3D *norm)
{
  int i, j;

  glShadeModel( GL_SMOOTH );
  for( i=0; i<zNURBS3DSlice(nurbs,0); i++ ){
    for( j=0; j<zNURBS3DSlice(nurbs,1); j++ ){
      glBegin( GL_TRIANGLE_STRIP );
      rkglNormal( zArray2ElemNC(norm,i,j) );     rkglVertex( zArray2ElemNC(vert,i,j) );
      rkglNormal( zArray2ElemNC(norm,i+1,j) );   rkglVertex( zArray2ElemNC(vert,i+1,j) );
      rkglNormal( zArray2ElemNC(norm,i,j+1) );   rkglVertex( zArray2ElemNC(vert,i,j+1) );
      rkglNormal( zArray2ElemNC(norm,i+1,j+1) ); rkglVertex( zArray2ElemNC(vert,i+1,j+1) );
      glEnd();
    }
  }
}

static void _rkglNURBSWireframe(zNURBS3D *nurbs, zMesh3D *vert)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  for( i=0; i<=zNURBS3DSlice(nurbs,0); i++ ){
    glBegin( GL_LINE_STRIP );
    for( j=0; j<=zNURBS3DSlice(nurbs,1); j++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  for( j=0; j<=zNURBS3DSlice(nurbs,1); j++ ){
    glBegin( GL_LINE_STRIP );
    for( i=0; i<=zNURBS3DSlice(nurbs,0); i++ )
      rkglVertex( zArray2ElemNC(vert,i,j) );
    glEnd();
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D NURBS surface. */
void rkglNURBS(zNURBS3D *nurbs, ubyte disptype)
{
  zMesh3D vert, norm;

  if( _rkglNURBSVertNorm( nurbs, &vert, &norm ) > 0 ){
    glPushName( -1 );
    if( disptype & RKGL_FACE )
      _rkglNURBSFace( nurbs, &vert, &norm );
    if( disptype & RKGL_WIREFRAME )
      _rkglNURBSWireframe( nurbs, &vert );
    glPopName();
  }
  zArray2Free( &vert );
  zArray2Free( &norm );
}
static void _rkglShapeNURBS(void *nurbs, ubyte disptype){ rkglNURBS( (zNURBS3D *)nurbs, disptype ); }

/* draw a 3D NURBS surface with conrol points. */
void rkglNURBSCP(zNURBS3D *nurbs)
{
  int i, j;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glPushName( 0 );
  for( i=0; i<zNURBS3DCPNum(nurbs,0); i++ ){
    glLoadName( i );
    glPushName( 0 );
    for( j=0; j<zNURBS3DCPNum(nurbs,1); j++ ){
      glLoadName( j );
      glBegin( GL_POINTS );
        rkglVertex( zNURBS3DCP(nurbs,i,j) );
      glEnd();
    }
    glPopName();
  }
  glPopName();
  glPushName( -1 );
  for( i=0; i<zNURBS3DCPNum(nurbs,0); i++ ){
    for( j=0; j<zNURBS3DCPNum(nurbs,1); j++ ){
      if( i > 0 ){
        glBegin( GL_LINES );
        rkglVertex( zNURBS3DCP(nurbs,i,j) );
        rkglVertex( zNURBS3DCP(nurbs,i-1,j) );
        glEnd();
      }
      if( j > 0 ){
        glBegin( GL_LINES );
        rkglVertex( zNURBS3DCP(nurbs,i,j) );
        rkglVertex( zNURBS3DCP(nurbs,i,j-1) );
        glEnd();
      }
    }
  }
  glPopName();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D NURBS curve. */
void rkglNURBSCurve(zNURBS3D *nurbs)
{
  int i;
  zVec3D vert;
  double u;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glPushName( -1 );
  glBegin( GL_LINE_STRIP );
  for( i=0; i<=zNURBS3D1Slice(nurbs); i++ ){
    u = zNURBS3D1KnotSlice( nurbs, i );
    zNURBS3D1Vec( nurbs, u, &vert );
    rkglVertex( &vert );
  }
  glEnd();
  glPopName();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D NURBS curve with conrol points. */
void rkglNURBSCurveCP(zNURBS3D *nurbs)
{
  int i;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glPushName( 0 );
  for( i=0; i<zNURBS3D1CPNum(nurbs); i++ ){
    glLoadName( i );
    glBegin( GL_POINTS );
      rkglVertex( zNURBS3D1CP(nurbs,i) );
    glEnd();
  }
  glPopName();
  glPushName( -1 );
  for( i=0; i<zNURBS3D1CPNum(nurbs); i++ ){
    if( i > 0 ){
      glBegin( GL_LINES );
      rkglVertex( zNURBS3D1CP(nurbs,i) );
      rkglVertex( zNURBS3D1CP(nurbs,i-1) );
      glEnd();
    }
  }
  glPopName();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a polyhedron. */
void rkglPH(zPH3D *ph, ubyte disptype)
{
  int i;

  if( disptype & RKGL_FACE ){
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    for( i=0; i<zPH3DFaceNum(ph); i++ )
      rkglTriFace( zPH3DFace(ph,i) );
  }
  if( disptype & RKGL_WIREFRAME ){
    bool lighting_is_enabled;
    rkglSaveLighting( &lighting_is_enabled );
    for( i=0; i<zPH3DFaceNum(ph); i++ )
      rkglTriWireframe( zPH3DFace(ph,i) );
    rkglLoadLighting( lighting_is_enabled );
  }
}
static void _rkglShapePH(void *ph, ubyte disptype){ rkglPH( (zPH3D *)ph, disptype ); }

/* draw a polyhedron with texture. */
void rkglPHTexture(zPH3D *ph, zOpticalInfo *oi, zTexture *texture)
{
  int i;

  rkglTextureBind( texture );
  glEnable( GL_POLYGON_OFFSET_FILL );
  rkglMaterial( oi );
  glEnable( GL_TEXTURE_2D );
  rkglTextureOffset();
  for( i=0; i<zPH3DFaceNum(ph); i++ )
    rkglTriTexture( zPH3DFace(ph,i), zTextureFace(texture,i) );
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_POLYGON_OFFSET_FILL );
  rkglTextureUnbind();
}

/* draw a polyhedron with bump map. */
void rkglPHBump(zPH3D *ph, zTexture *bump, rkglLight *light)
{
  zVec3D lp;
  int i;

  glEnable( GL_BLEND );
  glBlendFunc( GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR );
  glEnable( GL_POLYGON_OFFSET_FILL );
  /* normal map */
  glActiveTexture( GL_TEXTURE0 );
  glEnable( GL_TEXTURE_2D );
  rkglTextureBind( bump );
  /* normalized light map */
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, bump->id_bump );
  rkglTextureOffsetBump();

  glEnable( GL_TEXTURE_CUBE_MAP );
  _zVec3DCreate( &lp, light->pos[0], light->pos[1], light->pos[2] );
  for( i=0; i<zPH3DFaceNum(ph); i++ )
    rkglTriBump( zPH3DFace(ph,i), zTextureFace(bump,i), &lp );
  glDisable( GL_TEXTURE_CUBE_MAP );
  glDisable( GL_POLYGON_OFFSET_FILL );
  glDisable( GL_BLEND );

  glActiveTexture( GL_TEXTURE0 );
  glDisable( GL_TEXTURE_2D );
  rkglTextureUnbind();
}

/* draw a 3D shape. */
void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light)
{
  struct{
    const char *typestr;
    void (* draw)(void*,ubyte);
  } shapelist[] = {
    { "box", _rkglShapeBox },
    { "sphere", _rkglShapeSphere },
    { "ellipsoid", _rkglShapeEllips },
    { "cylinder", _rkglShapeCyl },
    { "capsule", _rkglShapeCapsule },
    { "ellipticcylinder", _rkglShapeECyl },
    { "cone", _rkglShapeCone },
    { "polyhedron", _rkglShapePH },
    { "nurbs", _rkglShapeNURBS },
    { NULL, NULL },
  };
  int i;

  if( zShape3DTexture(s) && zShape3DTexture(s)->id != 0 && disptype & RKGL_FACE && strcmp( s->com->typestr, "polyhedron" ) == 0 ){
    if( zShape3DTexture(s)->type == ZTEXTURE_BUMP )
      rkglPHBump( (zPH3D*)s->body, zShape3DTexture(s), light );
    else
      rkglPHTexture( (zPH3D*)s->body, zShape3DOptic(s), zShape3DTexture(s) );
    return;
  }

  if( oi_alt ){
    rkglMaterial( oi_alt );
  } else
  if( zShape3DOptic(s) )
    rkglMaterial( zShape3DOptic(s) );
  for( i=0; shapelist[i].typestr; i++ )
    if( strcmp( s->com->typestr, shapelist[i].typestr ) == 0 ){
      shapelist[i].draw( s->body, disptype );
      break;
    }
  if( disptype & RKGL_BB ){
    zBox3D box;
    zVec3DData data;
    zVec3DDataAssignArray( &data, zPH3DVertArray(zShape3DPH(s)) );
    zVec3DDataOBB( &data, &box );
    zVec3DDataDestroy( &data );
    rkglBox( &box, disptype );
  }
}

/* entry a 3D shape to the display list. */
int rkglEntryShape(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light)
{
  int result;

  if( s == NULL ) return -1;
  result = rkglBeginList();
  rkglShape( s, oi_alt, disptype, light );
  glEndList();
  return result;
}

/* draw multiple 3D shapes. */
void rkglMShape(zMShape3D *s, ubyte disptype, rkglLight *light)
{
  int i;

  for( i=0; i<zMShape3DShapeNum(s); i++ )
    rkglShape( zMShape3DShape(s,i), NULL, disptype, light );
}

/* entry multiple 3D shapes to the display list. */
int rkglEntryMShape(zMShape3D *s, ubyte disptype, rkglLight *light)
{
  int result;

  result = rkglBeginList();
  rkglMShape( s, disptype, light );
  glEndList();
  return result;
}

/* draw 3D pointcloud. */
void rkglPointCloud(zVec3DData *data)
{
  zVec3D *v;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glBegin( GL_POINTS );
  zVec3DDataRewind( data );
  while( ( v = zVec3DDataFetch( data ) ) ){
    rkglVertex( v );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw 3D pointcloud with estimated normal vectors. */
void rkglPointCloudNormal(zVec3DData *pointdata, zVec3DData *normaldata, double length)
{
  zVec3D *v, *n, p;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  zVec3DDataRewind( pointdata );
  zVec3DDataRewind( normaldata );
  glBegin( GL_LINES );
  while( ( v = zVec3DDataFetch( pointdata ) ) && ( n = zVec3DDataFetch( normaldata ) ) ){
    rkglVertex( v );
    zVec3DCat( v, length, n, &p );
    rkglVertex( &p );
  }
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D ellipsoid represented by a barycenter and a variance-covariane matrix. */
void rkglEllipsBaryCov(const zVec3D *center, const zMat3D *cov)
{
  zEllips3D ellips;
  zVec3D eigval;
  zMat3D eigbase;

  zMat3DSymEig( cov, &eigval, &eigbase );
  zEllips3DCreate( &ellips, center, &eigbase.b.x, &eigbase.b.y, &eigbase.b.z, 2*sqrt(eigval.c.x), 2*sqrt(eigval.c.y), 2*sqrt(eigval.c.z), 0 );
  rkglEllips( &ellips, RKGL_FACE );
}

/* draw a 3D octant of an octree. */
static void _rkglOctant(zVec3DOctant *octant)
{
  bool have_suboctant = false;
  int i;

  for( i=0; i<8; i++ ){
    if( octant->suboctant[i] ){
      have_suboctant = true;
      _rkglOctant( octant->suboctant[i] );
    }
  }
  if( !have_suboctant )
    rkglAABox( &octant->region, RKGL_FACE );
}

/* draw an octree. */
void rkglOctree(zVec3DOctree *octree)
{
  _rkglOctant( &octree->root );
}

/* draw points in a 3D octant of an octree. */
static void _rkglOctantPoints(zVec3DOctant *octant)
{
  bool have_suboctant = false;
  zVec3DListCell *cp;
  int i;

  for( i=0; i<8; i++ ){
    if( octant->suboctant[i] ){
      have_suboctant = true;
      _rkglOctantPoints( octant->suboctant[i] );
    }
  }
  if( !have_suboctant ){
    zListForEach( &octant->points, cp )
      rkglVertex( &cp->data );
  }
}

/* draw points in an octree. */
void rkglOctreePoints(zVec3DOctree *octree)
{
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glBegin( GL_POINTS );
  _rkglOctantPoints( &octree->root );
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw normal vectors of a 3D octant of an octree. */
static void _rkglOctantNormal(zVec3DOctant *octant, double length)
{
  bool have_suboctant = false;
  int i;

  for( i=0; i<8; i++ ){
    if( octant->suboctant[i] ){
      have_suboctant = true;
      _rkglOctantNormal( octant->suboctant[i], length );
    }
  }
  if( !have_suboctant ){
    zVec3D p;
    rkglVertex( &octant->center );
    zVec3DCat( &octant->center, length, &octant->_norm, &p );
    rkglVertex( &p );
  }
}

/* draw normal vectors of an octree. */
void rkglOctreeNormal(zVec3DOctree *octree, double length)
{
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  glBegin( GL_LINES );
  _rkglOctantNormal( &octree->root, length );
  glEnd();
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D arrow. */
void rkglArrow(zVec3D *bot, zVec3D *vec, double mag)
{
  zCyl3D cyl;
  zCone3D cone;
  zVec3D tip, neck;

  zVec3DAdd( bot, vec, &tip );
  if( !zVec3DNormalize( vec, &neck ) ) return;
  zVec3DMulDRC( &neck,-RKGL_ARROW_TIP_LEN * mag );
  zVec3DAddDRC( &neck, &tip );
  zCyl3DCreate( &cyl, bot, &neck, RKGL_ARROW_BOTTOM_RAD * mag, RKGL_ARROW_DIV );
  zCone3DCreate( &cone, &neck, &tip, RKGL_ARROW_NECK_RAD * mag, RKGL_ARROW_DIV );

  rkglCyl( &cyl, RKGL_FACE );
  rkglCone( &cone, RKGL_FACE );
}

/* draw a 3D coordinate frame. */
void rkglFrame(zFrame3D *f, double length)
{
  bool lighting_is_enabled;
  zVec3D *e1, *e2, p, pf;
  int i;
  GLfloat current_color[4];
  zRGB color[] = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 } };

  rkglSaveLighting( &lighting_is_enabled );
  glGetFloatv( GL_CURRENT_COLOR, current_color );
  glBegin( GL_LINES );
  for( i=0; i<3; i++ ){
    rkglRGB( &color[i] );
    e1 = zFrame3DVec(f, zX+i     );
    e2 = zFrame3DVec(f,(zX+i+1)%3);
    zVec3DCat( zFrame3DPos(f), length, e1, &p );
    rkglVertex( zFrame3DPos(f) );
    rkglVertex( &p );
    zVec3DCat( &p,-RKGL_FRAME_HEAD_LENGTH_RATIO*length, e1, &pf );
    zVec3DCatDRC( &pf, RKGL_FRAME_HEAD_APATURE_RATIO*length, e2 );
    rkglVertex( &p );
    rkglVertex( &pf );
    zVec3DCatDRC( &pf,-2*RKGL_FRAME_HEAD_APATURE_RATIO*length, e2 );
    rkglVertex( &p );
    rkglVertex( &pf );
  }
  glEnd();
  glColor3fv( current_color );
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D coordinate axis. */
void rkglAxis(zAxis axis, double length)
{
  zEdge3D edge;
  zVec3D e1, e2;
  bool lighting_is_enabled;

  rkglSaveLighting( &lighting_is_enabled );
  zVec3DZero( &e1 ); e1.e[(int)axis] = length;
  zVec3DZero( &e2 ); e2.e[(int)axis] =-length;
  zEdge3DCreate( &edge, &e1, &e2 );

  rkglEdge( &edge );
  rkglLoadLighting( lighting_is_enabled );
}

/* draw 3D wireframe gauges. */
void rkglGauge(zAxis axis1, double span1, zAxis axis2, double span2, double step)
{
  zEdge3D edge;
  zVec3D e1, e2;
  double d;
  bool lighting_is_enabled;

  zVec3DZero( &e1 );
  zVec3DZero( &e2 );
  zEdge3DCreate( &edge, &e1, &e2 );

  rkglSaveLighting( &lighting_is_enabled );
  zEdge3DVert(&edge,0)->e[(int)axis2] = span2;
  zEdge3DVert(&edge,1)->e[(int)axis2] =-span2;
  for( d=-span1; d<=span1; d+=step ){
    zEdge3DVert(&edge,0)->e[(int)axis1] = d;
    zEdge3DVert(&edge,1)->e[(int)axis1] = d;
    zEdge3DCalcVec( &edge );
    rkglEdge( &edge );
  }
  zEdge3DVert(&edge,0)->e[(int)axis1] = span1;
  zEdge3DVert(&edge,1)->e[(int)axis1] =-span1;
  for( d=-span2; d<=span2; d+=step ){
    zEdge3DVert(&edge,0)->e[(int)axis2] = d;
    zEdge3DVert(&edge,1)->e[(int)axis2] = d;
    zEdge3DCalcVec( &edge );
    rkglEdge( &edge );
  }
  rkglLoadLighting( lighting_is_enabled );
}

/* draw a 3D checker board. */
void rkglCheckerBoard(zVec3D *pc0, zVec3D *pc1, zVec3D *pc2, int div1, int div2, zOpticalInfo *oi1, zOpticalInfo *oi2)
{
  int i, j;
  zVec3D d1, d2, d11, d12, d21, d22, n, v[4];

  zVec3DSub( pc1, pc0, &d1 );
  zVec3DSub( pc2, pc0, &d2 );
  zVec3DOuterProd( &d1, &d2, &n );
  zVec3DNormalizeDRC( &n );

  zVec3DZero( &d12 );
  for( i=1; i<=div1; i++ ){
    zVec3DCopy( &d12, &d11 );
    zVec3DMul( &d1, (double)i/div1, &d12 );
    zVec3DZero( &d22 );
    for( j=1; j<=div2; j++ ){
      zVec3DCopy( &d22, &d21 );
      zVec3DMul( &d2, (double)j/div2, &d22 );
      zVec3DAdd( &d11, &d21, &v[0] ); zVec3DAddDRC( &v[0], pc0 );
      zVec3DAdd( &d12, &d21, &v[1] ); zVec3DAddDRC( &v[1], pc0 );
      zVec3DAdd( &d12, &d22, &v[2] ); zVec3DAddDRC( &v[2], pc0 );
      zVec3DAdd( &d11, &d22, &v[3] ); zVec3DAddDRC( &v[3], pc0 );
      rkglMaterial( ( i + j ) % 2 == 0 ? oi1 : oi2 );
      glBegin( GL_TRIANGLE_STRIP );
      rkglNormal( &n );
      rkglVertex( &v[0] );
      rkglVertex( &v[1] );
      rkglVertex( &v[3] );
      rkglVertex( &v[2] );
      glEnd();
    }
  }
}
