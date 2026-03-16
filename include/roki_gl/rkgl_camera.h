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
  GLclampf background[4];   /*! \brief background color */
  GLint viewport[4];        /*! \brief viewport */
  double left;              /*! \brief leftmost of frontal plane of viewvolume */
  double right;             /*! \brief rightmost of frontal plane of viewvolume */
  double bottom;            /*! \brief bottom of frontal plane of viewvolume */
  double top;               /*! \brief top of frontal plane of viewvolume */
  double znear;             /*! \brief z-near of viewvolume */
  double zfar;              /*! \brief z-far of viewvolume */
  double fovy;              /*! \brief field of view in y-direction */
  zFrame3D viewframe;       /*! \brief view frame */
  /*! \cond */
  GLdouble _viewvolume[16]; /* viewvolume */
  GLvoid (* _viewvolume_f)(GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble); /* glOrtho/glFrustum */
  zFrame3D *platform;       /* frame of a platform */
  ubyte *_depthbuffer;       /* depth buffer */
  /*! \endcond */
#ifdef __cplusplus
  rkglCamera();
  ~rkglCamera();
  // background
  void setBackground(double red, double green, double blue);
  void setBackground(zRGB *rgb);
  void setBackground(zRGB &rgb);
  void copyBackground(rkglCamera *dest);
  void copyBackground(rkglCamera &dest);
  // viewport
  void setViewport(GLint x, GLint y, GLsizei w, GLsizei h);
  void loadViewport();
  void getViewport();
  void copyViewport(rkglCamera *dest);
  void copyViewport(rkglCamera &dest);
  double viewportOX();
  double viewportOY();
  double viewportWidth();
  double viewportHeight();
  int viewportSize();
  double viewportAspectRatio();
  ubyte *readRGBBuffer(ubyte *buf);
  ubyte *readDepthBuffer(ubyte *buf);
  bool allocInternalDepthBuffer();
  void freeInternalDepthBuffer();
  ubyte *readInternalDepthBuffer();
  // viewvolume
  void loadViewvolume();
  void getViewvolume();
  void copyViewvolume(rkglCamera *dest);
  void copyViewvolume(rkglCamera &dest);
  void putViewvolume();
  void setViewvolumeXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top);
  void setViewvolumeZ(GLdouble _znear, GLdouble _zfar);
  void setViewvolumeFovy(GLdouble _fovy);
  void setViewvolumeZFovy(GLdouble _znear, GLdouble _zfar, GLdouble _fovy);
  void setOrtho();
  void setFrustum();
  void setViewvolumeOrthoXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top);
  void setViewvolumeFrustumXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top);
  void setViewvolumeXYToScaleWidth(double scale);
  void setViewvolumeXYToScaleHeight(double scale);
  void setViewvolumeXYPerspective(GLdouble aspect);
  void adjustViewvolumePerspective();
  // camera angle
  void copyViewframe(rkglCamera *dest);
  void copyViewframe(rkglCamera &dest);
  zVec3D *getViewVec(zVec3D *v);
  void put();
  void setViewframe(double x, double y, double z, double pan, double tilt, double roll);
  void translate(double x, double y, double z);
  void rotate(double angle, double x, double y, double z);
  void lookat(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz);
  void gazeAndRotate(double centerx, double centery, double centerz, double distance, double pan, double tilt, double roll);
  void moveLeft(double d);
  void moveRight(double d);
  void moveUp(double d);
  void moveDown(double d);
  void zoomIn(double d);
  void zoomOut(double d);
  void tiltUp(double a);
  void tiltDown(double a);
  void panLeft(double a);
  void panRight(double a);
  // general operations
  rkglCamera *init();
  void destroy();
  rkglCamera *copy(rkglCamera *dest);
  rkglCamera *copy(rkglCamera &dest);
  zFrame3D *setPlatform(zFrame3D *new_platform);
  zFrame3D *setPlatform(zFrame3D &new_platform);
#endif /* __cplusplus */
};

/* background color */

/*! \brief set background color of a camera. */
#define rkglCameraSetBackground(camera,r,g,b) do{\
  (camera)->background[0] = (r);\
  (camera)->background[1] = (g);\
  (camera)->background[2] = (b);\
  (camera)->background[3] = 1.0;\
} while(0)

/*! \brief set background color of a camera by a set of RGB parameters. */
#define rkglCameraSetBackgroundRGB(camera,rgb) rkglCameraSetBackground( camera, (rgb)->r, (rgb)->g, (rgb)->b )

/*! \brief copy background color of a camera to another. */
#define rkglCameraCopyBackground(src,dest) memcpy( (dest)->background, (src)->background, sizeof(GLclampf)*4 )

/* viewport */

/*! \brief set viewport of a camera. */
__ROKI_GL_EXPORT void rkglCameraSetViewport(rkglCamera *camera, GLint x, GLint y, GLsizei w, GLsizei h);
/*! \brief load viewport of a camera to the current render. */
__ROKI_GL_EXPORT void rkglCameraLoadViewport(rkglCamera *camera);
/*! \brief get and store the current viewport to a camera. */
__ROKI_GL_EXPORT void rkglCameraGetViewport(rkglCamera *camera);

/*! \brief copy viewport of a camera to another. */
#define rkglCameraCopyViewport(src,dest) memcpy( (dest)->viewport, (src)->viewport, sizeof(GLint)*4 )

#define rkglCameraViewportOX(camera)          ( (double)(camera)->viewport[0] )
#define rkglCameraViewportOY(camera)          ( (double)(camera)->viewport[1] )
#define rkglCameraViewportWidth(camera)       ( (double)(camera)->viewport[2] )
#define rkglCameraViewportHeight(camera)      ( (double)(camera)->viewport[3] )
#define rkglCameraViewportSize(camera)        ( (camera)->viewport[2] * (camera)->viewport[3] )
#define rkglCameraViewportAspectRatio(camera) ( rkglCameraViewportWidth(camera) / rkglCameraViewportHeight(camera) )

/*! \brief read RGB buffer of the current viewport of a camera. */
__ROKI_GL_EXPORT ubyte *rkglCameraReadRGBBuffer(rkglCamera *camera, ubyte *buf);
/*! \brief read depth buffer of the current viewport of a camera. */
__ROKI_GL_EXPORT ubyte *rkglCameraReadDepthBuffer(rkglCamera *camera, ubyte *buf);

/* allocate internal depth buffer for the current viewport of a camera. */
__ROKI_GL_EXPORT bool rkglCameraAllocInternalDepthBuffer(rkglCamera *camera);
/* free internal depth buffer for viewport of a camera. */
__ROKI_GL_EXPORT void rkglCameraFreeInternalDepthBuffer(rkglCamera *camera);
/*! \brief read internal depth buffer of the current viewport of a camera. */
__ROKI_GL_EXPORT ubyte *rkglCameraReadInternalDepthBuffer(rkglCamera *camera);

/* viewvolume */

/*! \brief reset viewvolume of the current render. */
__ROKI_GL_EXPORT void rkglResetViewvolume(void);

/*! \brief load viewvolume of a camera to the current render. */
__ROKI_GL_EXPORT void rkglCameraLoadViewvolume(rkglCamera *camera);
/*! \brief get and store the current viewvolume to a camera. */
__ROKI_GL_EXPORT void rkglCameraGetViewvolume(rkglCamera *camera);
/*! \brief copy viewvolume of a camera to another. */
__ROKI_GL_EXPORT void rkglCameraCopyViewvolume(rkglCamera *src, rkglCamera *dest);
/* put viewvolume of a camera. */
__ROKI_GL_EXPORT void rkglCameraPutViewvolume(rkglCamera *camera);

/*! \brief set viewplane of viewvolume of a camera. */
#define rkglCameraSetViewvolumeXY(camera,_left,_right,_bottom,_top) do{ \
  (camera)->left = (_left); \
  (camera)->right = (_right); \
  (camera)->bottom = (_bottom); \
  (camera)->top = (_top); \
} while(0)

/*! \brief set near-far boundaries of viewvolume of a camera. */
#define rkglCameraSetViewvolumeZ(camera,_znear,_zfar) do{ \
  (camera)->znear = (_znear); \
  (camera)->zfar = (_zfar); \
} while(0)

/*! \brief set field of view in y-direction of a camera. */
#define rkglCameraSetViewvolumeFovy(camera,_fovy) ( (camera)->fovy = (_fovy) )

/*! \brief set near-far boundaries of viewvolume and field of view in y-direction of a camera. */
#define rkglCameraSetViewvolumeZFovy(camera,_znear,_zfar,_fovy) do{ \
  rkglCameraSetViewvolumeZ( camera, _znear, _zfar ); \
  rkglCameraSetViewvolumeFovy( camera, _fovy ); \
} while(0)

/*! \brief set viewvolume method of a camera for glOrtho. */
#define rkglCameraSetOrtho(camera)   ( (camera)->_viewvolume_f = glOrtho )
/*! \brief set viewvolume method of a camera for glFrustum. */
#define rkglCameraSetFrustum(camera) ( (camera)->_viewvolume_f = glFrustum )

/* set viewvolume of a camera that produces a parallel projection. */
__ROKI_GL_EXPORT void rkglCameraSetViewvolumeOrthoXY(rkglCamera *camera, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
/* set viewvolume of a camera that produces a perspective projection. */
__ROKI_GL_EXPORT void rkglCameraSetViewvolumeFrustumXY(rkglCamera *camera, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
/* compute corner coordinates of viewplane of a camera by scaling its width to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraSetViewvolumeXYToScaleWidth(rkglCamera *camera, double scale);
/* compute corner coordinates of viewplane of a camera by scaling its height to that of viewport. */
__ROKI_GL_EXPORT void rkglCameraSetViewvolumeXYToScaleHeight(rkglCamera *camera, double scale);
/* set viewvolume of a camera that produces perspective projection from field of view and aspect ratio. */
__ROKI_GL_EXPORT void rkglCameraSetViewvolumeXYPerspective(rkglCamera *camera, GLdouble aspect);

/*! \brief adjust viewvolume of a camera that produces perspective projection to aspect ratio of the current viewport. */
#define rkglCameraAdjustViewvolumePerspective(camera) rkglCameraSetViewvolumeXYPerspective( camera, rkglCameraViewportAspectRatio(camera) )

/* camera angle */

/*! \brief set viewframe of a camera. */
__ROKI_GL_EXPORT void rkglResetViewframe(void);
/*! \brief get and store viewframe matrix of the current render to an array of values. */
__ROKI_GL_EXPORT void rkglGetViewframe(GLdouble viewframe[16]);

/*! \brief copy viewframe of a camera to another. */
#define rkglCameraCopyViewframe(src,dest) zFrame3DCopy( &(src)->viewframe, &(dest)->viewframe )

/*! \brief get view vector of a camera. */
#define rkglCameraGetViewVec(camera,v) zVec3DCopy( zMat3DVec(zFrame3DAtt(&(camera)->viewframe),zX), v )

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

/*! \brief destroy a camera. */
__ROKI_GL_EXPORT void rkglCameraDestroy(rkglCamera *camera);

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

#ifdef __cplusplus
inline rkglCamera::rkglCamera(){ rkglCameraInit( this ); }
inline rkglCamera::~rkglCamera(){ rkglCameraDestroy( this ); }
// background
inline void rkglCamera::setBackground(double red, double green, double blue){ rkglCameraSetBackground( this, red, green, blue ); }
inline void rkglCamera::setBackground(zRGB *rgb){ rkglCameraSetBackgroundRGB( this, rgb ); }
inline void rkglCamera::setBackground(zRGB &rgb){ rkglCameraSetBackgroundRGB( this, &rgb ); }
inline void rkglCamera::copyBackground(rkglCamera *dest){ rkglCameraCopyBackground( this, dest ); }
inline void rkglCamera::copyBackground(rkglCamera &dest){ rkglCameraCopyBackground( this, &dest ); }
// viewport
inline void rkglCamera::setViewport(GLint x, GLint y, GLsizei w, GLsizei h){ rkglCameraSetViewport( this, x, y, w, h ); }
inline void rkglCamera::loadViewport(){ rkglCameraLoadViewport( this ); }
inline void rkglCamera::getViewport(){ rkglCameraGetViewport( this ); }
inline void rkglCamera::copyViewport(rkglCamera *dest){ rkglCameraCopyViewport( this, dest ); }
inline void rkglCamera::copyViewport(rkglCamera &dest){ rkglCameraCopyViewport( this, &dest ); }
inline double rkglCamera::viewportOX(){ return rkglCameraViewportOX( this ); }
inline double rkglCamera::viewportOY(){ return rkglCameraViewportOY( this ); }
inline double rkglCamera::viewportWidth(){ return rkglCameraViewportWidth( this ); }
inline double rkglCamera::viewportHeight(){ return rkglCameraViewportHeight( this ); }
inline int rkglCamera::viewportSize(){ return rkglCameraViewportSize( this ); }
inline double rkglCamera::viewportAspectRatio(){ return rkglCameraViewportAspectRatio( this ); }
inline ubyte *rkglCamera::readRGBBuffer(ubyte *buf){ return rkglCameraReadRGBBuffer( this, buf ); }
inline ubyte *rkglCamera::readDepthBuffer(ubyte *buf){ return rkglCameraReadDepthBuffer( this, buf ); }
inline bool rkglCamera::allocInternalDepthBuffer(){ return rkglCameraAllocInternalDepthBuffer( this ); }
inline void rkglCamera::freeInternalDepthBuffer(){ rkglCameraFreeInternalDepthBuffer( this ); }
inline ubyte *rkglCamera::readInternalDepthBuffer(){ return rkglCameraReadInternalDepthBuffer( this ); }
// viewvolume
inline void rkglCamera::loadViewvolume(){ rkglCameraLoadViewvolume( this ); }
inline void rkglCamera::getViewvolume(){ rkglCameraGetViewvolume( this ); }
inline void rkglCamera::copyViewvolume(rkglCamera *dest){ rkglCameraCopyViewvolume( this, dest ); }
inline void rkglCamera::copyViewvolume(rkglCamera &dest){ rkglCameraCopyViewvolume( this, &dest ); }

inline void rkglCamera::putViewvolume(){ rkglCameraPutViewvolume( this ); }
inline void rkglCamera::setViewvolumeXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top){ rkglCameraSetViewvolumeXY( this, _left, _right, _bottom, _top ); }
inline void rkglCamera::setViewvolumeZ(GLdouble _znear, GLdouble _zfar){ rkglCameraSetViewvolumeZ( this, _znear, _zfar ); }
inline void rkglCamera::setViewvolumeFovy(GLdouble _fovy){ rkglCameraSetViewvolumeFovy( this, _fovy ); }
inline void rkglCamera::setViewvolumeZFovy(GLdouble _znear, GLdouble _zfar, GLdouble _fovy){ rkglCameraSetViewvolumeZFovy( this, _znear, _zfar, _fovy ); }
inline void rkglCamera::setOrtho(){ rkglCameraSetOrtho( this ); }
inline void rkglCamera::setFrustum(){ rkglCameraSetFrustum( this ); }
inline void rkglCamera::setViewvolumeOrthoXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top){ rkglCameraSetViewvolumeOrthoXY( this, _left, _right, _bottom, _top ); }
inline void rkglCamera::setViewvolumeFrustumXY(GLdouble _left, GLdouble _right, GLdouble _bottom, GLdouble _top){ rkglCameraSetViewvolumeFrustumXY( this, _left, _right, _bottom, _top ); }
inline void rkglCamera::setViewvolumeXYToScaleWidth(double scale){ rkglCameraSetViewvolumeXYToScaleWidth( this, scale ); }
inline void rkglCamera::setViewvolumeXYToScaleHeight(double scale){ rkglCameraSetViewvolumeXYToScaleHeight( this, scale ); }
inline void rkglCamera::setViewvolumeXYPerspective(GLdouble aspect){ rkglCameraSetViewvolumeXYPerspective( this, aspect ); }
inline void rkglCamera::adjustViewvolumePerspective(){ rkglCameraAdjustViewvolumePerspective( this ); }
// camera angle
inline void rkglCamera::copyViewframe(rkglCamera *dest){ rkglCameraCopyViewframe( this, dest ); }
inline void rkglCamera::copyViewframe(rkglCamera &dest){ rkglCameraCopyViewframe( this, &dest ); }
inline zVec3D *rkglCamera::getViewVec(zVec3D *v){ return rkglCameraGetViewVec( this, v ); }
inline void rkglCamera::put(){ rkglCameraPut( this ); }
inline void rkglCamera::setViewframe(double x, double y, double z, double pan, double tilt, double roll){ rkglCameraSetViewframe( this, x, y, z, pan, tilt, roll ); }
inline void rkglCamera::translate(double x, double y, double z){ rkglCameraTranslate( this, x, y, z ); }
inline void rkglCamera::rotate(double angle, double x, double y, double z){ rkglCameraRotate( this, angle, x, y, z ); }
inline void rkglCamera::lookat(double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz){ rkglCameraLookAt( this, eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz ); }
inline void rkglCamera::gazeAndRotate(double centerx, double centery, double centerz, double distance, double pan, double tilt, double roll){ rkglCameraGazeAndRotate( this, centerx, centery, centerz, distance, pan, tilt, roll ); }
inline void rkglCamera::moveLeft(double d){ rkglCameraMoveLeft( this, d ); }
inline void rkglCamera::moveRight(double d){ rkglCameraMoveRight( this, d ); }
inline void rkglCamera::moveUp(double d){ rkglCameraMoveUp( this, d ); }
inline void rkglCamera::moveDown(double d){ rkglCameraMoveDown( this, d ); }
inline void rkglCamera::zoomIn(double d){ rkglCameraZoomIn( this, d ); }
inline void rkglCamera::zoomOut(double d){ rkglCameraZoomOut( this,d ); }
inline void rkglCamera::tiltUp(double a){ rkglCameraTiltUp( this, a ); }
inline void rkglCamera::tiltDown(double a){ rkglCameraTiltDown( this, a ); }
inline void rkglCamera::panLeft(double a){ rkglCameraPanLeft( this, a ); }
inline void rkglCamera::panRight(double a){ rkglCameraPanRight( this, a ); }
// general operations
inline rkglCamera *rkglCamera::init(){ return rkglCameraInit( this ); }
inline void rkglCamera::destroy(){ rkglCameraDestroy( this ); }
inline rkglCamera *rkglCamera::copy(rkglCamera *dest){ return rkglCameraCopy( this, dest ); }
inline rkglCamera *rkglCamera::copy(rkglCamera &dest){ return rkglCameraCopy( this, &dest ); }
inline zFrame3D *rkglCamera::setPlatform(zFrame3D *new_platform){ return rkglCameraSetPlatform( this, new_platform ); }
inline zFrame3D *rkglCamera::setPlatform(zFrame3D &new_platform){ return rkglCameraSetPlatform( this, &new_platform ); }
#endif /* __cplusplus */

#endif /* __RKGL_CAMERA_H__ */
