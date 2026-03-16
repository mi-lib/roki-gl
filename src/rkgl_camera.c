/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#include <roki_gl/rkgl_camera.h>

/* viewport */

/* set viewport of a camera. */
void rkglCameraSetViewport(rkglCamera *camera, GLint x, GLint y, GLsizei w, GLsizei h)
{
  camera->viewport[0] = x; /* x */
  camera->viewport[1] = y; /* y */
  camera->viewport[2] = w; /* width */
  camera->viewport[3] = h; /* height */
  rkglCameraLoadViewport( camera );
}

/* load viewport of a camera to the current render. */
void rkglCameraLoadViewport(rkglCamera *camera)
{
  glViewport( camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3] );
  glScissor( camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3] );
  glClearColor( camera->background[0], camera->background[1], camera->background[2], camera->background[3] );
}

/* get and store the current viewport to a camera. */
void rkglCameraGetViewport(rkglCamera *camera)
{
  glGetIntegerv( GL_VIEWPORT, camera->viewport );
}

/* read RGB buffer of the current viewport of a camera. */
ubyte *rkglCameraReadRGBBuffer(rkglCamera *camera, ubyte *buf)
{
  rkglReadBuffer( GL_RGB, camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3], buf );
  return buf;
}

/* read depth buffer of the current viewport of a camera. */
ubyte *rkglCameraReadDepthBuffer(rkglCamera *camera, ubyte *buf)
{
  rkglReadBuffer( GL_DEPTH_COMPONENT, camera->viewport[0], camera->viewport[1], camera->viewport[2], camera->viewport[3], buf );
  return buf;
}

/* allocate internal depth buffer for the current viewport of a camera. */
bool rkglCameraAllocInternalDepthBuffer(rkglCamera *camera)
{
  if( camera->_depthbuffer ) free( camera->_depthbuffer );
  if( !( camera->_depthbuffer = zAlloc( ubyte, rkglCameraViewportSize(camera) )) ){
    ZALLOCERROR();
    return false;
  }
  return true;
}

/* free internal depth buffer for viewport of a camera. */
void rkglCameraFreeInternalDepthBuffer(rkglCamera *camera)
{
  if( camera->_depthbuffer )
    free( camera->_depthbuffer );
}

/* read internal depth buffer of the current viewport of a camera. */
ubyte *rkglCameraReadInternalDepthBuffer(rkglCamera *camera)
{
  if( !camera->_depthbuffer )
    if( !rkglCameraAllocInternalDepthBuffer( camera ) ) return NULL;
  return rkglCameraReadDepthBuffer( camera, camera->_depthbuffer );
}

/* view volume */

/* reset viewvolume of the current render. */
void rkglResetViewvolume(void)
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
}

/* load viewvolume of a camera to the current render. */
void rkglCameraLoadViewvolume(rkglCamera *camera)
{
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd( camera->_viewvolume );
}

/* get and store the current viewvolume to a camera. */
void rkglCameraGetViewvolume(rkglCamera *camera)
{
  glGetDoublev( GL_PROJECTION_MATRIX, camera->_viewvolume );
}

/* copy viewvolume of a camera to another. */
void rkglCameraCopyViewvolume(rkglCamera *src, rkglCamera *dest)
{
  dest->fovy = src->fovy;
  dest->znear = src->znear;
  dest->zfar = src->zfar;
  memcpy( dest->_viewvolume, src->_viewvolume, sizeof(GLdouble)*16 );
}

/* set viewvolume of a camera that produces a parallel projection. */
void rkglCameraSetOrtho(rkglCamera *camera, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble znear, GLdouble zfar)
{
  rkglResetViewvolume();
  camera->fovy = 0;
  glOrtho( left, right, bottom, top, ( camera->znear = znear ), ( camera->zfar = zfar ) );
  rkglCameraGetViewvolume( camera );
}

/* set viewvolume of a camera that produces a perspective projection. */
void rkglCameraSetFrustum(rkglCamera *camera, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble znear, GLdouble zfar)
{
  rkglResetViewvolume();
  camera->fovy = zRad2Deg( 2 * atan2( fabs( 0.5 * ( right - left ) ), znear ) );
  glFrustum( left, right, bottom, top, ( camera->znear = znear ), ( camera->zfar = zfar ) );
  rkglCameraGetViewvolume( camera );
}

/* set viewvolume of a camera that produces a parallel projection centering a specified point. */
void rkglCameraSetOrthoCenter(rkglCamera *camera, GLdouble x, GLdouble y, GLdouble znear, GLdouble zfar)
{
  rkglCameraSetOrtho( camera, -x, x, -y, y, znear, zfar );
}

/* set viewvolume of a camera that produces a perspective projection centering a specified point. */
void rkglCameraSetFrustumCenter(rkglCamera *camera, GLdouble x, GLdouble y, GLdouble znear, GLdouble zfar)
{
  rkglCameraSetFrustum( camera, -x, x, -y, y, znear, zfar );
}

/* compute corner coordinates of viewplane of a camera that fit to width of the current viewport. */
static void _rkglCameraFitViewvolumeWidth(rkglCamera *camera, double width, double *x, double *y)
{
  *y = ( *x = 0.5 * width ) / rkglCameraViewportAspectRatio(camera);
}

/* compute corner coordinates of viewplane of a camera that fit to height of the current viewport. */
static void _rkglCameraFitViewvolumeHeight(rkglCamera *camera, double height, double *x, double *y)
{
  *x = ( *y = 0.5 * height ) * rkglCameraViewportAspectRatio(camera);
}

/* compute corner coordinates of viewplane of a camera by scaling its width to that of viewport. */
static void _rkglCameraScaleViewvolumeWidth(rkglCamera *camera, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeWidth( camera, rkglCameraViewportWidth(camera) * scale, x, y );
}

/* compute corner coordinates of viewplane of a camera by scaling its height to that of viewport. */
static void _rkglCameraScaleViewvolumeHeight(rkglCamera *camera, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeHeight( camera, rkglCameraViewportHeight(camera) * scale, x, y );
}

/* scale viewvolume of a camera that produces parallel projection as to fit width to that of viewport. */
void rkglCameraScaleOrthoWidth(rkglCamera *camera, double scale, GLdouble znear, GLdouble zfar)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( camera, scale, &x, &y );
  rkglCameraSetOrthoCenter( camera, x, y, znear, zfar );
}

/* scale viewvolume of a camera that produces perspective projection as to fit width to that of viewport. */
void rkglCameraScaleFrustumWidth(rkglCamera *camera, double scale, GLdouble znear, GLdouble zfar)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( camera, scale, &x, &y );
  rkglCameraSetFrustumCenter( camera, x, y, znear, zfar );
}

/* scale viewvolume of a camera that produces parallel projection as to fit height to that of viewport. */
void rkglCameraScaleOrthoHeight(rkglCamera *camera, double scale, GLdouble znear, GLdouble zfar)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( camera, scale, &x, &y );
  rkglCameraSetOrthoCenter( camera, x, y, znear, zfar );
}

/* scale viewvolume of a camera that produces perspective projection as to fit height to that of viewport. */
void rkglCameraScaleFrustumHeight(rkglCamera *camera, double scale, GLdouble znear, GLdouble zfar)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( camera, scale, &x, &y );
  rkglCameraSetFrustumCenter( camera, x, y, znear, zfar );
}

/* set viewvolume of a camera that produces perspective projection from field of view and aspect ratio. */
void rkglCameraSetPerspective(rkglCamera *camera, GLdouble fovy, GLdouble aspect, GLdouble znear, GLdouble zfar)
{
  double right, top;

  camera->fovy = fovy;
  right = ( top = znear * tan( 0.5 * zDeg2Rad( ( camera->fovy = fovy ) ) ) ) * aspect;
  rkglCameraSetFrustumCenter( camera, right, top, znear, zfar );
}

/* camera angle */

/* set viewframe of a camera. */
void rkglResetViewframe(void)
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

/* get and store viewframe matrix of the current render to an array of values. */
void rkglGetViewframe(double viewframe[16])
{
  glGetDoublev( GL_MODELVIEW_MATRIX, viewframe );
}

/* put a camera on the current render. */
void rkglCameraPut(rkglCamera *camera)
{
  GLdouble alignframe[] = {
    0, 0, 1, 0,
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1,
  };
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd( alignframe );
  rkglXformInv( &camera->viewframe );
  if( camera->platform )
    rkglXformInv( camera->platform );
}

/* set viewframe of a camera. */
void rkglCameraSetViewframe(rkglCamera *camera, double x, double y, double z, double pan, double tilt, double roll)
{
  zFrame3DFromPosZYX( &camera->viewframe, x, y, z, zDeg2Rad(pan), zDeg2Rad(tilt), zDeg2Rad(roll) );
}

/* translate viewframe of a camera. */
void rkglCameraTranslate(rkglCamera *camera, double x, double y, double z)
{
  zFrame3DTranslateView( &camera->viewframe, x, y, z );
}

/* rotate viewframe of a camera. */
void rkglCameraRotate(rkglCamera *camera, double angle, double x, double y, double z)
{
  zFrame3DRotateView( &camera->viewframe, zDeg2Rad(angle), x, y, z );
}

/* locate viewframe of a camera as to look at a specified point from another. */
void rkglCameraLookAt(rkglCamera *camera, double eyex, double eyey, double eyez, double centerx, double centery, double centerz, double upx, double upy, double upz)
{
  zFrame3DLookAtView( &camera->viewframe, eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz );
}

/* rotate viewframe of a camera as to look at a specified point at a spcified distance. */
void rkglCameraGazeAndRotate(rkglCamera *camera, double centerx, double centery, double centerz, double distance, double pan, double tilt, double roll)
{
  zFrame3DGazeAndRotateView( &camera->viewframe, centerx, centery, centerz, distance, zDeg2Rad(pan), zDeg2Rad(tilt), zDeg2Rad(roll) );
}

/* camera */

/* initialize a camera. */
rkglCamera *rkglCameraInit(rkglCamera *camera)
{
  rkglCameraSetBackground( camera, 0, 0, 0 );
  rkglCameraSetViewport( camera, 0, 0, 0, 0 );
  camera->fovy = RKGL_DEFAULT_VV_FOVY;
  camera->znear = RKGL_DEFAULT_VV_NEAR;
  camera->zfar  = RKGL_DEFAULT_VV_FAR;
  zFrame3DIdent( &camera->viewframe );
  rkglResetViewvolume();
  rkglCameraGetViewvolume( camera );
  rkglCameraSetPlatform( camera, NULL );
  camera->_depthbuffer = NULL;
  return camera;
}

/* destroy a camera. */
void rkglCameraDestroy(rkglCamera *camera)
{
  rkglCameraFreeInternalDepthBuffer( camera );
  rkglCameraInit( camera );
}

/* copy properties of a camera to anotoher. */
rkglCamera *rkglCameraCopy(rkglCamera *src, rkglCamera *dest)
{
  if( !src )
    if( !( src = rkgl_default_camera ) ){
      ZRUNERROR( "default camera not assigned" );
      return NULL;
    }
  rkglCameraCopyBackground( src, dest );
  rkglCameraCopyViewport( src, dest );
  rkglCameraCopyViewframe( src, dest );
  rkglCameraCopyViewvolume( src, dest );
  return dest;
}

/* default camera */

rkglCamera *rkgl_default_camera;

/* parse ZTK format */

static void *_rkglCameraBackgroundFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  double r, g, b;
  r = ZTKDouble(ztk);
  g = ZTKDouble(ztk);
  b = ZTKDouble(ztk);
  rkglCameraSetBackground( (rkglCamera*)obj, r, g, b );
  return obj;
}
static void *_rkglCameraViewportFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  double x, y, w, h;
  x = ZTKDouble(ztk);
  y = ZTKDouble(ztk);
  w = ZTKDouble(ztk);
  h = ZTKDouble(ztk);
  rkglCameraSetViewport( (rkglCamera*)obj, x, y, w, h );
  return obj;
}

static void *_rkglCameraFovyFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkglCamera*)obj)->fovy = ZTKDouble(ztk);
  return obj;
}
static void *_rkglCameraNearFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkglCamera*)obj)->znear = ZTKDouble(ztk);
  return obj;
}
static void *_rkglCameraFarFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((rkglCamera*)obj)->zfar = ZTKDouble(ztk);
  return obj;
}

static void *_rkglCameraPosFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zVec3DFromZTK( zFrame3DPos(&((rkglCamera*)obj)->viewframe), ztk );
  return obj;
}
static void *_rkglCameraAttFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zMat3DFromZTK( zFrame3DAtt(&((rkglCamera*)obj)->viewframe), ztk );
  return obj;
}
static void *_rkglCameraRotFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zVec3D aa;
  zAAFromZTK( &aa, ztk );
  zMat3DRotDRC( zFrame3DAtt(&((rkglCamera*)obj)->viewframe), &aa );
  return obj;
}
static void *_rkglCameraFrameFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  zFrame3DFromZTK( &((rkglCamera*)obj)->viewframe, ztk );
  return obj;
}

static bool _rkglCameraBackgroundFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%g, %g, %g\n", ((rkglCamera*)obj)->background[0], ((rkglCamera*)obj)->background[1], ((rkglCamera*)obj)->background[2] );
  return true;
}
static bool _rkglCameraViewportFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%d, %d, %d, %d\n", ((rkglCamera*)obj)->viewport[0], ((rkglCamera*)obj)->viewport[1], ((rkglCamera*)obj)->viewport[2], ((rkglCamera*)obj)->viewport[3] );
  return true;
}

static bool _rkglCameraFovyFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%g\n", ((rkglCamera*)obj)->fovy );
  return obj;
}
static bool _rkglCameraNearFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%g\n", ((rkglCamera*)obj)->znear );
  return obj;
}
static bool _rkglCameraFarFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%g\n", ((rkglCamera*)obj)->zfar );
  return obj;
}

static bool _rkglCameraFrameFPrint(FILE *fp, int i, void *obj){
  zFrame3DFPrint( fp, &((rkglCamera*)obj)->viewframe );
  return true;
}

static const ZTKPrp __ztk_prp_camera[] = {
  { ZTK_KEY_ROKIGL_CAMERA_BACKGROUND, 1, _rkglCameraBackgroundFromZTK, _rkglCameraBackgroundFPrint },
  { ZTK_KEY_ROKIGL_CAMERA_VIEWPORT,   1, _rkglCameraViewportFromZTK, _rkglCameraViewportFPrint },
};

static const ZTKPrp __ztk_prp_camera_viewvolume[] = {
  { ZTK_KEY_ROKIGL_CAMERA_FOVY, 1, _rkglCameraFovyFromZTK, _rkglCameraFovyFPrint },
  { ZTK_KEY_ROKIGL_CAMERA_NEAR, 1, _rkglCameraNearFromZTK, _rkglCameraNearFPrint },
  { ZTK_KEY_ROKIGL_CAMERA_FAR,  1, _rkglCameraFarFromZTK, _rkglCameraFarFPrint },
};

static const ZTKPrp __ztk_prp_camera_viewframe[] = {
  { ZTK_KEY_ROKIGL_CAMERA_POS,   1, _rkglCameraPosFromZTK, NULL },
  { ZTK_KEY_ROKIGL_CAMERA_ATT,   1, _rkglCameraAttFromZTK, NULL },
  { ZTK_KEY_ROKIGL_CAMERA_ROT,  -1, _rkglCameraRotFromZTK, NULL },
  { ZTK_KEY_ROKIGL_CAMERA_FRAME, 1, _rkglCameraFrameFromZTK, _rkglCameraFrameFPrint },
};

/* read a 3D shape from a ZTK format processor. */
rkglCamera *rkglCameraFromZTK(rkglCamera *camera, ZTK *ztk)
{
  rkglCameraInit( camera );
  if( !_ZTKEvalKey( camera, NULL, ztk, __ztk_prp_camera_viewvolume ) ) return NULL;
  if( !_ZTKEvalKey( camera, NULL, ztk, __ztk_prp_camera_viewframe ) ) return NULL;
  if( !_ZTKEvalKey( camera, NULL, ztk, __ztk_prp_camera ) ) return NULL;
  rkglCameraPerspective( camera );
  return camera;
}

/* print out a camera to a file. */
void rkglCameraFPrintZTK(FILE *fp, rkglCamera *camera)
{
  if( !camera ) return;
  _ZTKPrpKeyFPrint( fp, camera, __ztk_prp_camera );
  _ZTKPrpKeyFPrint( fp, camera, __ztk_prp_camera_viewvolume );
  _ZTKPrpKeyFPrint( fp, camera, __ztk_prp_camera_viewframe );
  fprintf( fp, "\n" );
}

/* camera array */

/* allocate an array of cameras. */
bool rkglCameraArrayAlloc(rkglCameraArray *cameraarray, int num)
{
  zArrayAlloc( cameraarray, rkglCamera, num );
  return zArraySize(cameraarray) != 0 ? true : false;
}

static void *_rkglCameraArrayCameraFromZTK(void *obj, int i, void *arg, ZTK *ztk)
{
  return rkglCameraFromZTK( zArrayElemNC((rkglCameraArray*)obj,i), ztk );
}

static bool _rkglCameraArrayCameraFPrintZTK(FILE *fp, int i, void *obj)
{
  rkglCameraFPrintZTK( fp, zArrayElemNC((rkglCameraArray*)obj,i) );
  return true;
}

static const ZTKPrp __ztk_prp_cameraarray[] = {
  { ZTK_TAG_ROKIGL_CAMERA, -1, _rkglCameraArrayCameraFromZTK, _rkglCameraArrayCameraFPrintZTK },
};

/* read properties of cameras from a ZTK format processor. */
bool rkglCameraArrayFromZTK(rkglCameraArray *cameraarray, ZTK *ztk)
{
  int num;

  if( ( num = ZTKCountTag( ztk, ZTK_TAG_ROKIGL_CAMERA ) ) == 0 ) return true;
  if( !rkglCameraArrayAlloc( cameraarray, num ) ) return false;
  _ZTKEvalTag( cameraarray, NULL, ztk, __ztk_prp_cameraarray );
  return true;
}

/* print properties of cameras out to a file. */
void rkglCameraArrayFPrintZTK(FILE *fp, rkglCameraArray *cameraarray)
{
  ZTKPrp *prp;
  size_t prpnum;

  prpnum = _ZTKPrpNum( __ztk_prp_cameraarray );
  if( !( prp = ZTKPrpDup( __ztk_prp_cameraarray, prpnum ) ) ){
    ZALLOCERROR();
    return;
  }
  ZTKPrpSetNum( prp, prpnum, ZTK_TAG_ROKIGL_CAMERA, zArraySize(cameraarray) );
  ZTKPrpTagFPrint( fp, cameraarray, prp, prpnum );
  free( prp );
}

/* read multiple cameras from a ZTK format file. */
rkglCameraArray *rkglCameraArrayReadZTK(rkglCameraArray *cameraarray, const char filename[])
{
  ZTK ztk;

  ZTKInit( &ztk );
  ZTKParse( &ztk, filename );
  if( !rkglCameraArrayFromZTK( cameraarray, &ztk ) ) cameraarray = NULL;
  ZTKDestroy( &ztk );
  return cameraarray;
}

/* write multiple cameras to a ZTK format file. */
bool rkglCameraArrayWriteZTK(rkglCameraArray *cameraarray, const char filename[])
{
  FILE *fp;

  if( !( fp = zOpenZTKFile( filename, "w" ) ) ){
    ZOPENERROR( filename );
    return false;
  }
  rkglCameraArrayFPrintZTK( fp, cameraarray );
  fclose( fp );
  return true;
}
