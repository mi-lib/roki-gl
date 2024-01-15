/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions
 */

#ifndef __RKGL_INPUT_H__
#define __RKGL_INPUT_H__

#include <roki_gl/rkgl_camera.h>

__BEGIN_DECLS

/* key state */
__ROKI_GL_EXPORT int rkgl_key_mod;

__ROKI_GL_EXPORT void rkglKeyCARotateUp(rkglCamera *cam, double d, bool ctrlison);
__ROKI_GL_EXPORT void rkglKeyCARotateDown(rkglCamera *cam, double d, bool ctrlison);
__ROKI_GL_EXPORT void rkglKeyCARotateLeft(rkglCamera *cam, double d, bool ctrlison);
__ROKI_GL_EXPORT void rkglKeyCARotateRight(rkglCamera *cam, double d, bool ctrlison);

/* mouse state */
__ROKI_GL_EXPORT int rkgl_mouse_button;
__ROKI_GL_EXPORT int rkgl_mouse_x;
__ROKI_GL_EXPORT int rkgl_mouse_y;

#define rkglMouseStoreXY(x,y) do{\
  rkgl_mouse_x = (x);\
  rkgl_mouse_y = (y);\
} while(0)

__ROKI_GL_EXPORT void rkglMouseStoreButtonMod(int button, int state, int presscode, int mod);
__ROKI_GL_EXPORT void rkglMouseStoreInput(int button, int state, int presscode, int x, int y, int mod);
__ROKI_GL_EXPORT void rkglMouseDragGetIncrementer(rkglCamera *cam, int x, int y, double *dx, double *dy);

__ROKI_GL_EXPORT void rkglMouseDragCARotate(rkglCamera *cam, double dx, double dy, int ctrlkey);
__ROKI_GL_EXPORT void rkglMouseDragCATranslate(rkglCamera *cam, double dx, double dy, int ctrlkey);
__ROKI_GL_EXPORT void rkglMouseDragCAZoom(rkglCamera *cam, double dx, double dy, int ctrlkey);

__END_DECLS

#endif /* __RKGL_INPUT_H__ */
