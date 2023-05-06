/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_optic - optics
 */

#ifndef __RKGL_OPTIC_H__
#define __RKGL_OPTIC_H__

#include <zeo/zeo_render_optic.h>
#include <roki-gl/rkgl_misc.h>

__BEGIN_DECLS

/* color material */

#define rkglRGB(rgb)    glColor3f( (rgb)->r, (rgb)->g, (rgb)->b )
#define rkglRGBA(rgb,a) glColor4f( (rgb)->r, (rgb)->g, (rgb)->b, (a) )

__EXPORT void rkglColor24(unsigned color);

#define rkglColorWhite() glColor3ub( 0xff, 0xff, 0xff )

__EXPORT void rkglMaterialRGBA(zRGB *rgb, float alpha);
__EXPORT void rkglMaterialWhite(void);
__EXPORT void rkglMaterialOpticalInfo(zOpticalInfo *oi);
__EXPORT void rkglMaterial(zOpticalInfo *oi);

/* lighting */

typedef struct{
  GLenum id;
  GLfloat pos[4];
  GLfloat amb[4];
  GLfloat dif[4];
  GLfloat spc[4];
} rkglLight;

__EXPORT void rkglLightCreate(rkglLight *l, uint id, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb, GLfloat ns);
__EXPORT void rkglLightLoad(rkglLight *l);

__EXPORT void rkglLightSetPos(rkglLight *l, double x, double y, double z);
__EXPORT void rkglLightPut(rkglLight *l);

/*
void rkglLightFRead(FILE *fp);
*/

__END_DECLS

#endif /* __RKGL_OPTIC_H__ */
