/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) ***
 *
 * roki_glfw - GLFW wrapper (on OpenGL FrameWork)
 */

#ifndef __RKGL_GLFW_H__
#define __RKGL_GLFW_H__

#include <roki_gl/roki_gl.h>
#include <GLFW/glfw3.h>

__BEGIN_DECLS

/* I don't know why, but mouse wheel actions are not defined in the original
 * freeglut (even in freeglut_ext, though it has glutMouseWheelFunc).
 */
#define GLFW_WHEEL_UP   0x0003
#define GLFW_WHEEL_DOWN 0x0004

__ROKI_GL_EXPORT int rkglInitGLFW(int *argc, char **argv);
__ROKI_GL_EXPORT int rkglWindowCreateGLFW(GLFWwindow* window, int x, int y, int w, int h, const char *title);

/* default callback functions */

__ROKI_GL_EXPORT void rkglSetCallbackParamGLFW(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl_key, double da_key);

__ROKI_GL_EXPORT void rkglReshapeFuncGLFW(int w, int h);
__ROKI_GL_EXPORT void rkglIdleFuncGLFW(void);
__ROKI_GL_EXPORT void rkglKeyFuncGLFW(unsigned char key, int x, int y);
__ROKI_GL_EXPORT void rkglSpecialFuncGLFW(GLFWwindow* window, int key, int x, int y);
__ROKI_GL_EXPORT void rkglMouseFuncGLFW(int button, int event, int x, int y);
__ROKI_GL_EXPORT void rkglMouseDragFuncGLFW(int x, int y);
__ROKI_GL_EXPORT void rkglVisFuncGLFW(GLFWwindow* window);

__END_DECLS

#endif /* __RKGL_GLFW_H__ */
