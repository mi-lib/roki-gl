/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_framehandle - handle to manipulate objects
 */

#ifndef __RKGL_FRAMEHANDLE_H__
#define __RKGL_FRAMEHANDLE_H__

#include <roki_gl/rkgl_select.h>

__BEGIN_DECLS

typedef struct{
  int part[6];
  zFrame3D frame;
  int name;
  int selected_id;

  zVec3D _anchor;
  double _depth;
} rkglFrameHandle;

#define rkglFrameHandlePos(h) zFrame3DPos( &(h)->frame )
#define rkglFrameHandleAtt(h) zFrame3DAtt( &(h)->frame )

#define rkglFrameHandleUnselect(h)     ( (h)->selected_id = -1 )
#define rkglFrameHandleIsUnselected(h) ( (h)->selected_id == -1 )

__ROKI_GL_EXPORT void rkglFrameHandleCreate(rkglFrameHandle *handle, int name, double l, double mag);
__ROKI_GL_EXPORT void rkglFrameHandleDestroy(rkglFrameHandle *handle);

__ROKI_GL_EXPORT void rkglFrameHandleDraw(rkglFrameHandle *handle);

__ROKI_GL_EXPORT void rkglFrameHandleSelect(rkglFrameHandle *handle, rkglCamera *cam, int x, int y, void (* draw_func)(void));
__ROKI_GL_EXPORT bool rkglFrameHandleMove(rkglFrameHandle *handle, rkglCamera *cam, int x, int y);

__END_DECLS

#endif /* __RKGL_FRAMEHANDLE_H__ */
