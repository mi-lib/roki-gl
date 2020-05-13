/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture map
 */

#ifndef __RKGL_TEXTURE_H__
#define __RKGL_TEXTURE_H__

#include <roki-gl/rkgl_shape.h>
#include <zx11/zximage.h>

__BEGIN_DECLS

typedef struct{
  uint id;
  zVec3D *v[4];
  zVec3D normal;
  int width;
  int height;
  ubyte *buf;
} rkglTexture;

bool rkglTextureCreate(rkglTexture *texture, char *filename, zVec3D *v);
void rkglTextureDestroy(rkglTexture *texture);
void rkglTextureDraw(rkglTexture *texture);

__END_DECLS

#endif /* __RKGL_TEXTURE_H__ */
