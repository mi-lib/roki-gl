/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_superimpose - special effects to superimpose images.
 */

#ifndef __RKGL_SUPERIMPOSE_H__
#define __RKGL_SUPERIMPOSE_H__

#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_optic.h>

__BEGIN_DECLS

/*! \brief abstract contour of objects in the current camera view. */
__ROKI_GL_EXPORT void rkglContour(rkglCamera *camera);

__END_DECLS

#endif /* __RKGL_SUPERIMPOSE_H__ */
