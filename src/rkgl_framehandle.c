/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_framehandle - handle to manipulate objects
 */

#include <roki_gl/rkgl_framehandle.h>

static void _rkglFrameHandleSetAxisMaterial(zAxis axis)
{
  zRGB color;

  switch( axis ){
  case zX: zRGBSet( &color, 1.0, 0.0, 0.0 ); break; /* red */
  case zY: zRGBSet( &color, 0.0, 1.0, 0.0 ); break; /* green */
  case zZ: zRGBSet( &color, 0.0, 0.0, 1.0 ); break; /* blue */
  default: zRGBSet( &color, 1.0, 1.0, 0.0 );        /* yellow */
  }
  rkglMaterialRGBA( &color, 0.5 );
}

static void _rkglFrameHandleDrawArrowPart(zFrame3D *f, zAxis axis, double l, double mag)
{
  zVec3D v, vb;

  zVec3DMul( &zFrame3DAtt(f)->v[axis], 0.5*l, &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
  zVec3DRevDRC( &v );
  zVec3DAdd( zFrame3DPos(f), &v, &vb );
  rkglArrow( &vb, &v, mag );
}

static void _rkglFrameHandleDrawRingPart(zFrame3D *f, zAxis axis, double l, double mag)
{
  zCyl3D ring;
  zVec3D c1, c2;

  zVec3DCat( zFrame3DPos(f), RKGL_ARROW_BOTTOM_RAD*mag*4, &zFrame3DAtt(f)->v[axis], &c1 );
  zVec3DCat( zFrame3DPos(f),-RKGL_ARROW_BOTTOM_RAD*mag*4, &zFrame3DAtt(f)->v[axis], &c2 );
  zCyl3DCreate( &ring, &c1, &c2, l*0.5, 16 );
  rkglTube( &ring, RKGL_FACE );
}

bool rkglFrameHandleIsInTranslation(rkglFrameHandle *handle)
{
  return handle->selected_id >=0 && handle->selected_id <= 2;
}

bool rkglFrameHandleIsInRotation(rkglFrameHandle *handle)
{
  return handle->selected_id >=3 && handle->selected_id <= 5;
}

static int _rkglFrameHandleCreatePart(zFrame3D *frame, zAxis axis, double l, double mag, void (*draw_part)(zFrame3D*,zAxis,double,double))
{
  int id;

  id = glGenLists( 1 );
  glNewList( id, GL_COMPILE );
  draw_part( frame, axis, l, mag );
  glEndList();
  return id;
}

void rkglFrameHandleCreate(rkglFrameHandle *handle, int name, double l, double mag)
{
  int i;

  handle->name = name;
  zFrame3DIdent( &handle->frame );
  for( i=0; i<6; i++ )
    handle->part[i] = _rkglFrameHandleCreatePart( &handle->frame, zX + i % 3, l, mag,
      i < 3 ? _rkglFrameHandleDrawArrowPart : _rkglFrameHandleDrawRingPart );
  rkglFrameHandleUnselect( handle );
}

void rkglFrameHandleDestroy(rkglFrameHandle *handle)
{
  int i;

  for( i=0; i<6; i++ )
    if( glIsList( handle->part[i] ) )
      glDeleteLists( handle->part[i], 1 );
}

static void _rkglFrameHandleDrawPart(rkglFrameHandle *handle, int i, zAxis axis)
{
  glLoadName( i );
  _rkglFrameHandleSetAxisMaterial( handle->selected_id == i ? zAxisInvalid : axis );
  glCallList( handle->part[i] );
}

void rkglFrameHandleDraw(rkglFrameHandle *handle)
{
  int i;

  glPushMatrix();
  rkglXform( &handle->frame );
  glLoadName( handle->name );
  glPushName( 0 );
  for( i=0; i<6; i++ )
    _rkglFrameHandleDrawPart( handle, i, zX + i % 3 );
  glPopName();
  glPopMatrix();
}

void rkglFrameHandleSelect(rkglFrameHandle *handle, rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y)
{
  rkglFrameHandleUnselect( handle );
  if( !rkglSelectionFindNearest( sb ) ) return;
  if( rkglSelectionName(sb,0) != handle->name ||
      rkglSelectionName(sb,1) < 0 || rkglSelectionName(sb,1) >= 6 ) return;
  handle->selected_id = rkglSelectionName(sb,1);
  handle->_depth = rkglSelectionZnearDepth(sb);
  rkglUnproject( cam, x, y, handle->_depth, &handle->_anchor );
}

static void _rkglFrameHandleTranslate(rkglFrameHandle *handle, rkglCamera *cam, zVec3D *v)
{
  zVec3D *a, d, u;
  int px, py;

  a = zFrame3DVec(&handle->frame,handle->selected_id%3);
  zVec3DSub( v, &handle->_anchor, &d );
  zVec3DAdd( &handle->_anchor, a, &u );
  rkglProject( cam, &u, &px, &py );
  rkglUnproject( cam, px, py, handle->_depth, &u );
  zVec3DSubDRC( &u, &handle->_anchor );
  zVec3DCatDRC( rkglFrameHandlePos(handle), zVec3DInnerProd(&d,&u)/zVec3DInnerProd(a,&u), a );
  zVec3DCopy( v, &handle->_anchor );
}

static void _rkglFrameHandleRotate(rkglFrameHandle *handle, rkglCamera *cam /* dummy */, zVec3D *v)
{
  zVec3D *a0, *a1, *a2, u, d, tmp, r0, r, aa;

  a0 = zFrame3DVec(&handle->frame,(handle->selected_id+0)%3);
  a1 = zFrame3DVec(&handle->frame,(handle->selected_id+1)%3);
  a2 = zFrame3DVec(&handle->frame,(handle->selected_id+2)%3);
  rkglCAGetViewVec( cam, &u ); /* view vector */
  zVec3DSub( &handle->_anchor, rkglFrameHandlePos(handle), &tmp );
  zVec3DOrthogonalize( &tmp, a0, &r0 ); /* anchor vector */
  zVec3DSub( v, rkglFrameHandlePos(handle), &d );
  if( zIsTiny( zVec3DInnerProd(a0,&u) ) ){
    zVec3DOrthogonalize( &d, a0, &r );
  } else{
    zVec3DMul( a1, zVec3DGrassmannProd(a2,&d,&u)/zVec3DGrassmannProd(a2,a1,&u), &tmp );
    zVec3DCatDRC( &tmp, zVec3DGrassmannProd(a1,&d,&u)/zVec3DGrassmannProd(a1,a2,&u), a2 );
    zVec3DOrthogonalize( &tmp, a0, &r );
  }
  zVec3DAAError( &r0, &r, &aa );
  zMat3DRotDRC( rkglFrameHandleAtt(handle), &aa );
  zVec3DAdd( rkglFrameHandlePos(handle), &r, &handle->_anchor );
}

bool rkglFrameHandleMove(rkglFrameHandle *handle, rkglCamera *cam, int x, int y)
{
  zVec3D v;

  if( rkglFrameHandleIsUnselected( handle ) ) return false;
  rkglUnproject( cam, x, y, handle->_depth, &v );
  if( rkglFrameHandleIsInTranslation( handle ) ){
    _rkglFrameHandleTranslate( handle, cam, &v );
  } else
  if( rkglFrameHandleIsInRotation( handle ) ){
    _rkglFrameHandleRotate( handle, cam, &v );
  }
  return true;
}
