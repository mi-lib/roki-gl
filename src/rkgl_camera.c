/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work
 */

#include <roki-gl/rkgl_camera.h>

/* background color */

void rkglBGFog(rkglCamera *c, double density)
{
  glEnable( GL_FOG );
  glFogi( GL_FOG_MODE, GL_EXP );
  glFogf( GL_FOG_DENSITY, density );
  glFogfv( GL_FOG_COLOR, c->bg );
}

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

void rkglPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far)
{
  rkglVVInit();
  gluPerspective( fovy, aspect, near, far );
  rkglVVGet( c );
}

static void _rkglVVScale(rkglCamera *c, double scale, double *x, double *y)
{
  *y = 0.5 * rkglVPHeight(c) * scale;
  *x = *y * rkglVPAspect(c);
}

void rkglOrthoScale(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScale( c, scale, &x, &y );
  rkglOrtho( c, -x, x, -y, y, near, far );
}

void rkglFrustumScale(rkglCamera *c, double scale, GLdouble near, GLdouble far)
{
  GLdouble x, y;

  _rkglVVScale( c, scale, &x, &y );
  rkglFrustum( c, -x, x, -y, y, near, far );
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

static void _rkglCAPTR(double pan, double tilt, double roll);
void _rkglCAPTR(double pan, double tilt, double roll)
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
