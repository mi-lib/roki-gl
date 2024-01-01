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

static bool _rkglFrameHandleIsInTranslation(rkglFrameHandle *handle)
{
  return handle->selected_id >=0 && handle->selected_id <= 2;
}

static bool _rkglFrameHandleIsInRotation(rkglFrameHandle *handle)
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

void rkglFrameHandleSelect(rkglFrameHandle *handle, rkglCamera *cam, int x, int y, void (* draw_func)(void))
{
  GLuint selbuf[BUFSIZ];
  GLuint *ns;

  rkglFrameHandleUnselect( handle );
  if( !( ns = rkglFindNearside( selbuf,
                rkglPick( cam, draw_func, selbuf, BUFSIZ, x, y, 1, 1 ) ) ) ) return;
  if( ns[3] != handle->name || ns[4] < 0 || ns[4] >= 6 ) return;
  handle->selected_id = ns[4];
  handle->depth = rkglGetDepth( cam, x, y );
  rkglUnproject( cam, x, y, handle->depth, &handle->anchor );
}

static void _rkglFrameHandleTranslate(rkglFrameHandle *handle, rkglCamera *cam, zVec3D *v)
{
  zVec3D *a, d, u;
  int px, py;

  a = zFrame3DVec(&handle->frame,handle->selected_id%3);
  zVec3DSub( v, &handle->anchor, &d );
  zVec3DAdd( &handle->anchor, a, &u );
  rkglProject( cam, &u, &px, &py );
  rkglUnproject( cam, px, py, handle->depth, &u );
  zVec3DSubDRC( &u, &handle->anchor );
  zVec3DCatDRC( zFrame3DPos(&handle->frame), zVec3DInnerProd(&d,&u)/zVec3DInnerProd(a,&u), a );
}

static void _rkglFrameHandleRotate(rkglFrameHandle *handle, rkglCamera *cam /* dummy */, zVec3D *v)
{
  zVec3D *a, tmp, r0, r1, aa;

  a = zFrame3DVec(&handle->frame,handle->selected_id%3);
  zVec3DSub( &handle->anchor, zFrame3DPos(&handle->frame), &tmp );
  zVec3DOrthogonalize( &tmp, a, &r0 );
  zVec3DSub( v, zFrame3DPos(&handle->frame), &tmp );
  zVec3DOrthogonalize( &tmp, a, &r1 );
  zVec3DAAError( &r0, &r1, &aa );
  zMat3DRotDRC( zFrame3DAtt(&handle->frame), &aa );
}

bool rkglFrameHandleMove(rkglFrameHandle *handle, rkglCamera *cam, int x, int y)
{
  zVec3D v;

  if( rkglFrameHandleIsUnselected( handle ) ) return false;
  rkglUnproject( cam, x, y, handle->depth, &v );
  if( _rkglFrameHandleIsInTranslation( handle ) ){
    _rkglFrameHandleTranslate( handle, cam, &v );
  } else
  if( _rkglFrameHandleIsInRotation( handle ) ){
    _rkglFrameHandleRotate( handle, cam, &v );
  }
  zVec3DCopy( &v, &handle->anchor );
  return true;
}
