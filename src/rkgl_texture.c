/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture mapping.
 */

#include <roki-gl/rkgl_texture.h>

/* color texture mapping */

/* read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename)
{
  zxImage img;
  zxPixelManip pm;
  register int i, j;
  ubyte *p;
  bool already_connected, ret = false;

  already_connected = !zxInit();
  if( !zxImageReadFile( &img, filename ) ) goto TERMINATE;
  if( !( texture->buf = zAlloc( ubyte, img.width*img.height*3 ) ) ) goto TERMINATE;
  ret = true;
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

 TERMINATE:
  if( !already_connected ) zxExit();
  return ret;
}

/* bump mapping */

/* x component of a normal vector on a bump texture */
static double _rkglTextureBumpNormalX(zxImage *img, zxPixelManip *pm, int j, int i)
{
  ubyte r1, g1, b1;
  ubyte r2, g2, b2;
  ubyte r3, g3, b3;

  if( j <= 0 ){
    zxImageCellRGB( img, pm, 0, i, &r1, &g1, &b1 );
    zxImageCellRGB( img, pm, 1, i, &r2, &g2, &b2 );
    zxImageCellRGB( img, pm, 2, i, &r3, &g3, &b3 );
    return -0.5 * (double)( r1 + g1 + b1 ) + 2 * (double)( r2 + g2 + b2 ) / 3 - (double)( r3 + g3 + b3 ) / 6;
  }
  if( j >= img->width - 1 ){
    zxImageCellRGB( img, pm, img->width-3, i, &r1, &g1, &b1 );
    zxImageCellRGB( img, pm, img->width-2, i, &r2, &g2, &b2 );
    zxImageCellRGB( img, pm, img->width-1, i, &r3, &g3, &b3 );
    return (double)( r1 + g1 + b1 ) / 6 - 2 * (double)( r2 + g2 + b2 ) / 3 + 0.5 * (double)( r3 + g3 + b3 );
  }
  zxImageCellRGB( img, pm, j-1, i, &r1, &g1, &b1 );
  zxImageCellRGB( img, pm, j+1, i, &r3, &g3, &b3 );
  return (double)( r3 + g3 + b3 - r1 - g1 - b1 ) / 6;
}

/* y component of a normal vector on a bump texture */
static double _rkglTextureBumpNormalY(zxImage *img, zxPixelManip *pm, int j, int i)
{
  ubyte r1, g1, b1;
  ubyte r2, g2, b2;
  ubyte r3, g3, b3;

  if( i <= 0 ){
    zxImageCellRGB( img, pm, j, 0, &r1, &g1, &b1 );
    zxImageCellRGB( img, pm, j, 1, &r2, &g2, &b2 );
    zxImageCellRGB( img, pm, j, 2, &r3, &g3, &b3 );
    return -0.5 * (double)( r1 + g1 + b1 ) + 2 * (double)( r2 + g2 + b2 ) / 3 - (double)( r3 + g3 + b3 ) / 6;
  }
  if( i >= img->height - 1 ){
    zxImageCellRGB( img, pm, j, img->height-3, &r1, &g1, &b1 );
    zxImageCellRGB( img, pm, j, img->height-2, &r2, &g2, &b2 );
    zxImageCellRGB( img, pm, j, img->height-1, &r3, &g3, &b3 );
    return (double)( r1 + g1 + b1 ) / 6 - 2 * (double)( r2 + g2 + b2 ) / 3 + 0.5 * (double)( r3 + g3 + b3 );
  }
  zxImageCellRGB( img, pm, j, i-1, &r1, &g1, &b1 );
  zxImageCellRGB( img, pm, j, i+1, &r3, &g3, &b3 );
  return (double)( r3 + g3 + b3 - r1 - g1 - b1 ) / 6;
}

/* convert a direction vector to a texture value */
static ubyte *_rkglTextureBumpVec(ubyte *p, double x, double y, double z)
{
  *(p  ) = 0xff * ( 0.5 * ( x + 1 ) );
  *(p+1) = 0xff * ( 0.5 * ( y + 1 ) );
  *(p+2) = 0xff * ( 0.5 * ( z + 1 ) );
  return p;
}

/* generate a normal map from a bump texture */
static bool _rkglTextureBumpNormalMap(zTexture *bump, char *filename)
{
  register int i, j, k;
  zxImage img;
  zxPixelManip pm;
  double nx, ny, nz, l;
  bool already_connected, ret = true;

  already_connected = !zxInit();
  if( zxImageReadFile( &img, filename ) == 0 ||
      img.width < 3 || img.height < 3 ) return false;
  if( !zTextureBumpAlloc( bump, img.width, img.height ) ){
    ret = false;
    goto TERMINATE;
  }
  zxPixelManipSetDefault( &pm );
  if( zIsTiny( bump->depth ) ){
    ZRUNERROR( "zero-depth bump unrenderable" );
    bump->depth = 1.0;
  }
  nz = 1.0 / bump->depth;
  for( k=0, i=0; i<img.height; i++ ){
    for( j=0; j<img.width; j++, k+=3 ){
      nx = _rkglTextureBumpNormalX( &img, &pm, j, i );
      ny = _rkglTextureBumpNormalY( &img, &pm, j, i );
      l = sqrt( nx*nx + ny*ny + nz*nz );
      _rkglTextureBumpVec( bump->buf + k, nx/l, ny/l, nz/l );
    }
  }
 TERMINATE:
  zxImageDestroy( &img );
  if( !already_connected ) zxExit();
  return ret;
}

/* generate a light map from a bump texture */
static void _rkglTextureBumpLightMap(zTexture *bump)
{
  register int i, j, k;
  int wh, hh;
  double x, y, y2;
  double xr, yr, zr;

  wh = bump->width / 2;
  hh = bump->height / 2;
  for( k=0, i=0; i<hh; i++ ){
    y = 2*(double)i/hh - 1;
    y2 = y*y + 1;
    for( j=0; j<wh; j++, k+=3 ){
      x = 2*(double)j/wh - 1;
      zr = 1.0 / sqrt( x*x + y2 );
      xr = x * zr;
      yr = y * zr;
      _rkglTextureBumpVec( bump->lbuf[0] + k, -zr, -yr,  xr );
      _rkglTextureBumpVec( bump->lbuf[1] + k,  xr, -zr, -yr );
      _rkglTextureBumpVec( bump->lbuf[2] + k, -xr, -yr, -zr );
      _rkglTextureBumpVec( bump->lbuf[3] + k,  zr, -yr, -xr );
      _rkglTextureBumpVec( bump->lbuf[4] + k,  xr,  zr,  yr );
      _rkglTextureBumpVec( bump->lbuf[5] + k,  xr, -yr,  zr );
    }
  }
}

/* create a bump map */
bool rkglTextureBumpReadFile(zTexture *bump, char *filename)
{
  static int cmap_type[] = {
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  };
  register int i;

  if( !_rkglTextureBumpNormalMap( bump, filename ) ) return false;

  glActiveTexture( RKGL_TEXTURE_BASE );
  glGenTextures( 1, &bump->id );
  glBindTexture( GL_TEXTURE_2D, bump->id );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, bump->width, bump->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump->buf );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  glActiveTexture( RKGL_TEXTURE_BUMP );
  glGenTextures( 1, &bump->id_bump );
  glBindTexture( GL_TEXTURE_2D, bump->id_bump );

  _rkglTextureBumpLightMap( bump );
  for( i=0; i<6; i++ )
    glTexImage2D( cmap_type[i], 0, GL_RGB, bump->width/2, bump->height/2, 0, GL_RGB, GL_UNSIGNED_BYTE, bump->lbuf[i] );

  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB );
  glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, RKGL_TEXTURE_BASE );
  glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE );

  glBindTexture( GL_TEXTURE_2D, 0 );
  glActiveTexture( RKGL_TEXTURE_BASE );
  return true;
}
