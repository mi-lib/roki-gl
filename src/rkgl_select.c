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
  GLdouble viewframe[16];

  rkglGetViewframe( viewframe );
  ret = gluProject( p->e[zX], p->e[zY], p->e[zZ], viewframe, c->viewvolume, c->viewport, &_x, &_y, &_z );
  *x = _x;
  *y = c->viewport[3] - _y;
  return ret;
}

int rkglUnproject(rkglCamera *c, int x, int y, double depth, zVec3D *p)
{
  GLdouble viewframe[16];

  zVec3DZero( p );
  rkglGetViewframe( viewframe );
  return gluUnProject( x, c->viewport[3]-y, depth, viewframe, c->viewvolume, c->viewport,
    &p->e[zX], &p->e[zY], &p->e[zZ] );
}

double rkglGetDepth(rkglCamera *c, int x, int y)
{
  GLfloat depth;

  glReadPixels( x, c->viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );
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

GLuint *rkglSelectionFindNearest(rkglSelectionBuffer *sb)
{
  GLuint *ns;
  int i;

  if( rkglSelectionHits(sb) <= 0 ) return NULL;
  rkglSelectionRewind( sb );
  for( ns=sb->cur, i=0; i<rkglSelectionHits(sb); i++ ){
    if( ns[1] > rkglSelectionZnear(sb) )
      ns = sb->cur;
    rkglSelectionNext( sb );
  }
  return ( sb->cur = ns );
}

int rkglSelect(rkglSelectionBuffer *sb, rkglCamera *cam, void (* scene)(void), int x, int y, int w, int h)
{
  rkglSelectionRewind( sb );
  glSelectBuffer( RKGL_SELECTION_BUF_SIZE, sb->buf );
  glRenderMode( GL_SELECT );
  glInitNames();
  glPushName( 0 );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix( x, cam->viewport[3]-y, w, h, cam->viewport );
  glMultMatrixd( cam->viewvolume );
  rkglCameraPut( cam );
  scene();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  return ( sb->hits = glRenderMode( GL_RENDER ) );
}

GLuint *rkglSelectNearest(rkglSelectionBuffer *sb, rkglCamera *cam, void (* scene)(void), int x, int y, int w, int h)
{
  rkglSelect( sb, cam, scene, x, y, w, h );
  return rkglSelectionFindNearest( sb );
}

/* for debug */

void rkglSelectionPrintName(rkglSelectionBuffer *sb)
{
  uint i;

  printf( "%d", rkglSelectionName(sb,0) );
  for( i=1; i<rkglSelectionNameSize(sb); i++ ){
    printf( "-%d", rkglSelectionName(sb,i) );
  }
}

void rkglSelectionPrint(rkglSelectionBuffer *sb)
{
  int i;

  printf( "\n%d hits.\n", rkglSelectionHits(sb) );
  rkglSelectionRewind( sb );
  for( i=0; i<rkglSelectionHits(sb); i++ ){
    printf( "number of stacks: %d\n", rkglSelectionNameSize(sb) );
    printf( " z near = %g\n", rkglSelectionZnearDepth(sb) );
    printf( " z far  = %g\n", rkglSelectionZfarDepth(sb) );
    printf( " name: " );
    rkglSelectionPrintName( sb );
    printf( "\n" );
    rkglSelectionNext( sb );
  }
}
