/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glut - GLUT (OpenGL utility toolkit) wrapper
 */

#ifndef __RKGL_GLUT_H__
#define __RKGL_GLUT_H__

#include <roki-gl/rkgl_camera.h>
#include <GL/glut.h>

__BEGIN_DECLS

void rkglInitGLUT(int *argc, char **argv);
int rkglWindowCreateGLUT(int x, int y, int w, int h, const char *title);

/* default callback functions */

void rkglSetCallbackParamGLUT(rkglCamera *c, double vv_width, double vv_near, double vv_far, double dl_key, double da_key);

void rkglSaveInputStatusGLUT(int button, int state, int x, int y);

void rkglReshapeFuncGLUT(int w, int h);
void rkglIdleFuncGLUT(void);
void rkglKeyFuncGLUT(unsigned char key, int x, int y);
void rkglSpecialFuncGLUT(int key, int x, int y);
void rkglMouseFuncGLUT(int button, int state, int x, int y);
void rkglMouseDragFuncGLUT(int x, int y);
void rkglVisFuncGLUT(int visible);

__END_DECLS

#endif /* __RKGL_GLUT_H__ */
