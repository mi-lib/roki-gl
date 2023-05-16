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

__EXPORT int rkglTextureNum(void);

/* texture environment mode */

#define rkglTextureSetModulate() glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE )
#define rkglTextureSetBlend()    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND )
#define rkglTextureSetCombine()  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE )
#define rkglTextureSetReplace()  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE )
#define rkglTextureSetDecal()    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL )

/* magic numbers to prevent z-fighting */
#define rkglAntiZFighting()   glPolygonOffset( -1.1, 4.0 )

/* color texture mapping */

/*! \brief generate a 2D texture, and set related GL parameters as default. */
__EXPORT GLuint rkglTextureGen(int width, int height, ubyte *buf);

/*! \brief initialize GL parameters for a 2D texture. */
__EXPORT GLuint rkglTextureInit(zTexture *texture);

/*! \brief read an image file and make a texture data. */
__EXPORT bool rkglTextureReadFile(zTexture *texture, char *filename);

#define rkglTextureEnable()   zTextureSetReadFunc( rkglTextureReadFile )

#define rkglTextureBind(texture) glBindTexture( GL_TEXTURE_2D, (texture)->id )
#define rkglTextureUnbind()      glBindTexture( GL_TEXTURE_2D, 0 )

#define rkglCoord(coord)      glTexCoord2d( (coord)->c.x, (coord)->c.y )

/* units for multitexture */

__EXPORT void rkglTextureInitUnit(void);
__EXPORT GLint rkglTextureNewUnit(void);

#define rkglTextureAssignUnit(n,id) do{\
  glActiveTexture( GL_TEXTURE0 + (n) );\
  glBindTexture( GL_TEXTURE_2D, id );\
} while(0)

/* frame buffer and render buffer for off-screan rendering */

__EXPORT GLuint rkglFramebufferAttachTexture(GLuint texid);
__EXPORT GLuint rkglFramebufferAttachRenderbuffer(int width, int height);

#define rkglTextureCopySubImage(dx,dy,sx,sy,sw,sh) glCopyTexSubImage2D( GL_TEXTURE_2D, 0, dx, dy, sx, sy, sw, sh )

/* bump mapping */

/*! \brief read an image file and make a bump map. */
__EXPORT bool rkglTextureBumpReadFile(zTexture *bump, char *filename);

#define rkglTextureBumpEnable() zTextureSetBumpReadFunc( rkglTextureBumpReadFile )

__END_DECLS

#endif /* __RKGL_TEXTURE_H__ */
