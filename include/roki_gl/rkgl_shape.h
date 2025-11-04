/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shape - visualization of shapes.
 */

#ifndef __RKGL_SHAPE_H__
#define __RKGL_SHAPE_H__

#include <roki_gl/rkgl_optic.h>
#include <roki_gl/rkgl_texture.h>
#include <zeo/zeo.h>

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

#define rkglVertex(v)    glVertex3dv( (v)->e )
#define rkglNormal(n)    glNormal3dv( (n)->e )
#define rkglNormalRev(n) glNormal3d( -(n)->c.x, -(n)->c.y, -(n)->c.z )

/*! \brief put a 3D point. */
__ROKI_GL_EXPORT void rkglPoint(zVec3D *p);
/*! \brief put a 3D edge. */
__ROKI_GL_EXPORT void rkglEdge(zEdge3D *e);

/*! \brief put a 3D triangle face. */
__ROKI_GL_EXPORT void rkglTriFace(zTri3D *t);
/*! \brief put a 3D triangle wireframe. */
__ROKI_GL_EXPORT void rkglTriWireframe(zTri3D *t);
/*! \brief put a 3D triangle with texture. */
__ROKI_GL_EXPORT void rkglTriTexture(zTri3D *t, zTri2D *f);
/*! \brief put a 3D triangle with bump map. */
__ROKI_GL_EXPORT void rkglTriBump(zTri3D *t, zTri2D *f, zVec3D *lp);

/* primitive shapes */

/*! \brief draw an axis-aligned 3D box. */
__ROKI_GL_EXPORT void rkglAABox(zAABox3D *box, ubyte disptype);
/*! \brief draw a 3D box. */
__ROKI_GL_EXPORT void rkglBox(zBox3D *box, ubyte disptype);
/*! \brief draw a 3D hemisphere. */
__ROKI_GL_EXPORT void rkglHemisphere(zSphere3D *sphere, zVec3D *dir, ubyte disptype);
/*! \brief draw a 3D sphere. */
__ROKI_GL_EXPORT void rkglSphere(zSphere3D *sphere, ubyte disptype);
/*! \brief draw a 3D ellipsoid. */
__ROKI_GL_EXPORT void rkglEllips(zEllips3D *ellips, ubyte disptype);
/*! \brief draw a 3D tube. */
__ROKI_GL_EXPORT void rkglTube(zCyl3D *tube, ubyte disptype);
/*! \brief draw a 3D cylinder. */
__ROKI_GL_EXPORT void rkglCyl(zCyl3D *cyl, ubyte disptype);
/*! \brief draw a 3D capsule. */
__ROKI_GL_EXPORT void rkglCapsule(zCapsule3D *capsule, ubyte disptype);
/*! \brief draw a 3D elliptic cylinder. */
__ROKI_GL_EXPORT void rkglECyl(zECyl3D *ecyl, ubyte disptype);
/*! \brief draw a 3D cone. */
__ROKI_GL_EXPORT void rkglCone(zCone3D *cone, ubyte disptype);
/*! \brief draw a 3D torus. */
__ROKI_GL_EXPORT void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, ubyte disptype);

/* NURBS surface and curve */

/*! \brief draw a 3D NURBS surface. */
__ROKI_GL_EXPORT void rkglNURBS(zNURBS3D *nurbs, ubyte disptype);
/*! \brief draw a 3D NURBS surface with conrol points. */
__ROKI_GL_EXPORT void rkglNURBSCP(zNURBS3D *nurbs);

/*! \brief draw a 3D NURBS curve. */
__ROKI_GL_EXPORT void rkglNURBSCurve(zNURBS3D *nurbs);
/*! \brief draw a 3D NURBS curve with conrol points. */
__ROKI_GL_EXPORT void rkglNURBSCurveCP(zNURBS3D *nurbs);

/* polyhedron */

/*! \brief draw a polyhedron. */
__ROKI_GL_EXPORT void rkglPH(zPH3D *ph, ubyte disptype);
/*! \brief draw a polyhedron with texture. */
__ROKI_GL_EXPORT void rkglPHTexture(zPH3D *ph, zOpticalInfo *oi, zTexture *texture);
/*! \brief draw a polyhedron with bump map. */
__ROKI_GL_EXPORT void rkglPHBump(zPH3D *ph, zTexture *bump, rkglLight *light);

/* shape and multishape */

/*! \brief draw a 3D shape. */
__ROKI_GL_EXPORT void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light);
/*! \brief entry a 3D shape to the display list. */
__ROKI_GL_EXPORT int rkglEntryShape(zShape3D *s, zOpticalInfo *oi_alt, ubyte disptype, rkglLight *light);

/*! \brief draw multiple 3D shapes. */
__ROKI_GL_EXPORT void rkglMShape(zMShape3D *s, ubyte disptype, rkglLight *light);
/*! \brief entry multiple 3D shapes to the display list. */
__ROKI_GL_EXPORT int rkglEntryMShape(zMShape3D *s, ubyte disptype, rkglLight *light);

/* point cloud */

/*! \brief draw 3D pointcloud. */
__ROKI_GL_EXPORT void rkglPointCloud(zVec3DData *data);
/*! \brief draw 3D pointcloud with estimated normal vectors. */
__ROKI_GL_EXPORT void rkglPointCloudNormal(zVec3DData *pointdata, zVec3DData *normaldata, double length);

/*! \brief draw a 3D ellipsoid represented by a barycenter and a variance-covariane matrix. */
__ROKI_GL_EXPORT void rkglEllipsBaryCov(const zVec3D *center, const zMat3D *cov);

/* octree */

/*! \brief draw an octree. */
__ROKI_GL_EXPORT void rkglOctree(zVec3DOctree *octree);
/*! \brief draw points in an octree. */
__ROKI_GL_EXPORT void rkglOctreePoints(zVec3DOctree *octree);
/*! \brief draw normal vectors of an octree. */
__ROKI_GL_EXPORT void rkglOctreeNormal(zVec3DOctree *octree, double length);

/* fancy geometries */

#define RKGL_ARROW_DIV        8
#define RKGL_ARROW_BOTTOM_RAD 0.05
#define RKGL_ARROW_NECK_RAD   0.1
#define RKGL_ARROW_TIP_LEN    0.2
/*! \brief draw a 3D arrow. */
__ROKI_GL_EXPORT void rkglArrow(zVec3D *bot, zVec3D *vec, double mag);

#define RKGL_FRAME_HEAD_LENGTH_RATIO  0.1
#define RKGL_FRAME_HEAD_APATURE_RATIO 0.05
/*! \brief draw a 3D coordinate frame. */
__ROKI_GL_EXPORT void rkglFrame(zFrame3D *f, double length);

/*! \brief draw a 3D coordinate axis. */
__ROKI_GL_EXPORT void rkglAxis(zAxis axis, double length);
/*! \brief draw 3D wireframe gauges. */
__ROKI_GL_EXPORT void rkglGauge(zAxis axis1, double span1, zAxis axis2, double span2, double step);
/*! \brief draw a 3D checker board. */
__ROKI_GL_EXPORT void rkglCheckerBoard(zVec3D *pc0, zVec3D *pc1, zVec3D *pc2, int div1, int div2, zOpticalInfo *oi1, zOpticalInfo *oi2);

__END_DECLS

#endif /* __RKGL_SHAPE_H__ */
