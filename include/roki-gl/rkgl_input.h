/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions
 */

#ifndef __RKGL_INPUT_H__
#define __RKGL_INPUT_H__

#include <roki-gl/rkgl_camera.h>

__BEGIN_DECLS

/* key state */
extern int rkgl_key_mod;

void rkglKeyCARotateUp(rkglCamera *cam, double d, bool ctrlison);
void rkglKeyCARotateDown(rkglCamera *cam, double d, bool ctrlison);
void rkglKeyCARotateLeft(rkglCamera *cam, double d, bool ctrlison);
void rkglKeyCARotateRight(rkglCamera *cam, double d, bool ctrlison);

/* mouse state */
extern int rkgl_mouse_button;
extern int rkgl_mouse_x;
extern int rkgl_mouse_y;

#define rkglMouseStoreXY(x,y) do{\
  rkgl_mouse_x = (x);\
  rkgl_mouse_y = (y);\
} while(0)

void rkglMouseStoreInput(int button, int state, int presscode, int x, int y, int mod);
void rkglMouseDragGetIncrementer(rkglCamera *cam, int x, int y, double *dx, double *dy);

void rkglMouseDragCARotate(rkglCamera *cam, double dx, double dy, int ctrlkey);
void rkglMouseDragCATranslate(rkglCamera *cam, double dx, double dy, int ctrlkey);
void rkglMouseDragCAZoom(rkglCamera *cam, double dx, double dy, int ctrlkey);

__END_DECLS

#endif /* __RKGL_INPUT_H__ */
