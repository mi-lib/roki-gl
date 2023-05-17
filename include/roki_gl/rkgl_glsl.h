/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl - programmable shaders with GLSL.
 */

#ifndef __RKGL_GLSL_H__
#define __RKGL_GLSL_H__

#include <roki_gl/rkgl_misc.h>
#include <roki_gl/rkgl_optic.h>

__BEGIN_DECLS

__ROKI_GL_EXPORT GLuint rkglShaderCreateFromFile(const char *vsfile, const char *fsfile);
__ROKI_GL_EXPORT GLuint rkglShaderCreate(const char *vssource, const char *fssource);

__ROKI_GL_EXPORT void rkglShaderSetUniform1i(GLuint shader, const char *varname, int val);
__ROKI_GL_EXPORT void rkglShaderSetUniform1f(GLuint shader, const char *varname, float val);

__END_DECLS

#include <roki_gl/rkgl_glsl_phong.h>
#include <roki_gl/rkgl_glsl_spotlight.h>
#include <roki_gl/rkgl_glsl_fog.h>
#include <roki_gl/rkgl_glsl_texture.h>

#endif /* __RKGL_GLSL_H__ */
