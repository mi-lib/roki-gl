/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_chain - visualization of kinematic chain
 */

#ifndef __RKGL_CHAIN_H__
#define __RKGL_CHAIN_H__

#include <roki-gl/rkgl_shape.h>
#include <roki/rk_chain.h>

__BEGIN_DECLS

typedef struct{
  int disptype;
  double bone_r;
  double com_r;
  double ellips_mag;
} rkglChainAttr;

__EXPORT void rkglChainAttrInit(rkglChainAttr *attr);
__EXPORT void rkglChainAttrCopy(rkglChainAttr *src, rkglChainAttr *dest);

typedef struct{
  bool visible;
  GLint list;
  GLint list_alt; /* alternative drawing */
} rkglLinkInfo;

typedef struct{
  GLuint name;
  rkChain *chain;
  rkglLinkInfo *info;
  rkglChainAttr attr; /* display attribute */
} rkglChain;

__EXPORT bool rkglChainLoad(rkglChain *gc, rkChain *c, rkglChainAttr *attr, rkglLight *light);
__EXPORT void rkglChainUnload(rkglChain *gc);

__EXPORT void rkglLinkStick(rkLink *l, rkglChainAttr *attr);
__EXPORT void rkglLinkCOM(rkLink *l, rkglChainAttr *attr);
__EXPORT void rkglLinkInertiaEllips(rkLink *l, rkglChainAttr *attr);
__EXPORT int rkglLinkEntry(rkLink *l, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light);

__EXPORT void rkglChainLinkAlt(rkglChain *gc, int id, zOpticalInfo *oi_alt, rkglChainAttr *attr, rkglLight *light);
__EXPORT void rkglChainLinkReset(rkglChain *gc, int id);

__EXPORT void rkglChainLinkDraw(rkglChain *gc, int id);
__EXPORT void rkglChainDraw(rkglChain *gc);
__EXPORT void rkglChainNamedDraw(rkglChain *gc, GLuint name);

__EXPORT int rkglChainDrawSeethru(rkglChain *gc, double alpha, rkglLight *light);

__EXPORT void rkglChainCOMDraw(rkglChain *gc, double r);

__END_DECLS

#endif /* __RKGL_CHAIN_H__ */
