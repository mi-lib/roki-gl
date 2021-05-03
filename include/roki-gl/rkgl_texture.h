/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture mapping.
 */

#ifndef __RKGL_TEXTURE_H__
#define __RKGL_TEXTURE_H__

#include <roki-gl/rkgl_misc.h>
#include <zeo/zeo_render_texture.h>
#include <zx11/zximage.h>

__BEGIN_DECLS

/* texture units for rendering by multiple textures */
#define RKGL_TEXTURE_BASE   GL_TEXTURE0
#define RKGL_TEXTURE_BUMP   GL_TEXTURE1
#define RKGL_TEXTURE_SHADOW GL_TEXTURE2
#define RKGL_TEXTURE_COLOR  GL_TEXTURE3

/* magic numbers to prevent z-fighting */
#define rkglAntiZFighting()   glPolygonOffset( -1.1, 4.0 )

/* color texture mapping */

/*! \brief read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename);

#define rkglTextureEnable()   zTextureSetReadFunc( rkglTextureReadFile )

#define rkglCoord(coord)      glTexCoord2d( (coord)->c.x, (coord)->c.y )

/* bump mapping */

/*! \brief read an image file and make a bump map. */
bool rkglTextureBumpReadFile(zTexture *bump, char *filename);

#define rkglTextureBumpEnable() zTextureSetBumpReadFunc( rkglTextureBumpReadFile )

__END_DECLS

#endif /* __RKGL_TEXTURE_H__ */
