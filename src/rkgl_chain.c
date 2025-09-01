/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_chain - visualization of kinematic chain.
 */

#include <roki_gl/rkgl_chain.h>

void rkglChainAttrInit(rkglChainAttr *attr)
{
  attr->disptype            = RKGL_FACE;
  attr->wireframe_linewidth = RKGL_LINK_DEFAULT_WIREFRAME_LINEWIDTH;
  attr->bone_radius         = RKGL_LINK_DEFAULT_BONE_RADIUS;
  attr->link_com_radius     = RKGL_LINK_DEFAULT_LINK_COM_RADIUS;
  attr->com_radius          = RKGL_LINK_DEFAULT_COM_RADIUS;
  attr->ellips_scale        = RKGL_LINK_DEFAULT_ELLIPS_SCALE;
  attr->frame_length        = RKGL_LINK_DEFAULT_FRAME_LENGTH;
  attr->frame_linewidth     = RKGL_LINK_DEFAULT_FRAME_LINEWIDTH;
}

void rkglChainAttrCopy(rkglChainAttr *src, rkglChainAttr *dest)
{
  if( !dest ) return;
  if( !src )
    rkglChainAttrInit( dest );
  else
    memcpy( dest, src, sizeof(rkglChainAttr) );
}

static void _rkglLinkStick(rkLink *link, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  zVec3D z1, z2;
  zBox3D jb;
  zCyl3D jc;
  zCyl3D bone;
  zSphere3D js;
  rkLink *child;

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  if( rkLinkJoint(link)->com == &rk_joint_revol ){
    zVec3DCreate( &z1, 0, 0,-attr->bone_radius * 4 );
    zVec3DCreate( &z2, 0, 0, attr->bone_radius * 4 );
    zCyl3DCreate( &jc, &z1, &z2, attr->bone_radius*2, 0 );
    rkglCyl( &jc, RKGL_FACE );
  } else
  if( rkLinkJoint(link)->com == &rk_joint_prism ){
    zBox3DCreateAlign( &jb, ZVEC3DZERO, attr->bone_radius*4, attr->bone_radius*4, attr->bone_radius*8 );
    rkglBox( &jb, RKGL_FACE );
  } else
  if( rkLinkJoint(link)->com == &rk_joint_spher ){
    zSphere3DCreate( &js, ZVEC3DZERO, attr->bone_radius*3, 0 );
    rkglSphere( &js, RKGL_FACE );
  }
  for( child=rkLinkChild(link); child; child=rkLinkSibl(child) ){
    zCyl3DCreate( &bone, ZVEC3DZERO, rkLinkAdjPos(child), attr->bone_radius, 0 );
    rkglCyl( &bone, RKGL_FACE );
  }
}

static void _rkglLinkCOM(rkLink *link, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  zSphere3D com;

  zOpticalInfoCreateSimple( &oi, 0.4, 0.7, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  zSphere3DCreate( &com, rkLinkCOM(link), attr->link_com_radius, 0 );
  rkglSphere( &com, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
}

static void _rkglLinkInertiaEllips(rkLink *l, rkglChainAttr *attr)
{
  zOpticalInfo oi;
  zEllips3D e;

  zOpticalInfoCreateSimple( &oi, 1.0, 0.8, 0.4, NULL );
  rkglMaterialOpticalInfo( &oi );
  rkLinkInertiaEllips( l, &e );
  zEllips3DRadius(&e,0) *= attr->ellips_scale;
  zEllips3DRadius(&e,1) *= attr->ellips_scale;
  zEllips3DRadius(&e,2) *= attr->ellips_scale;
  rkglEllips( &e, RKGL_FACE );
}

static void _rkglLink(rkLink *link, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light)
{
  zShapeListCell *sp;

  if( attr->disptype & RKGL_STICK  ) _rkglLinkStick( link, attr );
  if( attr->disptype & RKGL_FRAME  ){
    glLineWidth( attr->frame_linewidth );
    rkglFrame( ZFRAME3DIDENT, attr->frame_length );
  }
  if( attr->disptype & RKGL_COM    ) _rkglLinkCOM( link, attr );
  if( attr->disptype & RKGL_ELLIPS ) _rkglLinkInertiaEllips( link, attr );
  glLineWidth( attr->wireframe_linewidth );
  if( !rkLinkShapeIsEmpty( link ) && attr->disptype & ( RKGL_FACE | RKGL_WIREFRAME | RKGL_BB ) )
    zListForEach( rkLinkShapeList(link), sp )
      rkglShape( zShapeListCellShape(sp), oi_alt, attr->disptype, light );
}

static int _rkglEntryLink(rkLink *link, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light)
{
  int result;

  result = rkglBeginList();
  _rkglLink( link, oi_alt, attr, light );
  glEndList();
  return result;
}

#define _rkglLinkInfoInit(linkinfo,entry) do{ \
  (linkinfo)->list = entry; \
  (linkinfo)->visible = ( (linkinfo)->list >= 0 ) ? true : false; \
  (linkinfo)->_list_backup = -1; \
  (linkinfo)->_optic_alt = NULL; \
} while(0)

#define _rkglLinkInfoDestroy(linkinfo) do{ \
  if( (linkinfo)->list >= 0 )         glDeleteLists( (linkinfo)->list, 1 ); \
  if( (linkinfo)->_list_backup >= 0 ) glDeleteLists( (linkinfo)->_list_backup, 1 ); \
  if( (linkinfo)->_optic_alt )        free( (linkinfo)->_optic_alt ); \
} while(0)

bool rkglChainLoad(rkglChain *gc, rkChain *chain, rkglChainAttr *attr, rkglLight *light)
{
  int i;

  attr ? rkglChainAttrCopy( attr, &gc->attr ) : rkglChainAttrInit( &gc->attr );
  gc->chain = chain;
  if( !( gc->linkinfo = zAlloc( rkglLinkInfo, rkChainLinkNum(gc->chain) ) ) ){
    ZALLOCERROR();
    return false;
  }
  for( i=0; i<rkChainLinkNum(gc->chain); i++ )
    _rkglLinkInfoInit( &gc->linkinfo[i], _rkglEntryLink( rkChainLink(gc->chain,i), NULL, &gc->attr, light ) );
  return true;
}

void rkglChainUnload(rkglChain *gc)
{
  int i;

  for( i=0; i<rkChainLinkNum(gc->chain); i++ )
    _rkglLinkInfoDestroy( &gc->linkinfo[i] );
  zFree( gc->linkinfo );
  gc->chain = NULL;
}

bool rkglChainAlternateLinkOptic(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglLight *light)
{
  if( !oi_alt ) return true; /* nothing to be done */
  if( gc->linkinfo[id]._optic_alt )
    zFree( gc->linkinfo[id]._optic_alt ); /* expire the previously saved information */
  if( !( gc->linkinfo[id]._optic_alt = zAlloc( zOpticalInfo, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  zOpticalInfoCopy( oi_alt, gc->linkinfo[id]._optic_alt );
  if( gc->linkinfo[id]._list_backup >= 0 ) /* original optical info already backed-up */
    glDeleteLists( gc->linkinfo[id].list, 1 );
  else
    gc->linkinfo[id]._list_backup = gc->linkinfo[id].list;

  gc->linkinfo[id].list = _rkglEntryLink( rkChainLink(gc->chain,id), oi_alt, &gc->attr, light );
  return true;
}

void rkglChainResetLinkOptic(rkglChain *gc, int id)
{
  int alt;

  if( gc->linkinfo[id]._list_backup >= 0 ){
    alt = gc->linkinfo[id].list; /* to avoid critical section */
    gc->linkinfo[id].list = gc->linkinfo[id]._list_backup;
    glDeleteLists( alt, 1 );
    gc->linkinfo[id]._list_backup = -1;
    if( gc->linkinfo[id]._optic_alt )
      zFree( gc->linkinfo[id]._optic_alt ); /* expire the previously saved information */
  }
}

static void _rkglChainDrawLink(rkglChain *gc, int id)
{
  if( !gc->linkinfo[id].visible ) return;
  glPushMatrix();
  rkglXform( rkChainLinkWldFrame(gc->chain,id) );
  glCallList( gc->linkinfo[id].list );
  glPopMatrix();
}

static void _rkglChainPhantomizeLink(rkglChain *gc, int id, double alpha, rkglLight *light)
{
  zShapeListCell *sp;
  zOpticalInfo oi;
  rkLink *link;

  link = rkChainLink(gc->chain,id);
  if( !gc->linkinfo[id].visible || rkLinkShapeIsEmpty( link ) ) return;
  glPushMatrix();
  rkglXform( rkLinkWldFrame(link) );
  zListForEach( rkLinkShapeList(link), sp ){
    zOpticalInfoCopy( gc->linkinfo[id]._optic_alt ?
      gc->linkinfo[id]._optic_alt : zShape3DOptic(zShapeListCellShape(sp)), &oi );
    oi.alpha = alpha;
    rkglShape( zShapeListCellShape(sp), &oi, RKGL_FACE, light );
  }
  glPopMatrix();
}

void rkglChainSetName(rkglChain *gc, GLuint name)
{
  gc->name = name;
  glLoadName( name );
}

void rkglChainDraw(rkglChain *gc)
{
  int i;

  glPushName( 0 );
  for( i=0; i<rkChainLinkNum(gc->chain); i++ ){
    glLoadName( i );
    _rkglChainDrawLink( gc, i );
  }
  glPopName();
}

void rkglChainPhantomize(rkglChain *gc, double alpha, rkglLight *light)
{
  int i;

  for( i=0; i<rkChainLinkNum(gc->chain); i++ )
    _rkglChainPhantomizeLink( gc, i, alpha, light );
}

void rkglChainDrawCOM(rkglChain *gc)
{
  zOpticalInfo oi;
  zSphere3D com;

  zOpticalInfoCreateSimple( &oi, 0.0, 0.0, 1.0, NULL );
  rkglMaterialOpticalInfo( &oi );
  zSphere3DCreate( &com, rkChainWldCOM(gc->chain), gc->attr.com_radius, 0 );
  rkglSphere( &com, RKGL_FACE );
}

int rkglChainLinkFindSelected(rkglChain *gc, rkglSelectionBuffer *sb)
{
  return ( rkglSelectionName(sb,0) == gc->name &&
           rkglSelectionName(sb,1) >= 0 &&
           rkglSelectionName(sb,1) < (uint)rkChainLinkNum(gc->chain) ) ?
    rkglSelectionName(sb,1) : -1;
}
