/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture mapping.
 */

#ifndef __RKGL_TEXTURE_H__
#define __RKGL_TEXTURE_H__

#include <roki_gl/rkgl_misc.h>
#include <zeo/zeo_render_texture.h>

__BEGIN_DECLS

__ROKI_GL_EXPORT int rkglTextureNum(void);

/* texture mode */

#define rkglTextureSetClamp() do{\
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );\
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );\
} while(0)

#define rkglTextureSetFilterLinear() do{\
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );\
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );\
} while(0)

/* texture environment mode */

#define rkglTextureSetModulate() glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE )
#define rkglTextureSetBlend()    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND )
#define rkglTextureSetCombine()  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE )
#define rkglTextureSetReplace()  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE )
#define rkglTextureSetDecal()    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL )

/* magic numbers to prevent z-fighting */

#define rkglTextureOffsetShadow() glPolygonOffset(  2.0, 1.0 )
#define rkglTextureOffset()       glPolygonOffset( -0.1, 0.0 )
#define rkglTextureOffsetBump()   glPolygonOffset( -1.0, 0.0 )

/* color texture mapping */

/*! \brief assign a 2D texture to GL. */
__ROKI_GL_EXPORT GLuint rkglTextureAssign(int width, int height, ubyte *buf);

/*! \brief initialize GL parameters for a 2D texture. */
__ROKI_GL_EXPORT GLuint rkglTextureInit(zTexture *texture, ubyte *buf);

/*! \brief read an image file and make a texture data. */
#if defined(__ROKI_GL_USE_ZX11)
__ROKI_GL_EXPORT bool rkglTextureReadFileZX11(zTexture *texture, const char *filename);
#elif defined(__ROKI_GL_USE_MAGICKWAND)
__ROKI_GL_EXPORT bool rkglTextureReadFileMagickWand(zTexture *texture, const char *filename);
#else
__ROKI_GL_EXPORT bool rkglTextureReadFileDummy(zTexture *texture, const char *filename);
#endif /* __ROKI_GL_USE_ZX11 || __ROKI_GL_USE_MAGICKWAND */

__ROKI_GL_EXPORT bool (* rkglTextureReadFile)(zTexture *, const char *);
#define rkglTextureEnable()   zTextureSetReadFunc( rkglTextureReadFile )

#define rkglTextureBind(texture) glBindTexture( GL_TEXTURE_2D, (texture)->id )
#define rkglTextureUnbind()      glBindTexture( GL_TEXTURE_2D, 0 )

#define rkglTextureDelete(t)     glDeleteTextures( 1, &(t)->id )

#define rkglCoord(coord)         glTexCoord2d( (coord)->c.x, (coord)->c.y )

/* units for multitexture */

__ROKI_GL_EXPORT void rkglTextureInitUnit(void);
__ROKI_GL_EXPORT GLint rkglTextureNewUnit(void);

#define rkglTextureBindUnit(n,id) do{\
  glActiveTexture( GL_TEXTURE0 + (n) );\
  glBindTexture( GL_TEXTURE_2D, id );\
} while(0)

#define rkglTextureAssignUnit(t,n) do{\
  glActiveTexture( GL_TEXTURE0 + (n) );\
  rkglTextureBind( (t) );\
} while(0)

/* frame buffer and render buffer for off-screan rendering */

__ROKI_GL_EXPORT GLuint rkglFramebufferAttachTexture(GLuint texid);
__ROKI_GL_EXPORT GLuint rkglFramebufferAttachRenderbuffer(int width, int height);

#define rkglTextureCopySubImage(dx,dy,sx,sy,sw,sh) glCopyTexSubImage2D( GL_TEXTURE_2D, 0, dx, dy, sx, sy, sw, sh )

/* cube mapping */

__ROKI_GL_EXPORT const GLenum rkgl_cubemap_id[];

__ROKI_GL_EXPORT void rkglTextureGenCubeMap(int width, int height);

/* projection mapping */

__ROKI_GL_EXPORT void rkglTextureGenProjectionObject(void);
__ROKI_GL_EXPORT void rkglTextureGenProjectionEye(void);
__ROKI_GL_EXPORT void rkglTextureEnableProjection(void);
__ROKI_GL_EXPORT void rkglTextureDisableProjection(void);

/* bump mapping */

/*! \brief read an image file and make a bump map. */
__ROKI_GL_EXPORT bool rkglTextureBumpReadFile(zTexture *bump, const char *filename);
#define rkglTextureBumpEnable() zTextureSetBumpReadFunc( rkglTextureBumpReadFile )

#ifdef __ROKI_GL_USE_GLEW
/* bump map using GLSL */
/* create a bump map */
__ROKI_GL_EXPORT bool rkglTextureBumpReadFileGLSL(zTexture *bump, const char *filename);
#endif /* __ROKI_GL_USE_GLEW */

__END_DECLS

#endif /* __RKGL_TEXTURE_H__ */
