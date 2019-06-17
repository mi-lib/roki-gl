/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_select - object selection
 */

#include <roki-gl/rkgl_select.h>

/* project / unproject */

int rkglProject(rkglCamera *c, zVec3D *p, int *x, int *y)
{
  double _x, _y, _z;
  GLint ret;

  ret = gluProject( p->e[zX], p->e[zY], p->e[zZ], c->ca, c->vv, c->vp, &_x, &_y, &_z );
  *x = _x;
  *y = c->vp[3] - _y;
  return ret;
}

int rkglUnproject(rkglCamera *c, int x, int y, double depth, zVec3D *p)
{
  zVec3DZero( p );
  return gluUnProject( x, c->vp[3]-y, depth, c->ca, c->vv, c->vp,
    &p->e[zX], &p->e[zY], &p->e[zZ] );
}

double rkglGetDepth(rkglCamera *c, int x, int y)
{
  GLfloat depth;

  glReadPixels( x, c->vp[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );
  return (double)depth;
}

int rkglPickAndUnproject(rkglCamera *c, int x, int y, zVec3D *p)
{
  return rkglUnproject( c, x, y, rkglGetDepth( c, x, y ), p );
}

/* pick */

int rkglPick(rkglCamera *c, void (* scene)(void), GLuint selbuf[], size_t size, int x, int y, int w, int h)
{
  glSelectBuffer( size, selbuf );
  glRenderMode( GL_SELECT );
  glInitNames();
  glPushName( 0 );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix( x, c->vp[3]-y, w, h, c->vp );
  glMultMatrixd( c->vv );
  rkglCALoad( c );
  scene();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  return glRenderMode( GL_RENDER );
}

GLuint *rkglFindNearside(GLuint selbuf[], int hits)
{
  GLuint *ptr, *ns;
  GLuint zmin;
  register int i;

  if( hits <= 0 || selbuf[0] == 0 ) return NULL;
  zmin = selbuf[1];
  for( ns=ptr=selbuf, i=0; i<hits; i++ ){
    if( ptr[1] < zmin ){
      zmin = ptr[1];
      ns = ptr;
    }
    ptr += ptr[0] + 3;
  }
  return ns;
}
