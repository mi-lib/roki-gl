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
GLuint rkglTextureInit(zTexture *texture, ubyte *buf)
{
  return ( texture->id = rkglTextureAssign( texture->width, texture->height, buf ) );
}

/* read an image file and make a texture data. */
bool rkglTextureReadFile(zTexture *texture, char *filename)
{
  zxImage img;
  zxPixelManip pm;
  uint i, j;
  ubyte *buf, *pt;
  bool already_connected, ret = false;

  already_connected = !zxInit();
  if( !zxImageReadFile( &img, filename ) ) goto TERMINATE;
  if( !( buf = zAlloc( ubyte, img.width*img.height*4 ) ) ){
    ZALLOCERROR();
    goto TERMINATE;
  }
  ret = true;
  zxPixelManipSet( &pm, zxdepth );
  for( pt=buf, i=0; i<img.height; i++ )
    for( j=0; j<img.width; j++, pt+=4 ){
      zxImageCellRGB( &img, &pm, j, i, pt, pt+1, pt+2 );
      *( pt + 3 ) = 0xff;
    }
  texture->width = img.width;
  texture->height = img.height;
  zxImageDestroy( &img );
  rkglTextureInit( texture, buf );
  free( buf );

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
  ubyte *buf;
  double nx, ny, nz;
  bool already_connected, ret = true;

  already_connected = !zxInit();
  if( zxImageReadFile( &img, filename ) == 0 ||
      img.width < 3 || img.height < 3 ) return false;
  if( !( buf = zAlloc( ubyte, ( bump->width = img.width ) * ( bump->height = img.height ) * 4 ) ) ){
    ZALLOCERROR();
    ret = false;
    goto TERMINATE;
  }
  zxPixelManipSetDefault( &pm );
  if( zIsTiny( bump->depth ) ){
    ZRUNWARN( "zero-depth bump unrenderable" );
    bump->depth = 1.0;
  }
  for( k=0, i=0; i<img.height; i++ ){
    for( j=0; j<img.width; j++, k+=4 ){
      zxImageNormalVec( &img, &pm, bump->depth, j, i, &nx, &ny, &nz );
      _rkglTextureBumpVec( buf + k, nx, ny, nz );
    }
  }
 TERMINATE:
  zxImageDestroy( &img );
  if( !already_connected ) zxExit();
  rkglTextureInit( bump, buf );
  free( buf );
  return ret;
}

/* generate a light map from a bump texture */
static bool _rkglTextureBumpLightMap(zTexture *bump)
{
  uint i, j, k;
  uint wh, hh;
  ubyte *buf[6];
  bool ret = true;
  double x, y, y2;
  double xr, yr, zr;

  wh = bump->width / 2;
  hh = bump->height / 2;
  for( i=0; i<6; i++ )
    if( !( buf[i] = zAlloc( ubyte, wh * hh * 4 ) ) ){
      ZALLOCERROR();
      ret = false;
    }
  if( !ret ) goto TERMINATE;
  for( k=0, i=0; i<hh; i++ ){
    y = 2*(double)i/hh - 1;
    y2 = y*y + 1.0 / ( bump->depth * bump->depth );
    for( j=0; j<wh; j++, k+=4 ){
      x = 2*(double)j/wh - 1;
      zr = 1.0 / sqrt( x*x + y2 );
      xr = x * zr;
      yr = y * zr;
      _rkglTextureBumpVec( buf[0] + k,  zr, -yr, -xr );
      _rkglTextureBumpVec( buf[1] + k, -zr, -yr,  xr );
      _rkglTextureBumpVec( buf[2] + k,  xr,  zr,  yr );
      _rkglTextureBumpVec( buf[3] + k,  xr, -zr, -yr );
      _rkglTextureBumpVec( buf[4] + k,  xr, -yr,  zr );
      _rkglTextureBumpVec( buf[5] + k, -xr, -yr, -zr );
    }
  }
 TERMINATE:
  for( i=0; i<6; i++ ){
    if( ret )
      glTexImage2D( rkgl_cubemap_id[i], 0, GL_RGBA, wh, hh, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf[i] );
    free( buf[i] );
  }
  return ret;
}

/* create a bump map */
bool rkglTextureBumpReadFile(zTexture *bump, char *filename)
{
  glActiveTexture( GL_TEXTURE0 );
  if( !_rkglTextureBumpNormalMap( bump, filename ) ) return false;

  glActiveTexture( GL_TEXTURE1 );
  glGenTextures( 1, &bump->id_bump );
  glBindTexture( GL_TEXTURE_2D, bump->id_bump );
  if( !_rkglTextureBumpLightMap( bump ) ) return false;

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
