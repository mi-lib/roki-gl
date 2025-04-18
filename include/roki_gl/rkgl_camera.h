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

/*! \struct rkglCamera
 *! \brief camera class
 */
ZDEF_STRUCT( __ROKI_GL_EXPORT, rkglCamera ){
  GLclampf background[4];  /*! \brief background color */
  GLint viewport[4];       /*! \brief viewport */
  GLdouble viewvolume[16]; /*! \brief view volume */
  zFrame3D viewframe;      /*! \brief view frame */
};

/* background color */

/*! \brief set background color of a camera. */
#define rkglCameraSetBackground(camera,r,g,b) do{\
  (camera)->background[0] = (r);\
  (camera)->background[1] = (g);\
  (camera)->background[2] = (b);\
  (camera)->background[3] = 1.0;\
} while(0)

/*! \brief copy background color of a camera to another. */
#define rkglCameraCopyBackground(src,dest) memcpy( (dest)->background, (src)->background, sizeof(GLclampf)*4 )

/* viewport */

/*! \brief set viewport of a camera. */
__ROKI_GL_EXPORT void rkglCameraSetViewport(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h);
/*! \brief load viewport of a camera to the current render. */
__ROKI_GL_EXPORT void rkglCameraLoadViewport(rkglCamera *c);
/*! \brief get and store the current viewport to a camera. */
__ROKI_GL_EXPORT void rkglCameraGetViewport(rkglCamera *c);

/*! \brief copy viewport of a camera to another. */
#define rkglCameraCopyViewport(src,dest) memcpy( (dest)->viewport, (src)->viewport, sizeof(GLint)*4 )

#define rkglCameraViewportOX(c)          ( (double)(c)->viewport[0] )
#define rkglCameraViewportOY(c)          ( (double)(c)->viewport[1] )
#define rkglCameraViewportWidth(c)       ( (double)(c)->viewport[2] )
#define rkglCameraViewportHeight(c)      ( (double)(c)->viewport[3] )
#define rkglCameraViewportAspectRatio(c) ( rkglCameraViewportWidth(c) / rkglCameraViewportHeight(c) )

/* viewvolume */

/*! \brief reset viewvolume of the current render. */
__ROKI_GL_EXPORT void rkglResetViewvolume(void);

/*! \brief load viewvolume of a camera to the current render. */
__ROKI_GL_EXPORT void rkglCameraLoadViewvolume(rkglCamera *c);
/*! \brief get and store the current viewvolume to a camera. */
__ROKI_GL_EXPORT void rkglCameraGetViewvolume(rkglCamera *c);
/*! \brief copy viewvolume of a camera to another. */
#define rkglCameraCopyViewvolume(src,dest) memcpy( (dest)->viewvolume, (src)->viewvolume, sizeof(GLdouble)*16 )

/*! \brief set viewvolume of a camera that produces a parallel projection. */
__ROKI_GL_EXPORT void rkglCameraSetOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
/*! \brief set viewvolume of a camera that produces a perspective projection. */
__ROKI_GL_EXPORT void rkglCameraSetFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
/*! \brief set viewvolume of a camera that produces a parallel projection centering a specified point. */
__ROKI_GL_EXPORT void rkglCameraSetOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far);
/*! \brief set viewvolume of a camera that produces a perspective projection centering a specified point. */
__ROKI_GL_EXPORT void rkglCameraSetFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far);
/*! \brief scale viewvolume of a camera that produces parallel projection as to fit width to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleOrthoWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far);
/*! \brief scale viewvolume of a camera that produces perspective projection as to fit width to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleFrustumWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far);
/*! \brief scale viewvolume of a camera that produces parallel projection as to fit height to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleOrthoHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far);
/*! \brief scale viewvolume of a camera that produces perspective projection as to fit height to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleFrustumHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far);
/*! \brief set viewvolume of a camera that produces perspective projection from field of view and aspect ratio. */
__ROKI_GL_EXPORT void rkglCameraSetPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far);
/*! \brief fit viewvolume of a camera that produces perspective projection to viewport. */
__ROKI_GL_EXPORT void rkglCameraFitFrustumToViewport(rkglCamera *cam, int w, int h, double width, double near, double far);

/* camera angle */

/*! \brief set viewframe of a camera. */
__ROKI_GL_EXPORT void rkglResetViewframe(void);
/*! \brief get and store viewframe matrix of the current render to an array of values. */
__ROKI_GL_EXPORT void rkglGetViewframe(GLdouble viewframe[16]);

/*! \brief copy viewframe of a camera to another. */
#define rkglCameraCopyViewframe(src,dest) zFrame3DCopy( &(src)->viewframe, &(dest)->viewframe )

/*! \brief get view vector of a camera. */
#define rkglCameraGetViewVec(camera,v) zVec3DCopy( zMat3DVec(zFrame3DAtt(&(cam)->viewframe),zX), v )

/*! \brief put a camera on the current render. */
__ROKI_GL_EXPORT void rkglCameraPut(rkglCamera *camera);

/* set viewframe of a camera. */
__ROKI_GL_EXPORT void rkglCameraSetViewframe(rkglCamera *camera, double x, double y, double z, double pan, double tilt, double roll);
/* translate viewframe of a camera. */
__ROKI_GL_EXPORT void rkglCameraTranslate(rkglCamera *camera, double x, double y, double z);
/* rotate viewframe of a camera. */
__ROKI_GL_EXPORT void rkglCameraRotate(rkglCamera *camera, double angle, double x, double y, double z);
/* locate viewframe of a camera as to look at a specified point from another. */
__ROKI_GL_EXPORT void rkglCameraLookAt(rkglCamera *camera, double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz);
/* rotate viewframe of a camera as to look at a specified point at a spcified distance. */
__ROKI_GL_EXPORT void rkglCameraGazeAndRotate(rkglCamera *camera, double centerx, double centery, double centerz, double distance, double pan, double tilt, double roll);

#define rkglCameraMoveLeft(camera,d)  rkglCameraTranslate( camera,   0, (d),   0 )
#define rkglCameraMoveRight(camera,d) rkglCameraTranslate( camera,   0,-(d),   0 )
#define rkglCameraMoveUp(camera,d)    rkglCameraTranslate( camera,   0,   0, (d) )
#define rkglCameraMoveDown(camera,d)  rkglCameraTranslate( camera,   0,   0,-(d) )
#define rkglCameraZoomIn(camera,d)    rkglCameraTranslate( camera,-(d),   0,   0 )
#define rkglCameraZoomOut(camera,d)   rkglCameraTranslate( camera, (d),   0,   0 )

#define rkglCameraTiltUp(camera,a)    rkglCameraRotate( camera, (a), 0, 1, 0 )
#define rkglCameraTiltDown(camera,a)  rkglCameraRotate( camera, (a), 0,-1, 0 )
#define rkglCameraPanLeft(camera,a)   rkglCameraRotate( camera, (a), 0, 0, 1 )
#define rkglCameraPanRight(camera,a)  rkglCameraRotate( camera, (a), 0, 0,-1 )

/*! \brief copy properties of a camera to another. */
__ROKI_GL_EXPORT rkglCamera *rkglCameraCopy(rkglCamera *src, rkglCamera *dest);

/*! \brief copy properties of the default camera to another. */
#define rkglCameraCopyDefault(dest) rkglCameraCopy( NULL, dest )

/*
void rkglCameraFRead(FILE *fp, rkglCamera *cam);
*/

/* default camera parameters */

__ROKI_GL_EXPORT rkglCamera *rkgl_default_camera;

__ROKI_GL_EXPORT double rkgl_default_vv_fovy;
__ROKI_GL_EXPORT double rkgl_default_vv_near;
__ROKI_GL_EXPORT double rkgl_default_vv_far;

__ROKI_GL_EXPORT void rkglSetDefaultCamera(rkglCamera *camera, double fovy, double near, double far);

#define rkglDefaultCameraSetPerspective() rkglCameraSetPerspective( rkgl_default_camera, rkgl_default_vv_fovy, rkglCameraViewportAspectRatio(rkgl_default_camera), rkgl_default_vv_near, rkgl_default_vv_far )

__END_DECLS

#endif /* __RKGL_CAMERA_H__ */
