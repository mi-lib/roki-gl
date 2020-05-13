/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture map
 */

#define GL_GLEXT_PROTOTYPES
#include <roki-gl/rkgl_texture.h>

bool rkglTextureCreate(rkglTexture *texture, char *filename, zVec3D *v)
{
  zxImage img;
  zxPixelManip pm;
  register int i, j;
  ubyte *p;
  zTri3D t1, t2;

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
  for( i=0; i<4; i++ )
    texture->v[i] = &v[i];
  zTri3DCreate( &t1, &v[0], &v[1], &v[2] );
  zTri3DCreate( &t2, &v[0], &v[2], &v[3] );
  zVec3DMid( zTri3DNorm(&t1), zTri3DNorm(&t2), &texture->normal );

  glGenTextures( 1, &texture->id );
  glBindTexture( GL_TEXTURE_2D, texture->id );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->buf );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glBindTexture( GL_TEXTURE_2D, 0 );
  return true;
}

void rkglTextureDestroy(rkglTexture *texture)
{
  glDeleteTextures( 1, &texture->id );
  free( texture->buf );
  texture->width = texture->height = 0;
}

void rkglTextureDraw(rkglTexture *texture)
{
  GLfloat color[] = { 1.0, 1.0, 1.0, 1.0 };

  glBindTexture( GL_TEXTURE_2D, texture->id );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glPolygonOffset( -1.1, 4.0 ); /* magic numbers to prevent z-fighting */
  glEnable( GL_TEXTURE_2D );
  glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color );
  rkglNormal( &texture->normal );
  glBegin( GL_QUADS );
  glTexCoord2d( 0.0, 1.0 ); rkglVertex( texture->v[0] );
  glTexCoord2d( 1.0, 1.0 ); rkglVertex( texture->v[1] );
  glTexCoord2d( 1.0, 0.0 ); rkglVertex( texture->v[2] );
  glTexCoord2d( 0.0, 0.0 ); rkglVertex( texture->v[3] );
  glEnd();
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_POLYGON_OFFSET_FILL );
  glBindTexture( GL_TEXTURE_2D, 0 );
}
