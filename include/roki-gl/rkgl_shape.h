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

/* display types */

#define RKGL_FACE      0x01
#define RKGL_WIREFRAME 0x02
#define RKGL_STICK     0x04
#define RKGL_FRAME     0x08
#define RKGL_COM       0x10
#define RKGL_ELLIPS    0x20
#define RKGL_BB        0x40

/* 3D object drawing */

__EXPORT void rkglTranslate(zVec3D *v);
__EXPORT void rkglXform(zFrame3D *f);

#define rkglVertex(v)  glVertex3dv( (v)->e )
#define rkglNormal(n)  glNormal3dv( (n)->e )

__EXPORT void rkglPoint(zVec3D *p);
__EXPORT void rkglEdge(zEdge3D *e);
__EXPORT void rkglTri(zTri3D *t);
__EXPORT void rkglTriTexture(zTri3D *t, zTri2D *f);
__EXPORT void rkglTriBump(zTri3D *t, zTri2D *f, zVec3D *lp);
__EXPORT void rkglPolygon(zVec3D v[], int n, ...);

__EXPORT void rkglBox(zBox3D *box, ubyte disptype);
__EXPORT void rkglSphere(zSphere3D *sphere, ubyte disptype);
__EXPORT void rkglEllips(zEllips3D *ellips, ubyte disptype);
__EXPORT void rkglCyl(zCyl3D *cyl, ubyte disptype);
__EXPORT void rkglECyl(zECyl3D *ecyl, ubyte disptype);
__EXPORT void rkglCone(zCone3D *cone, ubyte disptype);

__EXPORT void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, ubyte disptype);

__EXPORT void rkglNURBS(zNURBS3D *nurbs, ubyte disptype);
__EXPORT void rkglNURBSCP(zNURBS3D *nurbs, GLfloat size, zRGB *rgb);

__EXPORT void rkglPH(zPH3D *ph, ubyte disptype);
__EXPORT void rkglPHTexture(zPH3D *ph, zOpticalInfo *oi, zTexture *texture);
__EXPORT void rkglPHBump(zPH3D *ph, zOpticalInfo *oi, zTexture *bump, rkglLight *light);

__EXPORT void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light);
__EXPORT int rkglShapeEntry(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light);

__EXPORT void rkglMShape(zMShape3D *s, ubyte disptype, rkglLight *light);
__EXPORT int rkglMShapeEntry(zMShape3D *s, ubyte disptype, rkglLight *light);

__EXPORT void rkglPointCloud(zVec3DList *pc, zVec3D *center, short size);

#define RKGL_ARROW_DIV        8
#define RKGL_ARROW_BOTTOM_RAD 0.05
#define RKGL_ARROW_NECK_RAD   0.1
#define RKGL_ARROW_TIP_LEN    0.2
__EXPORT void rkglArrow(zVec3D *bot, zVec3D *vec, double mag);

__EXPORT void rkglFrame(zFrame3D *f, double l, double mag);
__EXPORT void rkglFrameHandle(zFrame3D *f, double l, double mag);

__EXPORT void rkglAxis(zAxis axis, double d, double w, GLfloat color[]);
__EXPORT void rkglGauge(zAxis axis1, double d1, zAxis axis2, double d2, double w, double step, GLfloat color[]);
__EXPORT void rkglCheckerBoard(zVec3D *pc0, zVec3D *pc1, zVec3D *pc2, int div1, int div2, zOpticalInfo *oi1, zOpticalInfo *oi2);

__END_DECLS

#endif /* __RKGL_SHAPE_H__ */
