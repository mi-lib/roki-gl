/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_chain - visualization of kinematic chain
 */

#include <roki-gl/rkgl_chain.h>

void rkglChainAttrInit(rkglChainAttr *attr)
{
#define RKGL_LINK_BONE_R     0.006
#define RKGL_LINK_COM_R      0.012
#define RKGL_LINK_ELLIPS_MAG 1.0
  attr->disptype = RKGL_FACE;
  attr->bone_r = RKGL_LINK_BONE_R;
  attr->com_r = RKGL_LINK_COM_R;
  attr->ellips_mag = RKGL_LINK_ELLIPS_MAG;
}

void rkglChainAttrCopy(rkglChainAttr *src, rkglChainAttr *dest)
{
  if( !dest ) return;
  if( !src ){
    rkglChainAttrInit( dest );
    return;
  }
  dest->disptype = src->disptype;
  dest->bone_r = src->bone_r;
  dest->com_r = src->com_r;
  dest->ellips_mag = src->ellips_mag;
}

bool rkglChainLoad(rkglChain *gc, rkChain *c, rkglChainAttr *attr, rkglLight *light)
{
  uint i;

  rkglChainAttrCopy( attr, &gc->attr );
  gc->chain = c;
  if( !( gc->info = zAlloc( rkglLinkInfo, rkChainLinkNum(gc->chain) ) ) ){
    ZALLOCERROR();
    return false;
  }
  for( i=0; i<rkChainLinkNum(gc->chain); i++ ){
    gc->info[i].list = rkglLinkEntry( rkChainLink(gc->chain,i), NULL, &gc->attr, light );
    gc->info[i].list_alt = -1;
    gc->info[i].visible = ( gc->info[i].list >= 0 ) ? true : false;
  }
  return true;
}

void rkglChainUnload(rkglChain *gc)
{
  uint i;

  for( i=0; i<rkChainLinkNum(gc->chain); i++ )
    if( gc->info[i].list >= 0 )
      glDeleteLists( gc->info[i].list, 1 );
  zFree( gc->info );
  gc->chain = NULL;
}

void rkglLinkStick(rkLink *l, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  rkLink *child;
  zCyl3D bone;
  zVec3D z1, z2;
  zBox3D jb;
  zCyl3D jc;
  zSphere3D js;

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  if( rkLinkJoint(l)->com == &rk_joint_revol ){
    zVec3DCreate( &z1, 0, 0,-attr->bone_r * 4 );
    zVec3DCreate( &z2, 0, 0, attr->bone_r * 4 );
    zCyl3DCreate( &jc, &z1, &z2, attr->bone_r*2, 0 );
    rkglCyl( &jc, RKGL_FACE );
  } else
  if( rkLinkJoint(l)->com == &rk_joint_prism ){
    zBox3DCreateAlign( &jb, ZVEC3DZERO, attr->bone_r*4, attr->bone_r*4, attr->bone_r*8 );
    rkglBox( &jb, RKGL_FACE );
  } else
  if( rkLinkJoint(l)->com == &rk_joint_spher ){
    zSphere3DCreate( &js, ZVEC3DZERO, attr->bone_r*3, 0 );
    rkglSphere( &js, RKGL_FACE );
  }
  for( child=rkLinkChild(l); child; child=rkLinkSibl(child) ){
    zCyl3DCreate( &bone, ZVEC3DZERO, rkLinkAdjPos(child), attr->bone_r, 0 );
    rkglCyl( &bone, RKGL_FACE );
  }
}

void rkglLinkCOM(rkLink *l, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  rkLink *child;
  zCyl3D bone;
  zSphere3D com;
  double br;

  zOpticalInfoCreateSimple( &oi, 0.4, 0.7, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  zSphere3DCreate( &com, rkLinkCOM(l), attr->com_r, 0 );
  rkglSphere( &com, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  br = attr->com_r / 3;
  for( child=rkLinkChild(l); child; child=rkLinkSibl(child) ){
    zCyl3DCreate( &bone, ZVEC3DZERO, rkLinkAdjPos(child), br, 0 );
    rkglCyl( &bone, RKGL_FACE );
  }
}

void rkglLinkInertiaEllips(rkLink *l, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  zEllips3D e;

  zOpticalInfoCreateSimple( &oi, 1.0, 0.8, 0.4, NULL );
  rkglMaterialOpticalInfo( &oi );
  rkLinkInertiaEllips( l, &e );
  zEllips3DRadius(&e,0) *= attr->ellips_mag;
  zEllips3DRadius(&e,1) *= attr->ellips_mag;
  zEllips3DRadius(&e,2) *= attr->ellips_mag;
  rkglEllips( &e, RKGL_FACE );
}

int rkglLinkEntry(rkLink *l, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light)
{
  zShapeListCell *sp;
  int result;

  switch( attr->disptype ){
  case RKGL_STICK:
    result = rkglBeginList();
      rkglLinkStick( l, attr );
    glEndList();
    break;
  case RKGL_COM:
    result = rkglBeginList();
      rkglLinkCOM( l, attr );
    glEndList();
    break;
  case RKGL_ELLIPS:
    result = rkglBeginList();
      rkglLinkInertiaEllips( l, attr );
    glEndList();
    break;
  default:
    if( rkLinkShapeIsEmpty( l ) ) return -1;
    result = rkglBeginList();
      zListForEach( rkLinkShapeList(l), sp )
        rkglShape( zShapeListCellShape(sp), oi_alt, attr->disptype, light );
    glEndList();
  }
  return result;
}

void rkglChainLinkAlt(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light)
{
  if( gc->info[id].list_alt >= 0 )
    glDeleteLists( gc->info[id].list, 1 );
  else
    gc->info[id].list_alt = gc->info[id].list;

  gc->info[id].list = rkglLinkEntry( rkChainLink(gc->chain,id), oi_alt, attr, light );
}

void rkglChainLinkReset(rkglChain *gc, int id)
{
  int alt;

  if( gc->info[id].list_alt >= 0 ){
    alt = gc->info[id].list;
    gc->info[id].list = gc->info[id].list_alt;
    glDeleteLists( alt, 1 );
    gc->info[id].list_alt = -1;
  }
}

void rkglChainLinkDraw(rkglChain *gc, int id)
{
  if( !gc->info[id].visible ) return;
  glPushMatrix();
  rkglXform( rkChainLinkWldFrame(gc->chain,id) );
  glCallList( gc->info[id].list );
  glPopMatrix();
}

void rkglChainDraw(rkglChain *gc)
{
  uint i;

  for( i=0; i<rkChainLinkNum(gc->chain); i++ ){
    glLoadName( i );
    rkglChainLinkDraw( gc, i );
  }
}

void rkglChainNamedDraw(rkglChain *gc, GLuint name)
{
  uint i;

  gc->name = name;
  glLoadName( name );
  glPushName( 0 ); /* dummy name */
  for( i=0; i<rkChainLinkNum(gc->chain); i++ ){
    glLoadName( i );
    rkglChainLinkDraw( gc, i );
  }
  glPopName();
}

int rkglChainDrawSeethru(rkglChain *gc, double alpha, rkglLight *light)
{
  rkLink *l;
  zShapeListCell *sp;
  zOpticalInfo oi;
  int result;
  uint i;

  result = rkglBeginList();
  for( i=0; i<rkChainLinkNum(gc->chain); i++ ){
    l = rkChainLink( gc->chain , i );
    if( !gc->info[i].visible || rkLinkShapeIsEmpty(l) ) continue;
    glPushMatrix();
    rkglXform( rkLinkWldFrame(l) );
    zListForEach( rkLinkShapeList(l), sp ){
      zOpticalInfoCopy( zShape3DOptic(zShapeListCellShape(sp)), &oi );
      oi.alpha = alpha;
      rkglShape( zShapeListCellShape(sp), &oi, RKGL_FACE, light );
    }
    glPopMatrix();
  }
  glEndList();
  return result;
}

void rkglChainCOMDraw(rkglChain *gc, double r)
{
  zOpticalInfo oi;
  zSphere3D com;

  zOpticalInfoCreateSimple( &oi, 0.0, 0.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  zSphere3DCreate( &com, rkChainWldCOM(gc->chain), r, 0 );
  rkglSphere( &com, RKGL_FACE );
}
