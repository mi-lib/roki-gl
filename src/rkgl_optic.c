/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_optic - optics
 */

#include <roki-gl/rkgl_optic.h>

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

void rkglMaterial(zOpticalInfo *oi)
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

/* lighting */

void rkglLightCreate(rkglLight *l, uint id, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb, GLfloat ns)
{
  GLenum light_id[] = {
    GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7,
  };

  if( id > 7 ){
    ZRUNWARN( "light over GL_LIGHT7 should be manually assigned" );
    id = 0;
  }
  glEnable( ( l->id = light_id[id] ) );

  l->amb[0] = ar; l->amb[1] = ag; l->amb[2] = ab; l->amb[3] = 1;
  l->dif[0] = dr; l->dif[1] = dg; l->dif[2] = db; l->dif[3] = 1;
  l->spc[0] = sr; l->spc[1] = sg; l->spc[2] = sb; l->spc[3] = 1;
  rkglLightLoad( l );
}

void rkglLightLoad(rkglLight *l)
{
  glLightfv( l->id, GL_AMBIENT,  l->amb );
  glLightfv( l->id, GL_DIFFUSE,  l->dif );
  glLightfv( l->id, GL_SPECULAR, l->spc );
}

void rkglLightSetPos(rkglLight *l, double x, double y, double z)
{
  l->pos[0] = x;
  l->pos[1] = y;
  l->pos[2] = z;
  l->pos[3] = 1;
  rkglLightPut( l );
}

void rkglLightPut(rkglLight *l)
{
  glLightfv( l->id, GL_POSITION, l->pos );
}
