/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_framehandle - handle to manipulate objects
 */

#ifndef __RKGL_FRAMEHANDLE_H__
#define __RKGL_FRAMEHANDLE_H__

#include <roki_gl/rkgl_select.h>

/*! \struct rkglFrameHandle
 * \brief handle to manipulate position and orientation of a 3D object.
 */
ZDEF_STRUCT( __ROKI_GL_CLASS_EXPORT, rkglFrameHandle ){
  int part[6];
  zFrame3D frame;
  uint name;
  int selected_id;
  zVec3D _anchor;
  double _depth;
#ifdef __cplusplus
 public:
  rkglFrameHandle();
  rkglFrameHandle(uint _name, double _l, double _mag);
  ~rkglFrameHandle();
  void unselect();
  bool isUnselected();
  void create(uint _name, double _l, double _mag);
  void destroy();
  void draw();
  bool isInTranslation();
  bool isInRotation();
  int anchor(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y);
  bool move(rkglCamera *cam, int x, int y);
#endif /* __cplusplus */
};

__BEGIN_DECLS

#define rkglFrameHandlePos(h) zFrame3DPos( &(h)->frame )
#define rkglFrameHandleAtt(h) zFrame3DAtt( &(h)->frame )

#define rkglFrameHandleUnselect(h)     ( (h)->selected_id = -1 )
#define rkglFrameHandleIsUnselected(h) ( (h)->selected_id == -1 )

__ROKI_GL_EXPORT void rkglFrameHandleCreate(rkglFrameHandle *handle, uint name, double l, double mag);
__ROKI_GL_EXPORT void rkglFrameHandleDestroy(rkglFrameHandle *handle);

__ROKI_GL_EXPORT void rkglFrameHandleDraw(rkglFrameHandle *handle);

__ROKI_GL_EXPORT bool rkglFrameHandleIsInTranslation(rkglFrameHandle *handle);
__ROKI_GL_EXPORT bool rkglFrameHandleIsInRotation(rkglFrameHandle *handle);

__ROKI_GL_EXPORT int rkglFrameHandleAnchor(rkglFrameHandle *handle, rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y);
__ROKI_GL_EXPORT bool rkglFrameHandleMove(rkglFrameHandle *handle, rkglCamera *cam, int x, int y);

__END_DECLS

#ifdef __cplusplus
inline rkglFrameHandle::rkglFrameHandle(){ rkglFrameHandle( 0, 0, 0 ); }
inline rkglFrameHandle::rkglFrameHandle(uint _name, double _l, double _mag){ this->create( _name, _l, _mag ); }
inline rkglFrameHandle::~rkglFrameHandle(){}
inline void rkglFrameHandle::unselect(){ rkglFrameHandleUnselect( this ); }
inline bool rkglFrameHandle::isUnselected(){ return rkglFrameHandleIsUnselected( this ); }
inline void rkglFrameHandle::create(uint _name, double _l, double _mag){ rkglFrameHandleCreate( this, _name, _l, _mag ); }
inline void rkglFrameHandle::destroy(){ rkglFrameHandleDestroy( this ); }
inline void rkglFrameHandle::draw(){ rkglFrameHandleDraw( this ); }
inline bool rkglFrameHandle::isInTranslation(){ return rkglFrameHandleIsInTranslation( this ); }
inline bool rkglFrameHandle::isInRotation(){ return rkglFrameHandleIsInRotation( this ); }
inline int rkglFrameHandle::anchor(rkglSelectionBuffer *sb, rkglCamera *cam, int x, int y){ return rkglFrameHandleAnchor( this, sb, cam, x, y ); }
inline bool rkglFrameHandle::move(rkglCamera *cam, int x, int y){ return rkglFrameHandleMove( this, cam, x, y ); }
#endif /* __cplusplus */

#endif /* __RKGL_FRAMEHANDLE_H__ */
