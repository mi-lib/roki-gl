/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_misc - miscellanies.
 */

#ifndef __RKGL_MISC_H__
#define __RKGL_MISC_H__

#include <zeda/zeda.h>

#ifdef __ROKI_GL_USE_GLEW
#include <GL/glew.h>
#endif /* __ROKI_GL_USE_GLEW */

#include <GL/gl.h>
#include <GL/glu.h>

__BEGIN_DECLS

__EXPORT void rkglEnableDefault(void);

#define rkglClear() glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )
#define rkglClearColor(r,g,b) do{\
  glClearColor( r, g, b, 1.0 );\
  rkglClear();\
} while(0)

/* matrix operation */

/* why does not OpenGL have glMultInvMatrixd() ? */
__EXPORT void rkglInvTranslated(double m[], double *x, double *y, double *z);
__EXPORT void rkglMultInvMatrixd(double m[]);

/* display list */

__EXPORT int rkglBeginList(void);
__EXPORT void rkglDeleteList(int id);

/* GLEW */

#ifdef __ROKI_GL_USE_GLEW
__EXPORT bool rkglInitGLEW(void);
#endif /* __ROKI_GL_USE_GLEW */

__END_DECLS

#endif /* __RKGL_MISC_H__ */
