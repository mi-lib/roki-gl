/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#ifndef __RKGL_CAMERA_H__
#define __RKGL_CAMERA_H__

#include <roki_gl/rkgl_misc.h>

__BEGIN_DECLS

/*! \struct rkglCamera
 *! \brief camera class.
 */
ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglCamera ){
  GLclampf background[4];  /*! \brief background color */
  GLint viewport[4];       /*! \brief viewport */
  double fovy;             /*! \brief field of view in y-direction */
  double znear;             /*! \brief z-near of the viewvolume */
  double zfar;              /*! \brief z-far of the viewvolume */
  zFrame3D viewframe;      /*! \brief view frame */
  /*! \cond */
  GLdouble _viewvolume[16]; /* view volume */
  zFrame3D *platform;      /* frame of a platform */
  /*! \endcond */
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
__ROKI_GL_EXPORT void rkglCameraCopyViewvolume(rkglCamera *src, rkglCamera *dest);

/*! \brief set viewvolume of a camera that produces a parallel projection. */
__ROKI_GL_EXPORT void rkglCameraSetOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble znear, GLdouble zfar);
/*! \brief set viewvolume of a camera that produces a perspective projection. */
__ROKI_GL_EXPORT void rkglCameraSetFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble znear, GLdouble zfar);
/*! \brief set viewvolume of a camera that produces a parallel projection centering a specified point. */
__ROKI_GL_EXPORT void rkglCameraSetOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble znear, GLdouble zfar);
/*! \brief set viewvolume of a camera that produces a perspective projection centering a specified point. */
__ROKI_GL_EXPORT void rkglCameraSetFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble znear, GLdouble zfar);
/*! \brief scale viewvolume of a camera that produces parallel projection as to fit width to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleOrthoWidth(rkglCamera *c, double scale, GLdouble znear, GLdouble zfar);
/*! \brief scale viewvolume of a camera that produces perspective projection as to fit width to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleFrustumWidth(rkglCamera *c, double scale, GLdouble znear, GLdouble zfar);
/*! \brief scale viewvolume of a camera that produces parallel projection as to fit height to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleOrthoHeight(rkglCamera *c, double scale, GLdouble znear, GLdouble zfar);
/*! \brief scale viewvolume of a camera that produces perspective projection as to fit height to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraScaleFrustumHeight(rkglCamera *c, double scale, GLdouble znear, GLdouble zfar);
/*! \brief set viewvolume of a camera that produces perspective projection from field of view and aspect ratio. */
__ROKI_GL_EXPORT void rkglCameraSetPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble znear, GLdouble zfar);
/*! \brief set viewvolume of a camera that produes perspective projection from field of view, where aspect ratio is automatically computed from the current viewport. */
#define rkglCameraFitPerspective(camera,fovy,znear,zfar) rkglCameraSetPerspective( camera, fovy, rkglCameraViewportAspectRatio(camera), znear,zfar )
#define rkglCameraPerspective(camera) rkglCameraFitPerspective( camera, (camera)->fovy, (camera)->znear, (camera)->zfar )

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

/* camera */

/*! \brief initialize a camera. */
__ROKI_GL_EXPORT rkglCamera *rkglCameraInit(rkglCamera *camera);

/*! \brief copy properties of a camera to another. */
__ROKI_GL_EXPORT rkglCamera *rkglCameraCopy(rkglCamera *src, rkglCamera *dest);

/*! \brief copy properties of the default camera to another. */
#define rkglCameraCopyDefault(dest) rkglCameraCopy( NULL, dest )

/*! \brief set a platform of a camera. */
#define rkglCameraSetPlatform(camera,_platform) ( (camera)->platform = _platform )

/* default camera */

__ROKI_GL_EXPORT rkglCamera *rkgl_default_camera;

#define rkglSetDefaultCamera(camera) ( rkgl_default_camera = (camera) )

#define RKGL_DEFAULT_VV_FOVY  30.0
#define RKGL_DEFAULT_VV_NEAR   1.0
#define RKGL_DEFAULT_VV_FAR  200.0

/* parse ZTK format */

#define ZTK_TAG_ROKIGL_CAMERA            "roki-gl::camera"

#define ZTK_KEY_ROKIGL_CAMERA_BACKGROUND "background"
#define ZTK_KEY_ROKIGL_CAMERA_VIEWPORT   "viewport"

#define ZTK_KEY_ROKIGL_CAMERA_FOVY       "fovy"
#define ZTK_KEY_ROKIGL_CAMERA_NEAR       "near"
#define ZTK_KEY_ROKIGL_CAMERA_FAR        "far"

#define ZTK_KEY_ROKIGL_CAMERA_POS        "pos"
#define ZTK_KEY_ROKIGL_CAMERA_ATT        "att"
#define ZTK_KEY_ROKIGL_CAMERA_ROT        "rot"
#define ZTK_KEY_ROKIGL_CAMERA_FRAME      "frame"

/*! \brief read a 3D shape from a ZTK format processor. */
__ROKI_GL_EXPORT rkglCamera *rkglCameraFromZTK(rkglCamera *camera, ZTK *ztk);

/*! \brief print out a camera to a file. */
__ROKI_GL_EXPORT void rkglCameraFPrintZTK(FILE *fp, rkglCamera *camera);

/* camera array */
zArrayClass( rkglCameraArray, rkglCamera );

/*! \brief allocate an array of cameras. */
__ROKI_GL_EXPORT bool rkglCameraArrayAlloc(rkglCameraArray *cameraarray, int num);

/*! \brief read properties of cameras from a ZTK format processor. */
__ROKI_GL_EXPORT bool rkglCameraArrayFromZTK(rkglCameraArray *cameraarray, ZTK *ztk);
/*! \brief print properties of cameras out to a file. */
__ROKI_GL_EXPORT void rkglCameraArrayFPrintZTK(FILE *fp, rkglCameraArray *cameraarray);
/*! \brief read multiple cameras from a ZTK format file. */
__ROKI_GL_EXPORT rkglCameraArray *rkglCameraArrayReadZTK(rkglCameraArray *cameraarray, const char filename[]);
/*! \brief write multiple cameras to a ZTK format file. */
__ROKI_GL_EXPORT bool rkglCameraArrayWriteZTK(rkglCameraArray *cameraarray, const char filename[]);

__END_DECLS

#endif /* __RKGL_CAMERA_H__ */
