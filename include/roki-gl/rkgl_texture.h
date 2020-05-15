/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture map
 */

#ifndef __RKGL_TEXTURE_H__
#define __RKGL_TEXTURE_H__

#include <roki-gl/rkgl_misc.h>
#include <zeo/zeo_texture.h>
#include <zx11/zximage.h>

__BEGIN_DECLS

/*! \brief read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename);

#define rkglCoord(coord)      glTexCoord2d( (coord)->c.x, (coord)->c.y )

#define rkglTextureCoord(t,i) rkglCoord( zTextureCoord(t,i) )

__END_DECLS

#endif /* __RKGL_TEXTURE_H__ */
