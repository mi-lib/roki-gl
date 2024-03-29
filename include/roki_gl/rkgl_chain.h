/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_chain - visualization of kinematic chain
 */

#ifndef __RKGL_CHAIN_H__
#define __RKGL_CHAIN_H__

#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_select.h>
#include <roki/rk_chain.h>

__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglChainAttr ){
  ubyte disptype;
  double bone_r;
  double link_com_r;
  double com_r;
  double ellips_mag;
  double frame_l;
  double frame_mag;
};

__ROKI_GL_EXPORT void rkglChainAttrInit(rkglChainAttr *attr);
__ROKI_GL_EXPORT void rkglChainAttrCopy(rkglChainAttr *src, rkglChainAttr *dest);

ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglLinkInfo ){
  bool visible;
  GLint list;
  GLint _list_backup; /* for alternative drawing */
  zOpticalInfo *_optic_alt; /* alternative optical information */
};

ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglChain ){
  GLuint name;
  rkChain *chain;
  rkglLinkInfo *linkinfo;
  rkglChainAttr attr; /* display attribute */
};

#define RKGL_LINK_DEFAULT_BONE_R     0.006
#define RKGL_LINK_DEFAULT_LINK_COM_R 0.012
#define RKGL_LINK_DEFAULT_COM_R      0.03
#define RKGL_LINK_DEFAULT_ELLIPS_MAG 1.0
#define RKGL_LINK_DEFAULT_FRAME_L    0.1
#define RKGL_LINK_DEFAULT_FRAME_MAG  0.1

__ROKI_GL_EXPORT bool rkglChainLoad(rkglChain *gc, rkChain *chain, rkglChainAttr *attr, rkglLight *light);
__ROKI_GL_EXPORT void rkglChainUnload(rkglChain *gc);

__ROKI_GL_EXPORT bool rkglChainAlternateLinkOptic(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglLight *light);
__ROKI_GL_EXPORT void rkglChainResetLinkOptic(rkglChain *gc, int id);

__ROKI_GL_EXPORT void rkglChainSetName(rkglChain *gc, GLuint name);

__ROKI_GL_EXPORT void rkglChainDraw(rkglChain *gc);
__ROKI_GL_EXPORT void rkglChainPhantomize(rkglChain *gc, double alpha, rkglLight *light);

__ROKI_GL_EXPORT void rkglChainDrawCOM(rkglChain *gc);

__ROKI_GL_EXPORT int rkglChainLinkFindSelected(rkglChain *gc, rkglSelectionBuffer *sb);

__END_DECLS

#endif /* __RKGL_CHAIN_H__ */
