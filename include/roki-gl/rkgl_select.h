/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_select - object selection
 */

#ifndef __RKGL_SELECT_H__
#define __RKGL_SELECT_H__

#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

__BEGIN_DECLS

/* project / unproject */
__EXPORT int rkglProject(rkglCamera *c, zVec3D *p, int *x, int *y);
__EXPORT int rkglUnproject(rkglCamera *c, int x, int y, double depth, zVec3D *p);
__EXPORT int rkglPickAndUnproject(rkglCamera *c, int x, int y, zVec3D *p);
__EXPORT double rkglGetDepth(rkglCamera *c, int x, int y);

/* pick */
__EXPORT int rkglPick(rkglCamera *c, void (* scene)(void), GLuint selbuf[], size_t size, int x, int y, int w, int h);
__EXPORT GLuint *rkglFindNearside(GLuint selbuf[], int hits);

__END_DECLS

#endif /* __RKGL_SELECT_H__ */
