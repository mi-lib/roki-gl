/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_select - object selection
 */

#ifndef __RKGL_SELECT_H__
#define __RKGL_SELECT_H__

#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>

__BEGIN_DECLS

/* project / unproject */
__ROKI_GL_EXPORT int rkglProject(rkglCamera *c, zVec3D *p, int *x, int *y);
__ROKI_GL_EXPORT int rkglUnproject(rkglCamera *c, int x, int y, double depth, zVec3D *p);
__ROKI_GL_EXPORT int rkglPickPoint(rkglCamera *c, int x, int y, zVec3D *p);
__ROKI_GL_EXPORT double rkglGetDepth(rkglCamera *c, int x, int y);

/* select */

#define RKGL_SELECTION_BUF_SIZE BUFSIZ

ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglSelectionBuffer ){
  GLuint buf[RKGL_SELECTION_BUF_SIZE];
  GLuint *cur;
  int hits;
};

#define rkglSelectionNameSize(sb) (sb)->cur[0]
#define rkglSelectionZnear(sb)    (sb)->cur[1]
#define rkglSelectionZfar(sb)     (sb)->cur[2]
#define rkglSelectionZnearDepth(sb) ( (double)rkglSelectionZnear(sb) / (double)0xffffffff )
#define rkglSelectionZfarDepth(sb)  ( (double)rkglSelectionZfar(sb)  / (double)0xffffffff )
#define rkglSelectionName(sb,i)   (sb)->cur[3+(i)]

#define rkglSelectionRewind(sb)   ( (sb)->cur = (sb)->buf )
#define rkglSelectionNext(sb)     ( (sb)->cur += 3 + rkglSelectionNameSize(sb) )

__ROKI_GL_EXPORT int rkglSelect(rkglSelectionBuffer *sb, rkglCamera *cam, void (* scene)(void), int x, int y, int w, int h);
__ROKI_GL_EXPORT GLuint *rkglSelectNearest(rkglSelectionBuffer *sb);

/* for debug */
__ROKI_GL_EXPORT void rkglSelectionPrintName(rkglSelectionBuffer *sb);
__ROKI_GL_EXPORT void rkglSelectionPrint(rkglSelectionBuffer *sb);

__END_DECLS

#endif /* __RKGL_SELECT_H__ */
