/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_misc - miscellanies.
 */

#include <roki_gl/rkgl_misc.h>

void rkglEnableDefault(void)
{
  glEnable( GL_DEPTH_TEST );   /* depth buffer */
  glEnable( GL_NORMALIZE );    /* auto normalization of normal vectors */
  glEnable( GL_CULL_FACE );    /* dual face disabled */
  glEnable( GL_SCISSOR_TEST ); /* viewport scissor */
  glEnable( GL_BLEND );        /* alpha blend */
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/* buf has to have size of width x height x 3. */
void rkglReadBuffer(GLuint type, int x, int  y, int width, int height, ubyte *buf)
{
  glReadBuffer( GL_FRONT );
  glReadPixels( x, y, width, height, type, GL_UNSIGNED_BYTE, buf );
}

/* matrix operation */

void rkglInvTranslated(double m[], double *x, double *y, double *z)
{
  *x = -( m[0]*m[12] + m[1]*m[13] + m[2]*m[14] );
  *y = -( m[4]*m[12] + m[5]*m[13] + m[6]*m[14] );
  *z = -( m[8]*m[12] + m[9]*m[13] + m[10]*m[14]);
}

void rkglMultInvMatrixd(double m[])
{ /* NOTE: perspective transformation not considered. */
  double im[16];

  im[0] = m[0];  im[1] = m[4];  im[2] = m[8];  im[3] = m[3];
  im[4] = m[1];  im[5] = m[5];  im[6] = m[9];  im[7] = m[7];
  im[8] = m[2];  im[9] = m[6];  im[10]= m[10]; im[11]= m[11];
  rkglInvTranslated( m, &im[12], &im[13], &im[14] );
  im[15]= m[15];
  glMultMatrixd( im );
}

void rkglXformInvd(double m[], double p[], double px[])
{
  double dp[3];

  dp[0] = p[0] - m[12];
  dp[1] = p[1] - m[13];
  dp[2] = p[2] - m[14];
  px[0] = m[0] * dp[0] + m[1] * dp[1] + m[2] * dp[2];
  px[1] = m[4] * dp[0] + m[5] * dp[1] + m[6] * dp[2];
  px[2] = m[8] * dp[0] + m[9] * dp[1] + m[10]* dp[2];
}

/* translate coordinates. */
void rkglTranslate(zVec3D *v)
{
  glTranslated( v->e[zX], v->e[zY], v->e[zZ] );
}

/* transform coordinates. */
void rkglXform(zFrame3D *f)
{
  GLdouble m[16];

  zMat3DCol( zFrame3DAtt(f), 0, (zVec3D*)&m[0] );
  zMat3DCol( zFrame3DAtt(f), 1, (zVec3D*)&m[4] );
  zMat3DCol( zFrame3DAtt(f), 2, (zVec3D*)&m[8] );
  zVec3DCopy( zFrame3DPos(f), (zVec3D*)&m[12] );
  m[3]=m[7]=m[11]=0.0; m[15]=1.0;
  glMultMatrixd( m );
}

/* inverse transform coordinates. */
void rkglXformInv(zFrame3D *f)
{
  zFrame3D invframe;
  rkglXform( zFrame3DInv( f, &invframe ) );
}

/* display list */

int rkglBeginList(void)
{
  int result;

  result = glGenLists( 1 );
  glNewList( result, GL_COMPILE );
  return result;
}

void rkglDeleteList(int id)
{
  if( glIsList( id ) ) glDeleteLists( id, 1 );
}

/* GLEW */

#ifdef __ROKI_GL_USE_GLEW
bool rkglInitGLEW(void)
{
  GLenum err;

  if( ( err = glewInit() ) != GLEW_OK ){
    fprintf( stderr, "Error: %s\n", glewGetErrorString( err ) );
    return false;
  }
  return true;
}
#endif /* __ROKI_GL_USE_GLEW */
