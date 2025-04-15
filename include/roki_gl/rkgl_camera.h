/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#ifndef __RKGL_CAMERA_H__
#define __RKGL_CAMERA_H__

#include <roki_gl/rkgl_misc.h>
#include <zeo/zeo.h>

__BEGIN_DECLS

ZDEF_STRUCT( __ROKI_GL_EXPORT, rkglCamera ){
  GLclampf background[4];  /*! \brief background color */
  GLint viewport[4];       /*! \brief viewport */
  GLdouble viewvolume[16]; /*! \brief view volume */
  GLdouble viewframe[16];  /*! \brief view frame */
};

__ROKI_GL_EXPORT void rkglCameraInit(rkglCamera *camera);

/* background color */

#define rkglCameraSetBackground(camera,r,g,b) do{\
  (camera)->background[0] = (r);\
  (camera)->background[1] = (g);\
  (camera)->background[2] = (b);\
  (camera)->background[3] = 1.0;\
} while(0)

#define rkglCameraCopyBackground(src,dest) memcpy( (dest)->background, (src)->background, sizeof(GLclampf)*4 )

/* viewport */

__ROKI_GL_EXPORT void rkglCameraSetViewport(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h);
__ROKI_GL_EXPORT void rkglCameraLoadViewport(rkglCamera *c);
__ROKI_GL_EXPORT void rkglCameraGetViewport(rkglCamera *c);

#define rkglCameraCopyViewport(src,dest) memcpy( (dest)->viewport, (src)->viewport, sizeof(GLint)*4 )

#define rkglCameraViewportOX(c)          ( (double)(c)->viewport[0] )
#define rkglCameraViewportOY(c)          ( (double)(c)->viewport[1] )
#define rkglCameraViewportWidth(c)       ( (double)(c)->viewport[2] )
#define rkglCameraViewportHeight(c)      ( (double)(c)->viewport[3] )
#define rkglCameraViewportAspectRatio(c) ( rkglCameraViewportWidth(c) / rkglCameraViewportHeight(c) )

/* view volume */

__ROKI_GL_EXPORT void rkglInitViewvolume(void);

__ROKI_GL_EXPORT void rkglCameraLoadViewvolume(rkglCamera *c);
__ROKI_GL_EXPORT void rkglCameraGetViewvolume(rkglCamera *c);
#define rkglCameraCopyViewvolume(src,dest) memcpy( (dest)->viewvolume, (src)->viewvolume, sizeof(GLdouble)*16 )

__ROKI_GL_EXPORT void rkglCameraSetOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraSetFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraSetOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraSetFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraScaleOrthoWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraScaleFrustumWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraScaleOrthoHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraScaleFrustumHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraSetPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far);
__ROKI_GL_EXPORT void rkglCameraFitFrustumToViewport(rkglCamera *cam, int w, int h, double width, double near, double far);

/* camera angle */

__ROKI_GL_EXPORT void rkglCameraLoadViewframe(rkglCamera *c);
__ROKI_GL_EXPORT void rkglCameraGetViewframe(rkglCamera *c);
__ROKI_GL_EXPORT zFrame3D *rkglCameraViewframeToFrame3D(rkglCamera *cam, zFrame3D *f);
__ROKI_GL_EXPORT zVec3D *rkglCameraGetViewVec(rkglCamera *cam, zVec3D *v);

#define rkglCameraCopyViewframe(src,dest) memcpy( (dest)->viewframe, (src)->viewframe, sizeof(GLdouble)*16 )

__ROKI_GL_EXPORT void rkglInitViewframe(void);

__ROKI_GL_EXPORT void rkglCameraAlignViewframe(rkglCamera *c);

__ROKI_GL_EXPORT void rkglCameraSetViewframe(rkglCamera *c, double x, double y, double z, double pan, double tilt, double roll);
__ROKI_GL_EXPORT void rkglCameraSetPanTiltRoll(rkglCamera *c, double pan, double tilt, double roll);
__ROKI_GL_EXPORT void rkglCameraLockonAndSetPanTiltRoll(rkglCamera *c, double pan, double tilt, double roll);
__ROKI_GL_EXPORT void rkglCameraRotate(rkglCamera *c, double angle, double x, double y, double z);
__ROKI_GL_EXPORT void rkglCameraLockonAndRotate(rkglCamera *c, double angle, double x, double y, double z);

__ROKI_GL_EXPORT void rkglCameraMove(rkglCamera *c, double x, double y, double z);
__ROKI_GL_EXPORT void rkglCameraRelMove(rkglCamera *c, double x, double y, double z);

__ROKI_GL_EXPORT void rkglCameraLookAt(rkglCamera *c, GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);

__ROKI_GL_EXPORT void rkglCameraRelMoveLeft(rkglCamera *cam, double d);
__ROKI_GL_EXPORT void rkglCameraRelMoveRight(rkglCamera *cam, double d);
__ROKI_GL_EXPORT void rkglCameraRelMoveUp(rkglCamera *cam, double d);
__ROKI_GL_EXPORT void rkglCameraRelMoveDown(rkglCamera *cam, double d);
__ROKI_GL_EXPORT void rkglCameraZoomIn(rkglCamera *cam, double d);
__ROKI_GL_EXPORT void rkglCameraZoomOut(rkglCamera *cam, double d);

/* camera */

__ROKI_GL_EXPORT rkglCamera *rkglCameraCopy(rkglCamera *src, rkglCamera *dest);
#define rkglCameraCopyDefault(dest) rkglCameraCopy( NULL, dest )

/*
void rkglCameraFRead(FILE *fp, rkglCamera *cam);
*/

/* default camera parameters */

__ROKI_GL_EXPORT rkglCamera *rkgl_default_cam;
__ROKI_GL_EXPORT double rkgl_default_vv_width;
__ROKI_GL_EXPORT double rkgl_default_vv_near;
__ROKI_GL_EXPORT double rkgl_default_vv_far;
__ROKI_GL_EXPORT double rkgl_default_key_delta_trans;
__ROKI_GL_EXPORT double rkgl_default_key_delta_angle;

__ROKI_GL_EXPORT void rkglSetDefaultCallbackParam(rkglCamera *cam, double width, double near, double far, double dl, double da);

__END_DECLS

#endif /* __RKGL_CAMERA_H__ */
