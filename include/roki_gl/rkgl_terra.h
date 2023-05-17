/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_terra - visualization of an elevation map.
 */

#ifndef __RKGL_TERRA_H__
#define __RKGL_TERRA_H__

#include <roki_gl/rkgl_shape.h>

__BEGIN_DECLS

/*! \brief draw grid points of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraPoint(zTerra *terra, zRGB *rgb_travs, zRGB *rgb_untravs);

/*! \brief draw net of grid points of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraPointNet(zTerra *terra, zRGB *rgb);

/*! \brief draw normal vectors associated with grids of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraNorm(zTerra *terra, zOpticalInfo *oi);

/*! \brief draw variance of estimate height at grids of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraVar(zTerra *terra, zOpticalInfo *oi);

/*! \brief draw mesh of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraMesh(zTerra *terra, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax);

/*! \brief draw mesh of an elevation map smoothed by Bezier interpolation. */
__ROKI_GL_EXPORT void rkglTerraMeshSmooth(zTerra *terra, int xslice, int yslice, zOpticalInfo *oi);

/*! \brief draw patches of an elevation map. */
__ROKI_GL_EXPORT void rkglTerraPatch(zTerra *terra, zOpticalInfo *oi);

__END_DECLS

#endif /* __RKGL_TERRA_H__ */
