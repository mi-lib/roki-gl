/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_superimpose - special effects to superimpose images.
 */

#include <roki_gl/rkgl_superimpose.h>

/* abstract contour of objects in the current camera view. */
void rkglContour(rkglCamera *camera)
{
  bool lighting_is_enabled;
  int width, height, i, j, pos;

  if( !camera->_depthbuffer ) return;
  rkglSaveLighting( &lighting_is_enabled );
  /* set projection matrix to see viewport */
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho( 0, camera->viewport[2], 0, camera->viewport[3], -1, 1 );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  /* plot contour points */
  glBegin( GL_POINTS );
  width = camera->viewport[2] - 1;
  height = camera->viewport[3] - 1;
  for( i=1; i<height; i++ )
    for( j=1; j<width; j++ ){
      pos = i*camera->viewport[2]+j;
      if( ( camera->_depthbuffer[pos-1] == 0xff && camera->_depthbuffer[pos] != 0xff ) ||
          ( camera->_depthbuffer[pos] != 0xff && camera->_depthbuffer[pos+1] == 0xff ) ||
          ( camera->_depthbuffer[pos-camera->viewport[2]] == 0xff && camera->_depthbuffer[pos] != 0xff ) ||
          ( camera->_depthbuffer[pos] != 0xff && camera->_depthbuffer[pos+camera->viewport[2]] == 0xff ) )
        glVertex2i( j, i );
    }
  glEnd();
  /* reset projection matrix */
  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  rkglLoadLighting( lighting_is_enabled );
}
