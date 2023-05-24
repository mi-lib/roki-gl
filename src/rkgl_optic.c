/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_optic - optics.
 */

#include <roki_gl/rkgl_optic.h>

/* color material */

void rkglColor24(unsigned color)
{
  GLubyte red, green, blue;

  red   = ( color & 0xff );
  color >>= 8;
  green = ( color & 0xff );
  color >>= 8;
  blue  = ( color & 0xff );
  glColor3ub( red, green, blue );
}

void rkglMaterialRGBA(zRGB *rgb, float alpha)
{
  GLfloat color[4];

  color[3] = alpha;
  zRGB2fv( rgb, color );
  glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color );
}

void rkglMaterialWhite(void)
{
  rkglMaterialRGBA( ZRGBWHITE, 1.0 );
}

void rkglMaterialOpticalInfo(zOpticalInfo *oi)
{
  GLfloat color[4];

  color[3] = oi->alpha; /* alpha value */
  /* ambient */
  zRGB2fv( &oi->amb, color );
  glMaterialfv( GL_FRONT, GL_AMBIENT, color );
  /* diffuse */
  zRGB2fv( &oi->dif, color );
  glMaterialfv( GL_FRONT, GL_DIFFUSE, color );
  /* specular */
  zRGB2fv( &oi->spc, color );
  glMaterialfv( GL_FRONT, GL_SPECULAR, color );
  /* shininess */
  color[0] = oi->sns;
  glMaterialfv( GL_FRONT, GL_SHININESS, color );
}

void rkglMaterial(zOpticalInfo *oi)
{
  oi ? rkglMaterialOpticalInfo( oi ) : rkglMaterialWhite();
}

/* lighting */

ZDEF_STRUCT( , rkglLightPool ){
  GLenum id;
  bool in_use;
};
static rkglLightPool _rkgl_light_pool[] = {
  { GL_LIGHT0, false },
  { GL_LIGHT1, false },
  { GL_LIGHT2, false },
  { GL_LIGHT3, false },
  { GL_LIGHT4, false },
  { GL_LIGHT5, false },
  { GL_LIGHT6, false },
  { GL_LIGHT7, false },
  { 0, false },
};

static void _rkglLightCreate(rkglLight *light, uint id, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb)
{
  glEnable( ( light->id = id ) );
  rkglLightSetAmbient( light, ar, ag, ab );
  rkglLightSetDiffuse( light, dr, dg, db );
  rkglLightSetSpecular( light, sr, sg, sb );
  rkglLightLoad( light );
  rkglLightSetAttenuation( light, 1.0, 0.0, 0.0 );
  rkglLightSetSpot( light, 0, 0, 0, 180, 0 );
}

bool rkglLightCreate(rkglLight *light, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb)
{
  rkglLightPool *lp;

  for( lp=_rkgl_light_pool; lp->in_use; lp++ ){
    if( lp->id == 0 ){
      ZRUNWARN( "no more reserved light object available" );
      ZRUNWARN( "use rkglLightCreateExtra()" );
      return false;
    }
  }
  lp->in_use = true;
  _rkglLightCreate( light, lp->id, ar, ag, ab, dr, dg, db, sr, sg, sb );
  return true;
}

bool rkglLightCreateExtra(rkglLight *light, uint i, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb)
{
  if( i < 1 ){
    ZRUNERROR( "extra light identifier has to be more than or equal to 1" );
    return false;
  }
  if( i > GL_MAX_LIGHTS - 8 ){
    ZRUNERROR( "no more light object available" );
    return false;
  }
  _rkglLightCreate( light, GL_LIGHT7 + i, ar, ag, ab, dr, dg, db, sr, sg, sb );
  return true;
}

int rkglLightNum(void)
{
  int count = 0;
  rkglLightPool *lp;

  for( lp=_rkgl_light_pool; lp->in_use && lp->id!=0; lp++, count++ );
  return count;
}

void rkglLightLoad(rkglLight *light)
{
  glLightfv( light->id, GL_AMBIENT,  light->amb );
  glLightfv( light->id, GL_DIFFUSE,  light->dif );
  glLightfv( light->id, GL_SPECULAR, light->spc );
}

void rkglLightSetAttenuation(rkglLight *light, GLfloat att_const, GLfloat att_lin, GLfloat att_quad)
{
  glLightf( light->id, GL_CONSTANT_ATTENUATION,  ( light->attenuation[0] = att_const ) );
  glLightf( light->id, GL_LINEAR_ATTENUATION,    ( light->attenuation[1] = att_lin   ) );
  glLightf( light->id, GL_QUADRATIC_ATTENUATION, ( light->attenuation[2] = att_quad  ) );
}

void rkglLightSetSpot(rkglLight *light, double lx, double ly, double lz, double cutoffangle, double exponent)
{
  rkglLightSetDir( light, lx, ly, lz );
  light->cutoffangle = cutoffangle;
  light->exponent = exponent;
}

void rkglLightPut(rkglLight *light)
{
  glLightfv( light->id, GL_POSITION, light->pos );
  glLightfv( light->id, GL_SPOT_DIRECTION, light->dir );
  glLightf( light->id, GL_SPOT_CUTOFF, light->cutoffangle );
  glLightf( light->id, GL_SPOT_EXPONENT, light->exponent );
}

void rkglLightMove(rkglLight *light, GLfloat x, GLfloat y, GLfloat z)
{
  rkglLightSetPos( light, x, y, z );
  rkglLightPut( light );
}

/* fog effect */

static void _rkglFog(GLint mode, GLfloat r, GLfloat g, GLfloat b, GLfloat density)
{
  GLfloat color[4];

  color[0] = r; color[1] = g; color[2] = b; color[3] = 1;
  glEnable( GL_FOG );
  glFogi( GL_FOG_MODE, mode );
  glFogfv( GL_FOG_COLOR, color );
  glFogf( GL_FOG_DENSITY, density );
}

void rkglFogExp(GLfloat r, GLfloat g, GLfloat b, GLfloat density)
{
  _rkglFog( GL_EXP, r, g, b, density );
}

void rkglFogExp2(GLfloat r, GLfloat g, GLfloat b, GLfloat density)
{
  _rkglFog( GL_EXP2, r, g, b, density );
}

void rkglFogLinear(GLfloat r, GLfloat g, GLfloat b, GLfloat density, GLfloat start, GLfloat end)
{
  _rkglFog( GL_LINEAR, r, g, b, density );
  glFogf( GL_FOG_START, start );
  glFogf( GL_FOG_END,   end   );
}
