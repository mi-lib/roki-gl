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

__ROKI_GL_EXPORT int rkglInitGLFW(int *argc, char **argv);
__ROKI_GL_EXPORT int rkglWindowCreateGLFW(GLFWwindow* window, int x, int y, int w, int h, const char *title);

/* default callback functions */
__ROKI_GL_EXPORT void rkglReshapeFuncGLFW(GLFWwindow* window, int w, int h);
__ROKI_GL_EXPORT void rkglCharFuncGLFW(GLFWwindow* window, unsigned int codepoint);
__ROKI_GL_EXPORT void rkglKeyFuncGLFW(GLFWwindow *window, int key, int scancode, int action, int mods);
__ROKI_GL_EXPORT void rkglSpecialFuncGLFW(GLFWwindow* window, int key, int x, int y);
__ROKI_GL_EXPORT void rkglMouseFuncGLFW(GLFWwindow* window, int button, int state, int mods);
__ROKI_GL_EXPORT void rkglMouseWheelFuncGLFW(GLFWwindow* window, double xoffset, double yoffset);
__ROKI_GL_EXPORT void rkglMouseDragFuncGLFW(GLFWwindow* window, double x, double y);
__ROKI_GL_EXPORT void rkglVisFuncGLFW(GLFWwindow* window);

__END_DECLS

#endif /* __RKGL_GLFW_H__ */
