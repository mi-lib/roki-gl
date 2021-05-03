/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shape - visualization of shapes
 */

#ifndef __RKGL_SHAPE_H__
#define __RKGL_SHAPE_H__

#include <roki-gl/rkgl_optic.h>
#include <roki-gl/rkgl_texture.h>
#include <zeo/zeo_pointcloud.h>
#include <zeo/zeo_mshape3d.h>

__BEGIN_DECLS

enum{
  RKGL_FACE = 0,
  RKGL_WIREFRAME,
  RKGL_STICK,
  RKGL_COM,
  RKGL_ELLIPS,
  RKGL_BB,
};

/* display list */

int rkglBeginList(void);
void rkglDeleteList(int id);

/* 3D object drawing */

void rkglTranslate(zVec3D *v);
void rkglXform(zFrame3D *f);

#define rkglVertex(v)  glVertex3dv( (v)->e )
#define rkglNormal(n)  glNormal3dv( (n)->e )

void rkglPoint(zVec3D *p);
void rkglEdge(zEdge3D *e);
void rkglTri(zTri3D *t);
void rkglTriTexture(zTri3D *t, zTri2D *f);
void rkglTriBump(zTri3D *t, zTri2D *f, zVec3D *lp);
void rkglPolygon(zVec3D v[], int n, ...);

void rkglBox(zBox3D *box, int disptype);
void rkglSphere(zSphere3D *sphere, int disptype);
void rkglEllips(zEllips3D *ellips, int disptype);
void rkglCyl(zCyl3D *cyl, int disptype);
void rkglECyl(zECyl3D *ecyl, int disptype);
void rkglCone(zCone3D *cone, int disptype);

void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, int disptype);

void rkglNURBS(zNURBS3D *nurbs, int disptype);
void rkglNURBSCP(zNURBS3D *nurbs, GLfloat size, zRGB *rgb);

void rkglPH(zPH3D *ph, int disptype);
void rkglPHTexture(zPH3D *ph, zOpticalInfo *oi, zTexture *texture);
void rkglPHBump(zPH3D *ph, zOpticalInfo *oi, zTexture *bump, rkglLight *light);

void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, int disptype, rkglLight *light);
int rkglShapeEntry(zShape3D *s, zOpticalInfo *oi_alt, int disptype, rkglLight *light);

void rkglMShape(zMShape3D *s, int disptype, rkglLight *light);
int rkglMShapeEntry(zMShape3D *s, int disptype, rkglLight *light);

void rkglPointCloud(zVec3DList *pc, zVec3D *center, short size);

#define RKGL_ARROW_DIV        8
#define RKGL_ARROW_BOTTOM_RAD 0.05
#define RKGL_ARROW_NECK_RAD   0.1
#define RKGL_ARROW_TIP_LEN    0.2
void rkglArrow(zVec3D *bot, zVec3D *vec, double mag);

void rkglAxis(zDir axis, double d, double w, GLfloat color[]);
void rkglGauge(zDir axis1, double d1, zDir axis2, double d2, double w, double step, GLfloat color[]);
void rkglChecker(zVec3D *pc0, zVec3D *pc1, zVec3D *pc2, int div1, int div2, zOpticalInfo *oi1, zOpticalInfo *oi2);

__END_DECLS

#endif /* __RKGL_SHAPE_H__ */
