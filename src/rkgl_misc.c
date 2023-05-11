/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_misc - miscellanies.
 */

#include <roki-gl/rkgl_misc.h>

void rkglEnableDefault(void)
{
  glEnable( GL_DEPTH_TEST );   /* depth buffer */
  glEnable( GL_NORMALIZE );    /* auto normalization of normal vectors */
  glEnable( GL_CULL_FACE );    /* dual face disabled */
  glEnable( GL_SCISSOR_TEST ); /* viewport scissor */
  glEnable( GL_BLEND );        /* alpha blend */
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
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
