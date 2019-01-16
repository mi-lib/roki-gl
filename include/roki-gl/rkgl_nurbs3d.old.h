/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_nurbs3d - visualization of nurbs3d
 * contributer: 2018 Naoki Wakisaka
 */

#ifndef __RKGL_NURBS3D_H__
#define __RKGL_NURBS3D_H__

#include <roki-gl/rkgl_optic.h>
#include <zeo/zeo_nurbs3d.h>

__BEGIN_DECLS

typedef struct{
  GLUnurbsObj *globj;
  zOpticalInfo oi;
  zNURBS3D *ns;
  bool on_both_faces;
  /* buffer for opengl */
  GLfloat *knot[2];
  GLfloat *cp;
  /* for alloc */
  int knotn[2];
  int cpn;
} rkglNURBS3D;

#define rkglNURBS3DObj(glns)         ( (glns)->globj )
#define rkglNURBS3DOpticalInfo(glns) ( &(glns)->oi )
#define rkglNURBS3DNURBS(glns)       ( (glns)->ns )

void rkglNURBS3DSetObjDefault(rkglNURBS3D *glns);
void rkglNURBS3DSetObjSamplingTolerance(rkglNURBS3D *glns, float tol);
void rkglNURBS3DOnDrawBothFaces(rkglNURBS3D *glns, bool flag);
void rkglNURBS3DOnDrawFill(rkglNURBS3D *glns);
void rkglNURBS3DOnDrawPolygon(rkglNURBS3D *glns);

bool rkglNURBS3DCreate(rkglNURBS3D *glns);
void rkglNURBS3DDestroy(rkglNURBS3D *glns);

bool rkglNURBS3DSetNURBS(rkglNURBS3D *glns, zNURBS3D *ns);
bool rkglNURBS3DLoad(rkglNURBS3D *glns, zNURBS3D *ns);
void rkglNURBS3DUnLoad(rkglNURBS3D *glns);

void rkglNURBS3DCreateOpticalInfo(rkglNURBS3D *glns, float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, double ns, double sns, double alpha, char *name);
void rkglNURBS3DCreateOpticalInfoSimple(rkglNURBS3D *glns, float r, float g, float b, char *name);
void rkglNURBS3DSetOpticalInfo(rkglNURBS3D *glns, zOpticalInfo *oi);

void rkglNURBS3DDraw(rkglNURBS3D *glns);


/* for debug */
void rkglNURBS3DFWrite(FILE *fp, rkglNURBS3D *glns);

__END_DECLS

#endif /* __RKGL_NURBS3D_H__ */
