/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_envmap - environment mapping (reflection/refraction/shadow).
 */

#ifndef __RKGL_ENVMAP_H__
#define __RKGL_ENVMAP_H__

#include <roki_gl/rkgl_optic.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_texture.h>
#include <roki_gl/rkgl_shape.h>

__BEGIN_DECLS

/* reflection and refraction mapping */

__ROKI_GL_EXPORT void rkglReflectionRefraction(int width, int height, rkglCamera *cam, rkglLight *light, void (* draw)(void), zVec3D *center);

/* shadow mapping */

typedef struct{
  int width;             /* texture width */
  int height;            /* texture height */
  double darkness_ratio; /* darkness ratio */
  double blur;           /* blurring distance of shadow edge */
  double radius;         /* radius of bounding sphere */
  bool antizfighting;    /* flag to enable anti-Z-fighting */
  /*! @cond */
  GLuint texid;          /* texture name */
  GLuint fb;             /* framebuffer name */
  GLuint shader_program; /* shader program in GLSL */
  double _lightview[16];
  /*! @endcond */
} rkglShadow;

__ROKI_GL_EXPORT GLuint rkglShadowInit(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur);
__ROKI_GL_EXPORT void rkglShadowDraw(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void));

#define rkglShadowEnableAntiZFighting(s)  ( (s)->antizfighting = true )
#define rkglShadowDisableAntiZFighting(s) ( (s)->antizfighting = false )

#ifdef __ROKI_GL_USE_GLEW
/* shadow map with GLSL */
#include <roki_gl/rkgl_glsl.h>
#include <roki_gl/rkgl_glsl_shadowmap.h>
#endif /* __ROKI_GL_USE_GLEW */

__END_DECLS

#endif /* __RKGL_ENVMAP_H__ */
