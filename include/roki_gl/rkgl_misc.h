/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_misc - miscellanies.
 */

#ifndef __RKGL_MISC_H__
#define __RKGL_MISC_H__

#include <zeda/zeda.h>
#include <roki_gl/roki_gl_export.h>

#ifdef __ROKI_GL_USE_GLEW
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif /* __ROKI_GL_USE_GLEW */

#include <GL/glu.h>

__BEGIN_DECLS

__ROKI_GL_EXPORT void rkglEnableDefault(void);

#define rkglClear() glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )
#define rkglClearColor(r,g,b) do{\
  glClearColor( r, g, b, 1.0 );\
  rkglClear();\
} while(0)

/* matrix operation */

/* why does not OpenGL have glMultInvMatrixd() ? */
__ROKI_GL_EXPORT void rkglInvTranslated(double m[], double *x, double *y, double *z);
__ROKI_GL_EXPORT void rkglMultInvMatrixd(double m[]);
__ROKI_GL_EXPORT void rkglXformInvd(double m[], double p[], double px[]);

/* display list */

__ROKI_GL_EXPORT int rkglBeginList(void);
__ROKI_GL_EXPORT void rkglDeleteList(int id);

/* GLEW */

#ifdef __ROKI_GL_USE_GLEW
__ROKI_GL_EXPORT bool rkglInitGLEW(void);
#endif /* __ROKI_GL_USE_GLEW */

__END_DECLS

#endif /* __RKGL_MISC_H__ */
