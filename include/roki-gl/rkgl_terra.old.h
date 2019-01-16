/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_terra - visualization of an elevation map.
 */

#ifndef __RKGL_TERRA_H__
#define __RKGL_TERRA_H__

#include <roki-gl/rkgl_shape.h>
#include <zeo/zeo_terra.h>

__BEGIN_DECLS

#define RKGL_TERRA_OI_POINT_TRAVS   0
#define RKGL_TERRA_OI_POINT_UNTRAVS 1
#define RKGL_TERRA_OI_NORM          2
#define RKGL_TERRA_OI_VAR           3
#define RKGL_TERRA_OI_ZMIN          4
#define RKGL_TERRA_OI_ZMAX          5

/*! \brief draw grid points of an elevation map. */
__EXPORT void rkglTerraPoint(zTerra *terra, double size, zOpticalInfo *oi_travs, zOpticalInfo *oi_untravs);

/*! \brief draw normal vectors associated with grids of an elevation map. */
__EXPORT void rkglTerraNorm(zTerra *terra, double size, zOpticalInfo *oi);

/*! \brief draw variance of estimate height at grids of an elevation map. */
__EXPORT void rkglTerraVar(zTerra *terra, double size, zOpticalInfo *oi);

/*! \brief draw mesh of an elevation map. */
__EXPORT void rkglTerraMesh(zTerra *terra, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax);

#define RKGL_TERRA_ENTRY_POINT 0x1
#define RKGL_TERRA_ENTRY_NORM  0x2
#define RKGL_TERRA_ENTRY_VAR   0x4
#define RKGL_TERRA_ENTRY_MESH  0x8

/*! \brief entry an elevation map to a draw list. */
__EXPORT void rkglTerraEntry(zTerra *terra, double size, zOpticalInfo oi[], int entry[]);

/*! \brief call entries of an elevation map in a draw list. */
__EXPORT void rkglTerraCallEntry(int entry[], byte sw);

__END_DECLS

#endif /* __RKGL_TERRA_H__ */
