/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_terra - visualization of an elevation map
 */

#include <roki-gl/rkgl_terra.h>

static void _rkglTerraCellPoint(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi_travs, zOpticalInfo *oi_untravs);
static void _rkglTerraCellNorm(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi);
static void _rkglTerraCellVar(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi);
static void _rkglTerraMeshGrid(zTerraCell *grid, double x, double y, double zmin, double zmax, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax);

void _rkglTerraCellPoint(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi_travs, zOpticalInfo *oi_untravs)
{
  zBox3D point;
  zVec3D center;

  zVec3DCreate( &center, x, y, terra->z );
  zBox3DCreateAlign( &point, &center, size, size, size );
  rkglMaterial( terra->travs ? oi_travs : oi_untravs );
  rkglBox( &point );
}

void rkglTerraPoint(zTerra *terra, double size, zOpticalInfo *oi_travs, zOpticalInfo *oi_untravs)
{
  register int i, j;

  for( i=0; i<terra->_nx; i++ )
    for( j=0; j<terra->_ny; j++ )
      _rkglTerraCellPoint( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), size, oi_travs, oi_untravs );
}

void _rkglTerraCellNorm(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi)
{
#define RKGL_TERRA_NORM_MUL 10
#define RKGL_TERRA_NORM_MAG  3
  zVec3D center, vec;

  zVec3DCreate( &center, x, y, terra->z );
  zVec3DMul( &terra->norm, RKGL_TERRA_NORM_MUL*size, &vec );
  rkglArrow( &center, &vec, RKGL_TERRA_NORM_MAG*size, oi );
}

void rkglTerraNorm(zTerra *terra, double size, zOpticalInfo *oi)
{
  register int i, j;

  for( i=0; i<terra->_nx; i++ )
    for( j=0; j<terra->_ny; j++ )
      _rkglTerraCellNorm( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), size, oi );
}

void _rkglTerraCellVar(zTerraCell *terra, double x, double y, double size, zOpticalInfo *oi)
{
#define RKGL_TERRA_VAR_MUL  0.5
#define RKGL_TERRA_VAR_MAG 10
  zBox3D point;
  zVec3D center;

  zVec3DCreate( &center, x, y, terra->z );
  zBox3DCreateAlign( &point, &center, RKGL_TERRA_VAR_MUL*size, RKGL_TERRA_VAR_MUL*size, RKGL_TERRA_VAR_MAG*terra->var );
  rkglMaterial( oi );
  rkglBox( &point );
}

void rkglTerraVar(zTerra *terra, double size, zOpticalInfo *oi)
{
  register int i, j;

  for( i=0; i<terra->_nx; i++ )
    for( j=0; j<terra->_ny; j++ )
      _rkglTerraCellVar( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), size, oi );
}

void _rkglTerraMeshGrid(zTerraCell *grid, double x, double y, double zmin, double zmax, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax)
{
  zOpticalInfo oi;

  zOpticalInfoBlend( oi_zmin, oi_zmax, (zmax-grid->z)/(zmax-zmin), &oi, NULL );
  rkglMaterial( &oi );
  glVertex3d( x, y, grid->z );
  rkglNormal( &grid->norm );
}

void rkglTerraMesh(zTerra *terra, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax)
{
  register int i, j;
  int nx, ny;
  double x0, y0, x1, y1, zmin, zmax;

  nx = terra->_nx - 1;
  ny = terra->_ny - 1;
  zTerraZRange( terra, &zmin, &zmax );
  for( i=0; i<nx; i++ )
    for( j=0; j<ny; j++ ){
      x0 = zTerraX(terra,i);
      y0 = zTerraY(terra,j);
      x1 = zTerraX(terra,i+1);
      y1 = zTerraY(terra,j+1);
      glBegin( GL_QUADS );
      _rkglTerraMeshGrid( zTerraGridNC(terra,i  ,j  ), x0, y0, zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( zTerraGridNC(terra,i+1,j  ), x1, y0, zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( zTerraGridNC(terra,i+1,j+1), x1, y1, zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( zTerraGridNC(terra,i  ,j+1), x0, y1, zmin, zmax, oi_zmin, oi_zmax );
      glEnd();
    }
}

void rkglTerraEntry(zTerra *terra, double size, zOpticalInfo oi[], int entry[])
{
  entry[0] = rkglBeginList();
  rkglTerraPoint( terra, size, &oi[RKGL_TERRA_OI_POINT_TRAVS], &oi[RKGL_TERRA_OI_POINT_UNTRAVS] );
  glEndList();
  entry[1] = rkglBeginList();
  rkglTerraNorm( terra, size, &oi[RKGL_TERRA_OI_NORM] );
  glEndList();
  entry[2] = rkglBeginList();
  rkglTerraVar( terra, size, &oi[RKGL_TERRA_OI_VAR] );
  glEndList();
  entry[3] = rkglBeginList();
  rkglTerraMesh( terra, &oi[RKGL_TERRA_OI_ZMIN], &oi[RKGL_TERRA_OI_ZMAX] );
  glEndList();
}

void rkglTerraCallEntry(int entry[], byte sw)
{
  if( sw & RKGL_TERRA_ENTRY_POINT ) glCallList( entry[0] );
  if( sw & RKGL_TERRA_ENTRY_NORM  ) glCallList( entry[1] );
  if( sw & RKGL_TERRA_ENTRY_VAR   ) glCallList( entry[2] );
  if( sw & RKGL_TERRA_ENTRY_MESH  ) glCallList( entry[3] );
}
