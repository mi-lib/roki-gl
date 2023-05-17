/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glut - GLUT (OpenGL utility toolkit) wrapper
 */

#ifndef __RKGL_GLUT_H__
#define __RKGL_GLUT_H__

#include <roki_gl/rkgl_input.h>
#include <GL/glut.h>

__BEGIN_DECLS

/* I don't know why, but mouse wheel actions are not defined in the original
 * freeglut (even in freeglut_ext, though it has glutMouseWheelFunc).
 */
#define GLUT_WHEEL_UP   0x0003
#define GLUT_WHEEL_DOWN 0x0004

__ROKI_GL_EXPORT void rkglInitGLUT(int *argc, char **argv);
__ROKI_GL_EXPORT int rkglWindowCreateGLUT(int x, int y, int w, int h, const char *title);

/* default callback functions */

__ROKI_GL_EXPORT void rkglSetCallbackParamGLUT(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl_key, double da_key);

__ROKI_GL_EXPORT void rkglReshapeFuncGLUT(int w, int h);
__ROKI_GL_EXPORT void rkglIdleFuncGLUT(void);
__ROKI_GL_EXPORT void rkglKeyFuncGLUT(unsigned char key, int x, int y);
__ROKI_GL_EXPORT void rkglSpecialFuncGLUT(int key, int x, int y);
__ROKI_GL_EXPORT void rkglMouseFuncGLUT(int button, int event, int x, int y);
__ROKI_GL_EXPORT void rkglMouseDragFuncGLUT(int x, int y);
__ROKI_GL_EXPORT void rkglVisFuncGLUT(int visible);

__END_DECLS

#endif /* __RKGL_GLUT_H__ */
