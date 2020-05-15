/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture map
 */

#include <roki-gl/rkgl_texture.h>

/* read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename)
{
  zxImage img;
  zxPixelManip pm;
  register int i, j;
  ubyte *p;

  if( !zxdisplay ) zxInit();
  if( !zxImageReadFile( &img, filename ) ) return false;
  if( !( texture->buf = zAlloc( ubyte, img.width*img.height*3 ) ) ) return false;
  zxPixelManipSet( &pm, zxdepth );
  for( p=texture->buf, i=0; i<img.height; i++ )
    for( j=0; j<img.width; j++ ){
      zxImageCellRGB( &img, &pm, j, i, p, p+1, p+2 );
      p += 3;
    }
  texture->width = img.width;
  texture->height = img.height;
  zxImageDestroy( &img );

  glGenTextures( 1, &texture->id );
  glBindTexture( GL_TEXTURE_2D, texture->id );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->buf );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glBindTexture( GL_TEXTURE_2D, 0 );
  return true;
}
