/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_select - object selection.
 */

#include <roki_gl/rkgl_select.h>

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

int rkglPickPoint(rkglCamera *c, int x, int y, zVec3D *p)
{
  return rkglUnproject( c, x, y, rkglGetDepth( c, x, y ), p );
}

/* select */

void rkglSelectionInit(rkglSelectionBuffer *sb)
{
  sb->cur = sb->buf;
  sb->buf[0] = 0;
  sb->hits = 0;
}

int rkglSelect(rkglSelectionBuffer *sb, rkglCamera *cam, void (* scene)(void), int x, int y, int w, int h)
{
  sb->cur = sb->buf;
  glSelectBuffer( RKGL_SELECTION_BUF_SIZE, sb->buf );
  glRenderMode( GL_SELECT );
  glInitNames();
  glPushName( 0 );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix( x, cam->vp[3]-y, w, h, cam->vp );
  glMultMatrixd( cam->vv );
  rkglCALoad( cam );
  scene();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  return ( sb->hits = glRenderMode( GL_RENDER ) );
}

GLuint *rkglSelectNearest(rkglSelectionBuffer *sb)
{
  GLuint *ns;
  int i;

  if( sb->hits <= 0 ) return NULL;
  rkglSelectionRewind( sb );
  for( ns=sb->cur, i=0; i<sb->hits; i++ ){
    if( ns[1] > rkglSelectionZnear(sb) )
      ns = sb->cur;
    rkglSelectionNext( sb );
  }
  return ( sb->cur = ns );
}

/* for debug */

void rkglSelectionPrintName(rkglSelectionBuffer *sb)
{
  int i;

  printf( "%d", rkglSelectionName(sb,0) );
  for( i=1; i<rkglSelectionNameSize(sb); i++ ){
    printf( "-%d", rkglSelectionName(sb,i) );
  }
}

void rkglSelectionPrint(rkglSelectionBuffer *sb)
{
  int i;

  printf( "\n%d hits.\n", sb->hits );
  rkglSelectionRewind( sb );
  for( i=0; i<sb->hits; i++ ){
    printf( "number of stacks: %d\n", rkglSelectionNameSize(sb) );
    printf( " z near = %g\n", rkglSelectionZnearDepth(sb) );
    printf( " z far  = %g\n", rkglSelectionZfarDepth(sb) );
    printf( " name: " );
    rkglSelectionPrintName( sb );
    printf( "\n" );
    rkglSelectionNext( sb );
  }
}
