/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#include <roki_gl/rkgl_camera.h>

/* viewport */

void rkglCameraLoadViewport(rkglCamera *c)
{
  glViewport( c->viewport[0], c->viewport[1], c->viewport[2], c->viewport[3] );
  glScissor( c->viewport[0], c->viewport[1], c->viewport[2], c->viewport[3] );
  glClearColor( c->background[0], c->background[1], c->background[2], c->background[3] );
}

void rkglCameraSetViewport(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h)
{
  c->viewport[0] = x; /* x */
  c->viewport[1] = y; /* y */
  c->viewport[2] = w; /* width */
  c->viewport[3] = h; /* height */
  rkglCameraLoadViewport( c );
}

void rkglCameraGetViewport(rkglCamera *c)
{
  glGetIntegerv( GL_VIEWPORT, c->viewport );
}

/* view volume */

void rkglInitViewvolume(void)
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
}

void rkglCameraLoadViewvolume(rkglCamera *c)
{
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd( c->viewvolume );
}

void rkglCameraGetViewvolume(rkglCamera *c)
{
  glGetDoublev( GL_PROJECTION_MATRIX, c->viewvolume );
}

void rkglCameraSetOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglInitViewvolume();
  glOrtho( left, right, bottom, top, near, far );
  rkglCameraGetViewvolume( c );
}

void rkglCameraSetFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglInitViewvolume();
  glFrustum( left, right, bottom, top, near, far );
  rkglCameraGetViewvolume( c );
}

void rkglCameraSetOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglCameraSetOrtho( c, -x, x, -y, y, near, far );
}

void rkglCameraSetFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglCameraSetFrustum( c, -x, x, -y, y, near, far );
}

static void _rkglCameraFitViewvolumeWidth(rkglCamera *c, double width, double *x, double *y)
{
  *x = 0.5 * width;
  *y = *x / rkglCameraViewportAspectRatio(c);
}

static void _rkglCameraFitViewvolumeHeight(rkglCamera *c, double height, double *x, double *y)
{
  *y = 0.5 * height;
  *x = *y * rkglCameraViewportAspectRatio(c);
}

static void _rkglCameraScaleViewvolumeWidth(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeWidth( c, rkglCameraViewportWidth(c) * scale, x, y );
}

static void _rkglCameraScaleViewvolumeHeight(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglCameraFitViewvolumeHeight( c, rkglCameraViewportHeight(c) * scale, x, y );
}

void rkglCameraScaleOrthoWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( c, scale, &x, &y );
  rkglCameraSetOrthoCenter( c, x, y, near, far );
}

void rkglCameraScaleFrustumWidth(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeWidth( c, scale, &x, &y );
  rkglCameraSetFrustumCenter( c, x, y, near, far );
}

void rkglCameraScaleOrthoHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( c, scale, &x, &y );
  rkglCameraSetOrthoCenter( c, x, y, near, far );
}

void rkglCameraScaleFrustumHeight(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglCameraScaleViewvolumeHeight( c, scale, &x, &y );
  rkglCameraSetFrustumCenter( c, x, y, near, far );
}

void rkglCameraSetPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far)
{
  rkglInitViewvolume();
  gluPerspective( fovy, aspect, near, far );
  rkglCameraGetViewvolume( c );
}

void rkglCameraFitFrustumToViewport(rkglCamera *cam, int w, int h, double width, double near, double far)
{
  double x, y;

  rkglCameraSetViewport( cam, 0, 0, w, h );
  _rkglCameraFitViewvolumeWidth( cam, width, &x, &y );
  rkglCameraSetFrustumCenter( cam, x, y, near, far );
}

/* camera angle */

void rkglCameraLoadViewframe(rkglCamera *c)
{
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd( c->viewframe );
}

void rkglCameraGetViewframe(rkglCamera *c)
{
  glGetDoublev( GL_MODELVIEW_MATRIX, c->viewframe );
}

zFrame3D *rkglCameraViewframeToFrame3D(rkglCamera *cam, zFrame3D *f)
{
  zMat3D m0;

  zMat3DCreate( zFrame3DAtt(f),
    cam->viewframe[0], cam->viewframe[1], cam->viewframe[2],
    cam->viewframe[4], cam->viewframe[5], cam->viewframe[6],
    cam->viewframe[8], cam->viewframe[9], cam->viewframe[10] );
  zVec3DCreate( zFrame3DPos(f), -cam->viewframe[12], -cam->viewframe[13], -cam->viewframe[14] );
  zMulMat3DVec3DDRC( zFrame3DAtt(f), zFrame3DPos(f) );
  zMat3DCreate( &m0,
    0, 0, 1,
    1, 0, 0,
    0, 1, 0 );
  zMulMat3DMat3DTDRC( zFrame3DAtt(f), &m0 );
  return f;
}

zVec3D *rkglCameraGetViewVec(rkglCamera *cam, zVec3D *v)
{
  _zVec3DCreate( v, cam->viewframe[2], cam->viewframe[6], cam->viewframe[10] );
  return v;
}

void rkglInitViewframe(void)
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

void rkglCameraAlignViewframe(rkglCamera *c)
{
  c->viewframe[2] = c->viewframe[4] = c->viewframe[9] = c->viewframe[15] = 1;
  c->viewframe[0] = c->viewframe[1] = c->viewframe[3] = c->viewframe[5] = c->viewframe[6] = c->viewframe[7] =
  c->viewframe[8] = c->viewframe[10]= c->viewframe[11]= c->viewframe[12]= c->viewframe[13]= c->viewframe[14]= 0;
  rkglCameraLoadViewframe( c );
}

static void _rkglCameraRotatePanTiltRoll(double pan, double tilt, double roll)
{
  glRotated( -tilt, 0.0, 1.0, 0.0 );
  glRotated( -roll, 1.0, 0.0, 0.0 );
  glRotated( -pan,  0.0, 0.0, 1.0 );
}

void rkglCameraSetViewframe(rkglCamera *c, double x, double y, double z, double pan, double tilt, double roll)
{
  rkglCameraAlignViewframe( c );
  _rkglCameraRotatePanTiltRoll( pan, tilt, roll );
  glTranslated( -x, -y, -z );
  rkglCameraGetViewframe( c );
}

void rkglCameraSetPanTiltRoll(rkglCamera *c, double pan, double tilt, double roll)
{
  double x, y, z;

  rkglInvTranslated( c->viewframe, &x, &y, &z );
  rkglCameraLoadViewframe( c );
  glTranslated( x, y, z );
  _rkglCameraRotatePanTiltRoll( pan, tilt, roll );
  glTranslated( -x, -y, -z );
  rkglCameraGetViewframe( c );
}

void rkglCameraLockonAndSetPanTiltRoll(rkglCamera *c, double pan, double tilt, double roll)
{
  rkglCameraLoadViewframe( c );
  _rkglCameraRotatePanTiltRoll( pan, tilt, roll );
  rkglCameraGetViewframe( c );
}

void rkglCameraRotate(rkglCamera *c, double angle, double x, double y, double z)
{
  rkglInitViewframe();
  glRotated( angle, x, y, z );
  glMultMatrixd( c->viewframe );
  rkglCameraGetViewframe( c );
}

void rkglCameraLockonAndRotate(rkglCamera *c, double angle, double x, double y, double z)
{
  double ax, ay, az;

  ax = c->viewframe[0]*x + c->viewframe[1]*y + c->viewframe[2]*z;
  ay = c->viewframe[4]*x + c->viewframe[5]*y + c->viewframe[6]*z;
  az = c->viewframe[8]*x + c->viewframe[9]*y + c->viewframe[10]*z;
  rkglCameraLoadViewframe( c );
  glRotated( angle, ax, ay, az );
  rkglCameraGetViewframe( c );
}

void rkglCameraMove(rkglCamera *c, double x, double y, double z)
{
  rkglCameraLoadViewframe( c );
  glTranslated( -x, -y, -z );
  rkglCameraGetViewframe( c );
}

void rkglCameraRelMove(rkglCamera *c, double x, double y, double z)
{
  rkglInitViewframe();
  glTranslated( -y, -z, -x );
  glMultMatrixd( c->viewframe );
  rkglCameraGetViewframe( c );
}

void rkglCameraLookAt(rkglCamera *c, GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
{
  rkglInitViewframe();
  gluLookAt( eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz );
  rkglCameraGetViewframe( c );
}

void rkglCameraRelMoveLeft(rkglCamera *cam, double d){  rkglCameraRelMove( cam, 0,-d, 0 ); }
void rkglCameraRelMoveRight(rkglCamera *cam, double d){ rkglCameraRelMove( cam, 0, d, 0 ); }
void rkglCameraRelMoveUp(rkglCamera *cam, double d){    rkglCameraRelMove( cam, 0, 0, d ); }
void rkglCameraRelMoveDown(rkglCamera *cam, double d){  rkglCameraRelMove( cam, 0, 0,-d ); }
void rkglCameraZoomIn(rkglCamera *cam, double d){       rkglCameraRelMove( cam,-d, 0, 0 ); }
void rkglCameraZoomOut(rkglCamera *cam, double d){      rkglCameraRelMove( cam, d, 0, 0 ); }

/* camera */

rkglCamera *rkglCameraCopy(rkglCamera *src, rkglCamera *dest)
{
  if( !src )
    if( !( src = rkgl_default_cam ) ){
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

rkglCamera *rkgl_default_cam;
double rkgl_default_vv_width;
double rkgl_default_vv_near;
double rkgl_default_vv_far;
double rkgl_default_key_delta_trans;
double rkgl_default_key_delta_angle;

void rkglSetDefaultCallbackParam(rkglCamera *cam, double width, double near, double far, double dl, double da)
{
  rkgl_default_cam = cam;
  rkgl_default_vv_width = width;
  rkgl_default_vv_near = near;
  rkgl_default_vv_far = far;
  rkgl_default_key_delta_trans = dl;
  rkgl_default_key_delta_angle = da;
}
