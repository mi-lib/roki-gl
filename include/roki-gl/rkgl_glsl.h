/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl - programmable shaders with GLSL.
 */

#ifndef __RKGL_GLSL_H__
#define __RKGL_GLSL_H__

#include <roki-gl/rkgl_misc.h>
#include <roki-gl/rkgl_optic.h>

__BEGIN_DECLS

__EXPORT GLuint rkglShaderCreateFromFile(const char *vsfile, const char *fsfile);
__EXPORT GLuint rkglShaderCreate(const char *vssource, const char *fssource);

__EXPORT void rkglShaderSetUniform1i(GLuint shader, const char *varname, int val);
__EXPORT void rkglShaderSetUniform1f(GLuint shader, const char *varname, float val);

__END_DECLS

#include <roki-gl/rkgl_glsl_phong.h>
#include <roki-gl/rkgl_glsl_spotlight.h>
#include <roki-gl/rkgl_glsl_fog.h>

#endif /* __RKGL_GLSL_H__ */
