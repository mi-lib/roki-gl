/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_optic - optics
 */

#ifndef __RKGL_OPTIC_H__
#define __RKGL_OPTIC_H__

#include <zeo/zeo_render_optic.h>
#include <roki_gl/rkgl_misc.h>

__BEGIN_DECLS

/* color material */

#define rkglRGB(rgb)    glColor3f( (rgb)->r, (rgb)->g, (rgb)->b )
#define rkglRGBA(rgb,a) glColor4f( (rgb)->r, (rgb)->g, (rgb)->b, (a) )

__ROKI_GL_EXPORT void rkglRGBIntensify(GLfloat dr, GLfloat dg, GLfloat db);

__ROKI_GL_EXPORT void rkglRGBByStr(const char *str);

__ROKI_GL_EXPORT void rkglColor24(unsigned int color);

__ROKI_GL_EXPORT void rkglMaterialRGBA(zRGB *rgb, float alpha);
__ROKI_GL_EXPORT void rkglMaterialWhite(void);
__ROKI_GL_EXPORT void rkglMaterialOpticalInfo(zOpticalInfo *oi);
__ROKI_GL_EXPORT void rkglMaterial(zOpticalInfo *oi);

/* lighting */

typedef struct{
  GLenum id;
  GLfloat pos[4];
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat dir[4];
  GLfloat cutoffangle;
  GLfloat exponent;
  GLfloat attenuation[3];
} rkglLight;

__ROKI_GL_EXPORT int rkglLightNum(void);

#define rkglLightSetPos(l,x,y,z) do{\
  (l)->pos[0] = (x); (l)->pos[1] = (y); (l)->pos[2] = (z); (l)->pos[3] = 1;\
} while(0)
#define rkglLightSetAmbient(l,r,g,b) do{\
  (l)->ambient[0] = (r); (l)->ambient[1] = (g); (l)->ambient[2] = (b); (l)->ambient[3] = 1;\
} while(0)
#define rkglLightSetDiffuse(l,r,g,b) do{\
  (l)->diffuse[0] = (r); (l)->diffuse[1] = (g); (l)->diffuse[2] = (b); (l)->diffuse[3] = 1;\
} while(0)
#define rkglLightSetSpecular(l,r,g,b) do{\
  (l)->specular[0] = (r); (l)->specular[1] = (g); (l)->specular[2] = (b); (l)->specular[3] = 1;\
} while(0)
#define rkglLightSetDir(l,x,y,z) do{\
  (l)->dir[0] = (x); (l)->dir[1] = (y); (l)->dir[2] = (z); (l)->dir[3] = 1;\
} while(0)

__ROKI_GL_EXPORT bool rkglLightCreate(rkglLight *light, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb);
__ROKI_GL_EXPORT bool rkglLightCreateExtra(rkglLight *light, uint n, GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat sr, GLfloat sg, GLfloat sb);

__ROKI_GL_EXPORT void rkglLightLoad(rkglLight *light);

__ROKI_GL_EXPORT void rkglLightSetAttenuation(rkglLight *light, GLfloat att_const, GLfloat att_lin, GLfloat att_quad);
#define rkglLightSetAttenuationConst(l,a)  rkglLightSetAttenuation( (l), (a), 0.0, 0.0 )
#define rkglLightSetAttenuationLinear(l,a) rkglLightSetAttenuation( (l), 0.0, (a)/(fabs((l)->pos[0])+fabs((l)->pos[1])+fabs((l)->pos[2])), 0.0 )
#define rkglLightSetAttenuationQuad(l,a)   rkglLightSetAttenuation( (l), 0.0, 0.0, (a)/(zSqr((l)->pos[0])+zSqr((l)->pos[1])+zSqr((l)->pos[2])) )

__ROKI_GL_EXPORT void rkglLightSetSpot(rkglLight *light, double lx, double ly, double lz, double cutoffangle, double exponent);

__ROKI_GL_EXPORT void rkglLightPut(rkglLight *light);

__ROKI_GL_EXPORT void rkglLightMove(rkglLight *light, GLfloat x, GLfloat y, GLfloat z);

/*
void rkglLightFRead(FILE *fp);
*/

#define rkglSaveLighting( __flag ) do{\
  if( ( *(__flag) = glIsEnabled( GL_LIGHTING ) ) ) glDisable( GL_LIGHTING );\
} while(0)
#define rkglLoadLighting( __flag ) do{\
  if( __flag ) glEnable( GL_LIGHTING );\
} while(0)

/* fog effect */

__ROKI_GL_EXPORT void rkglFogExp(GLfloat r, GLfloat g, GLfloat b, GLfloat density);
__ROKI_GL_EXPORT void rkglFogExp2(GLfloat r, GLfloat g, GLfloat b, GLfloat density);
__ROKI_GL_EXPORT void rkglFogLinear(GLfloat r, GLfloat g, GLfloat b, GLfloat density, GLfloat start, GLfloat end);

__END_DECLS

#endif /* __RKGL_OPTIC_H__ */
