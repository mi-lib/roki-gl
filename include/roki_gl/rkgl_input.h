/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions
 */

#ifndef __RKGL_INPUT_H__
#define __RKGL_INPUT_H__

#include <roki_gl/rkgl_camera.h>

__BEGIN_DECLS

/* key modifier state */
__ROKI_GL_EXPORT int rkgl_key_mod;

/* delta amounts of translation and rotation corresponding to key actions. */
__ROKI_GL_EXPORT double rkgl_key_delta_tra;
__ROKI_GL_EXPORT double rkgl_key_delta_rot;

__ROKI_GL_EXPORT void rkglSetKeyDelta(double tra, double rot);

__ROKI_GL_EXPORT void rkglKeyCameraRotate(rkglCamera *cam, double angle, double x, double y, double z, bool ctrlisin);

/* key actions */
#define rkglKeyCameraMoveLeft(camera)  rkglCameraMoveLeft(  camera, rkgl_key_delta_tra )
#define rkglKeyCameraMoveRight(camera) rkglCameraMoveRight( camera, rkgl_key_delta_tra )
#define rkglKeyCameraMoveUp(camera)    rkglCameraMoveUp(    camera, rkgl_key_delta_tra )
#define rkglKeyCameraMoveDown(camera)  rkglCameraMoveDown(  camera, rkgl_key_delta_tra )
#define rkglKeyCameraZoomIn(camera)    rkglCameraZoomIn(    camera, rkgl_key_delta_tra )
#define rkglKeyCameraZoomOut(camera)   rkglCameraZoomOut(   camera, rkgl_key_delta_tra )
#define rkglKeyCameraTiltUp(camera)    rkglCameraTiltUp(    camera, rkgl_key_delta_rot )
#define rkglKeyCameraTiltDown(camera)  rkglCameraTiltDown(  camera, rkgl_key_delta_rot )
#define rkglKeyCameraPanLeft(camera)   rkglCameraPanLeft(   camera, rkgl_key_delta_rot )
#define rkglKeyCameraPanRight(camera)  rkglCameraPanRight(  camera, rkgl_key_delta_rot )

#define rkglKeyDefaultCameraMoveLeft()  rkglKeyCameraMoveLeft(  rkgl_default_camera )
#define rkglKeyDefaultCameraMoveRight() rkglKeyCameraMoveRight( rkgl_default_camera )
#define rkglKeyDefaultCameraMoveUp()    rkglKeyCameraMoveUp(    rkgl_default_camera )
#define rkglKeyDefaultCameraMoveDown()  rkglKeyCameraMoveDown(  rkgl_default_camera )
#define rkglKeyDefaultCameraZoomIn()    rkglKeyCameraZoomIn(    rkgl_default_camera )
#define rkglKeyDefaultCameraZoomOut()   rkglKeyCameraZoomOut(   rkgl_default_camera )
#define rkglKeyDefaultCameraTiltUp()    rkglKeyCameraTiltUp(    rkgl_default_camera )
#define rkglKeyDefaultCameraTiltDown()  rkglKeyCameraTiltDown(  rkgl_default_camera )
#define rkglKeyDefaultCameraPanLeft()   rkglKeyCameraPanLeft(   rkgl_default_camera )
#define rkglKeyDefaultCameraPanRight()  rkglKeyCameraPanRight(  rkgl_default_camera )

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

__ROKI_GL_EXPORT void rkglMouseDragCameraRotate(rkglCamera *cam, double dx, double dy, int ctrlkey);
__ROKI_GL_EXPORT void rkglMouseDragCameraTranslate(rkglCamera *cam, double dx, double dy);
__ROKI_GL_EXPORT void rkglMouseDragCameraZoom(rkglCamera *cam, double dx, double dy);

#define rkglMouseDragDefaultCameraRotate( dx, dy, ctrlkey ) rkglMouseDragCameraRotate( rkgl_default_camera, dx, dy, ctrlkey )
#define rkglMouseDragDefaultCameraTranslate( dx, dy )       rkglMouseDragCameraTranslate( rkgl_default_camera, dx, dy )
#define rkglMouseDragDefaultCameraZoom( dx, dy )            rkglMouseDragCameraZoom( rkgl_default_camera, dx, dy )

__END_DECLS

#endif /* __RKGL_INPUT_H__ */
