/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work.
 */

#include <roki_gl/rkgl_camera.h>

/* viewport */

void rkglVPLoad(rkglCamera *c)
{
  glViewport( c->vp[0], c->vp[1], c->vp[2], c->vp[3] );
  glScissor( c->vp[0], c->vp[1], c->vp[2], c->vp[3] );
  glClearColor( c->bg[0], c->bg[1], c->bg[2], c->bg[3] );
}

void rkglVPCreate(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h)
{
  c->vp[0] = x; /* x */
  c->vp[1] = y; /* y */
  c->vp[2] = w; /* width */
  c->vp[3] = h; /* height */
  rkglVPLoad( c );
}

void rkglVPGet(rkglCamera *c)
{
  glGetIntegerv( GL_VIEWPORT, c->vp );
}

/* view volume */

void rkglVVLoad(rkglCamera *c)
{
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd( c->vv );
}

void rkglVVGet(rkglCamera *c)
{
  glGetDoublev( GL_PROJECTION_MATRIX, c->vv );
}

void rkglVVInit(void)
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
}

void rkglOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglVVInit();
  glOrtho( left, right, bottom, top, near, far );
  rkglVVGet( c );
}

void rkglFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far)
{
  rkglVVInit();
  glFrustum( left, right, bottom, top, near, far );
  rkglVVGet( c );
}

void rkglOrthoCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglOrtho( c, -x, x, -y, y, near, far );
}

void rkglFrustumCenter(rkglCamera *c, GLdouble x, GLdouble y, GLdouble near, GLdouble far)
{
  rkglFrustum( c, -x, x, -y, y, near, far );
}

static void _rkglVVFitW2XY(rkglCamera *c, double width, double *x, double *y)
{
  *x = 0.5 * width;
  *y = *x / rkglVPAspect(c);
}

static void _rkglVVFitH2XY(rkglCamera *c, double height, double *x, double *y)
{
  *y = 0.5 * height;
  *x = *y * rkglVPAspect(c);
}

static void _rkglVVScaleW2XY(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglVVFitW2XY( c, rkglVPWidth(c) * scale, x, y );
}

static void _rkglVVScaleH2XY(rkglCamera *c, double scale, double *x, double *y)
{
  _rkglVVFitH2XY( c, rkglVPHeight(c) * scale, x, y );
}

void rkglOrthoScaleW(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScaleW2XY( c, scale, &x, &y );
  rkglOrthoCenter( c, x, y, near, far );
}

void rkglFrustumScaleW(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScaleW2XY( c, scale, &x, &y );
  rkglFrustumCenter( c, x, y, near, far );
}

void rkglOrthoScaleH(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScaleH2XY( c, scale, &x, &y );
  rkglOrthoCenter( c, x, y, near, far );
}

void rkglFrustumScaleH(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScaleH2XY( c, scale, &x, &y );
  rkglFrustumCenter( c, x, y, near, far );
}

void rkglPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far)
{
  rkglVVInit();
  gluPerspective( fovy, aspect, near, far );
  rkglVVGet( c );
}

void rkglFrustumFit2VP(rkglCamera *cam, int w, int h, double width, double near, double far)
{
  double x, y;

  rkglVPCreate( cam, 0, 0, w, h );
  _rkglVVFitW2XY( cam, width, &x, &y );
  rkglFrustumCenter( cam, x, y, near, far );
}

/* camera angle */

void rkglCALoad(rkglCamera *c)
{
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd( c->ca );
}

void rkglCAGet(rkglCamera *c)
{
  glGetDoublev( GL_MODELVIEW_MATRIX, c->ca );
}

zFrame3D *rkglCAGetFrame3D(rkglCamera *cam, zFrame3D *f)
{
  zMat3D m0;

  zMat3DCreate( zFrame3DAtt(f),
    cam->ca[0], cam->ca[1], cam->ca[2],
    cam->ca[4], cam->ca[5], cam->ca[6],
    cam->ca[8], cam->ca[9], cam->ca[10] );
  zVec3DCreate( zFrame3DPos(f), -cam->ca[12], -cam->ca[13], -cam->ca[14] );
  zMulMat3DVec3DDRC( zFrame3DAtt(f), zFrame3DPos(f) );
  zMat3DCreate( &m0,
    0, 0, 1,
    1, 0, 0,
    0, 1, 0 );
  zMulMat3DMat3DTDRC( zFrame3DAtt(f), &m0 );
  return f;
}

zVec3D *rkglCAGetViewVec(rkglCamera *cam, zVec3D *v)
{
  _zVec3DCreate( v, cam->ca[2], cam->ca[6], cam->ca[10] );
  return v;
}

void rkglCAInit(void)
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

void rkglCAAlign(rkglCamera *c)
{
  c->ca[2] = c->ca[4] = c->ca[9] = c->ca[15] = 1;
  c->ca[0] = c->ca[1] = c->ca[3] = c->ca[5] = c->ca[6] = c->ca[7] =
  c->ca[8] = c->ca[10]= c->ca[11]= c->ca[12]= c->ca[13]= c->ca[14]= 0;
  rkglCALoad( c );
}

static void _rkglCAPTR(double pan, double tilt, double roll)
{
  glRotated( -tilt, 0.0, 1.0, 0.0 );
  glRotated( -roll, 1.0, 0.0, 0.0 );
  glRotated( -pan,  0.0, 0.0, 1.0 );
}

void rkglCASet(rkglCamera *c, double x, double y, double z, double pan, double tilt, double roll)
{
  rkglCAAlign( c );
  _rkglCAPTR( pan, tilt, roll );
  glTranslated( -x, -y, -z );
  rkglCAGet( c );
}

void rkglCAPTR(rkglCamera *c, double pan, double tilt, double roll)
{
  double x, y, z;

  rkglInvTranslated( c->ca, &x, &y, &z );
  rkglCALoad( c );
  glTranslated( x, y, z );
  _rkglCAPTR( pan, tilt, roll );
  glTranslated( -x, -y, -z );
  rkglCAGet( c );
}

void rkglCALockonPTR(rkglCamera *c, double pan, double tilt, double roll)
{
  rkglCALoad( c );
  _rkglCAPTR( pan, tilt, roll );
  rkglCAGet( c );
}

void rkglCARotate(rkglCamera *c, double angle, double x, double y, double z)
{
  rkglCAInit();
  glRotated( angle, x, y, z );
  glMultMatrixd( c->ca );
  rkglCAGet( c );
}

void rkglCALockonRotate(rkglCamera *c, double angle, double x, double y, double z)
{
  double ax, ay, az;

  ax = c->ca[0]*x + c->ca[1]*y + c->ca[2]*z;
  ay = c->ca[4]*x + c->ca[5]*y + c->ca[6]*z;
  az = c->ca[8]*x + c->ca[9]*y + c->ca[10]*z;
  rkglCALoad( c );
  glRotated( angle, ax, ay, az );
  rkglCAGet( c );
}

void rkglCAMove(rkglCamera *c, double x, double y, double z)
{
  rkglCALoad( c );
  glTranslated( -x, -y, -z );
  rkglCAGet( c );
}

void rkglCARelMove(rkglCamera *c, double x, double y, double z)
{
  rkglCAInit();
  glTranslated( -y, -z, -x );
  glMultMatrixd( c->ca );
  rkglCAGet( c );
}

void rkglCALookAt(rkglCamera *c, GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
{
  rkglCAInit();
  gluLookAt( eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz );
  rkglCAGet( c );
}

void rkglCARelMoveLeft(rkglCamera *cam, double d){  rkglCARelMove( cam, 0,-d, 0 ); }
void rkglCARelMoveRight(rkglCamera *cam, double d){ rkglCARelMove( cam, 0, d, 0 ); }
void rkglCARelMoveUp(rkglCamera *cam, double d){    rkglCARelMove( cam, 0, 0, d ); }
void rkglCARelMoveDown(rkglCamera *cam, double d){  rkglCARelMove( cam, 0, 0,-d ); }
void rkglCAZoomIn(rkglCamera *cam, double d){       rkglCARelMove( cam,-d, 0, 0 ); }
void rkglCAZoomOut(rkglCamera *cam, double d){      rkglCARelMove( cam, d, 0, 0 ); }

void rkglCATiltUp(rkglCamera *cam, double angle){   rkglCARotate( cam, angle,-1, 0, 0 ); }
void rkglCATiltDown(rkglCamera *cam, double angle){ rkglCARotate( cam, angle, 1, 0, 0 ); }
void rkglCAPanLeft(rkglCamera *cam, double angle){  rkglCARotate( cam, angle, 0,-1, 0 ); }
void rkglCAPanRight(rkglCamera *cam, double angle){ rkglCARotate( cam, angle, 0, 1, 0 ); }

void rkglCAAngleUp(rkglCamera *cam, double angle){    rkglCALockonRotate( cam, angle, 1, 0, 0 ); }
void rkglCAAngleDown(rkglCamera *cam, double angle){  rkglCALockonRotate( cam, angle,-1, 0, 0 ); }
void rkglCARoundLeft(rkglCamera *cam, double angle){  rkglCALockonRotate( cam, angle, 0, 1, 0 ); }
void rkglCARoundRight(rkglCamera *cam, double angle){ rkglCALockonRotate( cam, angle, 0,-1, 0 ); }

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
