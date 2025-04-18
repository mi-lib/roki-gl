/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#include <roki_gl/rkgl_camera.h>

/* viewport */

/* set viewport of a camera. */
void rkglCameraSetViewport(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h)
{
  c->viewport[0] = x; /* x */
  c->viewport[1] = y; /* y */
  c->viewport[2] = w; /* width */
  c->viewport[3] = h; /* height */
  rkglCameraLoadViewport( c );
}

/* load viewport of a camera to the current render. */
void rkglCameraLoadViewport(rkglCamera *c)
{
  glViewport( c->viewport[0], c->viewport[1], c->viewport[2], c->viewport[3] );
  glScissor( c->viewport[0], c->viewport[1], c->viewport[2], c->viewport[3] );
  glClearColor( c->background[0], c->background[1], c->background[2], c->background[3] );
}

/* get and store the current viewport to a camera. */
void rkglCameraGetViewport(rkglCamera *c)
{
  glGetIntegerv( GL_VIEWPORT, c->viewport );
}

/* view volume */

/* reset viewvolume of the current render. */
void rkglResetViewvolume(void)
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
}

/* load viewvolume of a camera to the current render. */
void rkglCameraLoadViewvolume(rkglCamera *c)
{
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd( c->viewvolume );
}

/* get and store the current viewvolume to a camera. */
void rkglCameraGetViewvolume(rkglCamera *c)
{
  glGetDoublev( GL_PROJECTION_MATRIX, c->viewvolume );
}

/* set viewvolume of a camera that produces a parallel projection. */
void rkglCameraSetOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglResetViewvolume();
  glOrtho( left, right, bottom, top, near, far );
  rkglCameraGetViewvolume( c );
}

/* set viewvolume of a camera that produces a perspective projection. */
void rkglCameraSetFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglResetViewvolume();
  glFrustum( left, right, bottom, top, near, far );
  rkglCameraGetViewvolume( c );
}

/* set viewvolume of a camera that produces a parallel projection centering a specified point. */
void rkglCameraSetOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglCameraSetOrtho( c, -x, x, -y, y, near, far );
}

/* set viewvolume of a camera that produces a perspective projection centering a specified point. */
void rkglCameraSetFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglCameraSetFrustum( c, -x, x, -y, y, near, far );
}

/* compute corner coordinates of viewplane of a camera that fit to width of the current viewport. */
static void _rkglCameraFitViewvolumeWidth(rkglCamera *c, double width, double *x, double *y)
{
  *y = ( *x = 0.5 * width ) / rkglCameraViewportAspectRatio(c);
}

/* compute corner coordinates of viewplane of a camera that fit to height of the current viewport. */
static void _rkglCameraFitViewvolumeHeight(rkglCamera *c, double height, double *x, double *y)
{
  *x = ( *y = 0.5 * height ) * rkglCameraViewportAspectRatio(c);
}

/* compute corner coordinates of viewplane of a camera by scaling its width to that of viewport. */
static void _rkglCameraScaleViewvolumeWidth(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeWidth( c, rkglCameraViewportWidth(c) * scale, x, y );
}

/* compute corner coordinates of viewplane of a camera by scaling its height to that of viewport. */
static void _rkglCameraScaleViewvolumeHeight(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeHeight( c, rkglCameraViewportHeight(c) * scale, x, y );
}

/* scale viewvolume of a camera that produces parallel projection as to fit width to that of viewport. */
void rkglCameraScaleOrthoWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( c, scale, &x, &y );
  rkglCameraSetOrthoCenter( c, x, y, near, far );
}

/* scale viewvolume of a camera that produces perspective projection as to fit width to that of viewport. */
void rkglCameraScaleFrustumWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( c, scale, &x, &y );
  rkglCameraSetFrustumCenter( c, x, y, near, far );
}

/* scale viewvolume of a camera that produces parallel projection as to fit height to that of viewport. */
void rkglCameraScaleOrthoHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( c, scale, &x, &y );
  rkglCameraSetOrthoCenter( c, x, y, near, far );
}

/* scale viewvolume of a camera that produces perspective projection as to fit height to that of viewport. */
void rkglCameraScaleFrustumHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( c, scale, &x, &y );
  rkglCameraSetFrustumCenter( c, x, y, near, far );
}

/* set viewvolume of a camera that produces perspective projection from field of view and aspect ratio. */
void rkglCameraSetPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far)
{
  rkglResetViewvolume();
  gluPerspective( fovy, aspect, near, far );
  rkglCameraGetViewvolume( c );
}

/* fit viewvolume of a camera that produces perspective projection to viewport. */
void rkglCameraFitFrustumToViewport(rkglCamera *cam, int w, int h, double width, double near, double far)
{
  double x, y;

  rkglCameraSetViewport( cam, 0, 0, w, h );
  _rkglCameraFitViewvolumeWidth( cam, width, &x, &y );
  rkglCameraSetFrustumCenter( cam, x, y, near, far );
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
}

/* set viewframe of a camera. */
void rkglCameraSetViewframe(rkglCamera *camera, double x, double y, double z, double pan, double tilt, double roll)
{
  zFrame3DFromZYX( &camera->viewframe, x, y, z, zDeg2Rad(pan), zDeg2Rad(tilt), zDeg2Rad(roll) );
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
  rkglCameraCopyViewvolume( src, dest );
  rkglCameraCopyViewframe( src, dest );
  return dest;
}

/* default camera parameters */

rkglCamera *rkgl_default_camera;

#define RKGL_DEFAULT_VV_FOVY 30.0
#define RKGL_DEFAULT_VV_NEAR  1.0
#define RKGL_DEFAULT_VV_FAR  20.0

double rkgl_default_vv_fovy = RKGL_DEFAULT_VV_FOVY;

double rkgl_default_vv_near = RKGL_DEFAULT_VV_NEAR;
double rkgl_default_vv_far  = RKGL_DEFAULT_VV_FAR;

void rkglSetDefaultCamera(rkglCamera *camera, double fovy, double near, double far)
{
  rkgl_default_camera = camera;
  rkgl_default_vv_fovy = fovy;
  rkgl_default_vv_near = near;
  rkgl_default_vv_far = far;
}
