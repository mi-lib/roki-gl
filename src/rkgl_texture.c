/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_texture - texture mapping.
 */

#include <roki_gl/rkgl_texture.h>

/* color texture mapping */

/* assign a 2D texture to GL. */
GLuint rkglTextureAssign(int width, int height, ubyte *buf)
{
  GLuint id;

  glGenTextures( 1, &id );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glBindTexture( GL_TEXTURE_2D, id );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf );
  rkglTextureSetClamp();
  rkglTextureSetFilterLinear();
  glBindTexture( GL_TEXTURE_2D, 0 );
  return id;
}

/* initialize GL parameters for a 2D texture. */
GLuint rkglTextureInit(zTexture *texture)
{
  return ( texture->id = rkglTextureAssign( texture->width, texture->height, texture->buf ) );
}

/* read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename)
{
  zxImage img;
  zxPixelManip pm;
  uint i, j;
  ubyte *pt;
  bool already_connected, ret = false;

  already_connected = !zxInit();
  if( !zxImageReadFile( &img, filename ) ) goto TERMINATE;
  if( !( texture->buf = zAlloc( ubyte, img.width*img.height*4 ) ) ) goto TERMINATE;
  ret = true;
  zxPixelManipSet( &pm, zxdepth );
  for( pt=texture->buf, i=0; i<img.height; i++ )
    for( j=0; j<img.width; j++, pt+=4 ){
      zxImageCellRGB( &img, &pm, j, i, pt, pt+1, pt+2 );
      *( pt + 3 ) = 0xff;
    }
  texture->width = img.width;
  texture->height = img.height;
  zxImageDestroy( &img );
  rkglTextureInit( texture );
  zFree( texture->buf );

 TERMINATE:
  if( !already_connected ) zxExit();
  return ret;
}

/* units for multitexture */

static int rkgl_texture_unit_num = 0;

/* initialize texture units. */
void rkglTextureInitUnit(void)
{
  rkgl_texture_unit_num = 0;
}

/* assign a new texture unit. */
GLint rkglTextureNewUnit(void)
{
  int n;

  glGetIntegerv( GL_MAX_TEXTURE_UNITS, &n );
  if( rkgl_texture_unit_num >= n ){
    ZRUNERROR( "no more texture units available" );
    return -1;
  }
  return GL_TEXTURE0 + rkgl_texture_unit_num++;
}

/* frame buffer and render buffer for off-screan rendering */

/* attach a 2D texture to a frame buffer. */
GLuint rkglFramebufferAttachTexture(GLuint texid)
{
  GLuint fbid;

  glGenFramebuffersEXT( 1, &fbid );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbid );
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texid, 0 );
  if( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ) != GL_FRAMEBUFFER_COMPLETE_EXT )
    ZRUNWARN( "the current framebuffer status is unsupported" );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
  return fbid;
}

/* attach a render buffer to a frame buffer. */
GLuint rkglFramebufferAttachRenderbuffer(int width, int height)
{
  GLuint rbid;

  glGenRenderbuffersEXT( 1, &rbid );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rbid );
  /* to use depthbuffer as a render buffer */
  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height );
  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbid );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
  return rbid;
}

/* cube mapping */

const GLenum rkgl_cubemap_id[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

/* generate a cube map. */
void rkglTextureGenCubeMap(int width, int height)
{
  int i;

  for( i=0; i<6; i++ )
    glTexImage2D( rkgl_cubemap_id[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

/* projection mapping */

/* generate a square texture map for projection mapping to an object. */
void rkglTextureGenProjectionObject(void)
{
  GLfloat param[] = { 0.0, 0.0, 0.0, 0.0 };

  glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
  glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
  glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
  glTexGenf( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
  param[0] = 1.0; glTexGenfv( GL_S, GL_OBJECT_PLANE, param ); param[0] = 0.0;
  param[1] = 1.0; glTexGenfv( GL_T, GL_OBJECT_PLANE, param ); param[1] = 0.0;
  param[2] = 1.0; glTexGenfv( GL_R, GL_OBJECT_PLANE, param ); param[2] = 0.0;
  param[3] = 1.0; glTexGenfv( GL_Q, GL_OBJECT_PLANE, param ); param[3] = 0.0;
}

/* generate a square texture map for projection mapping to an eye view. */
void rkglTextureGenProjectionEye(void)
{
  GLdouble param[] = { 0.0, 0.0, 0.0, 0.0 };

  glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  param[0] = 1.0; glTexGendv( GL_S, GL_EYE_PLANE, param ); param[0] = 0.0;
  param[1] = 1.0; glTexGendv( GL_T, GL_EYE_PLANE, param ); param[1] = 0.0;
  param[2] = 1.0; glTexGendv( GL_R, GL_EYE_PLANE, param ); param[2] = 0.0;
  param[3] = 1.0; glTexGendv( GL_Q, GL_EYE_PLANE, param ); param[3] = 0.0;
}

/* enable projection mapping of a 2D texture. */
void rkglTextureEnableProjection(void)
{
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_TEXTURE_GEN_S );
  glEnable( GL_TEXTURE_GEN_T );
  glEnable( GL_TEXTURE_GEN_R );
  glEnable( GL_TEXTURE_GEN_Q );
}

/* disable projection mapping of a 2D texture. */
void rkglTextureDisableProjection(void)
{
  glDisable( GL_TEXTURE_GEN_S );
  glDisable( GL_TEXTURE_GEN_T );
  glDisable( GL_TEXTURE_GEN_R );
  glDisable( GL_TEXTURE_GEN_Q );
  glDisable( GL_TEXTURE_2D );
}

/* bump mapping */

/* x component of a normal vector on a bump texture */
static double _rkglTextureBumpNormalX(zxImage *img, zxPixelManip *pm, uint j, uint i)
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
  if( j + 1 >= img->width ){
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
static double _rkglTextureBumpNormalY(zxImage *img, zxPixelManip *pm, uint j, uint i)
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
  if( i + 1 >= img->height ){
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
  *(p+3) = 0x00;
  return p;
}

/* generate a normal map from a bump texture */
static bool _rkglTextureBumpNormalMap(zTexture *bump, char *filename)
{
  uint i, j, k;
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
    for( j=0; j<img.width; j++, k+=4 ){
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
  uint i, j, k;
  uint wh, hh;
  double x, y, y2;
  double xr, yr, zr;

  wh = bump->width / 2;
  hh = bump->height / 2;
  for( k=0, i=0; i<hh; i++ ){
    y = 2*(double)i/hh - 1;
    y2 = y*y + 1;
    for( j=0; j<wh; j++, k+=4 ){
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
  int i;

  if( !_rkglTextureBumpNormalMap( bump, filename ) ) return false;

  glActiveTexture( GL_TEXTURE0 );
  glGenTextures( 1, &bump->id );
  glBindTexture( GL_TEXTURE_2D, bump->id );
  rkglTextureInit( bump );

  glActiveTexture( GL_TEXTURE1 );
  glGenTextures( 1, &bump->id_bump );
  glBindTexture( GL_TEXTURE_2D, bump->id_bump );
  _rkglTextureBumpLightMap( bump );
  for( i=0; i<6; i++ )
    glTexImage2D( cmap_type[i], 0, GL_RGBA, bump->width/2, bump->height/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, bump->lbuf[i] );

  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  rkglTextureSetCombine();
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGBA );
  glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0 );
  glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE1 );

  rkglTextureUnbind();
  return true;
}
