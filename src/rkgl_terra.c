/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_terra - visualization of an elevation map
 */

#include <roki-gl/rkgl_terra.h>

/* (static)
 * _rkglTerraVertex
 * - register a vertex on an elevation map.
 */
static void _rkglTerraVertex(zTerra *terra, int i, int j);
void _rkglTerraVertex(zTerra *terra, int i, int j)
{
  glVertex3f( zTerraX(terra,i), zTerraY(terra,j), zTerraGridNC(terra,i,j)->z );
}

/* rkglTerraPoint
 * - draw grid points of an elevation map.
 */
void rkglTerraPoint(zTerra *terra, zRGB *rgb_travs, zRGB *rgb_untravs)
{
  register int i, j;

  glPointSize( 0.1 * zMin( terra->dx, terra->dy ) );
  glDisable( GL_LIGHTING );
  glBegin( GL_POINTS );
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ ){
      rkglRGB( zTerraGridNC(terra,i,j)->travs ? rgb_travs : rgb_untravs );
      _rkglTerraVertex( terra, i, j );
    }
  glEnd();
  glEnable( GL_LIGHTING );
}

/* rkglTerraPointNet
 * - draw net of grid points of an elevation map.
 */
void rkglTerraPointNet(zTerra *terra, zRGB *rgb)
{
  register int i, j;

  glPointSize( 0.1 * zMin( terra->dx, terra->dy ) );
  glDisable( GL_LIGHTING );
  rkglRGB( rgb );
  glBegin( GL_POINTS );
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ )
      _rkglTerraVertex( terra, i, j );
  glEnd();
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ ){
      if( i > 0 ){
        glBegin(GL_LINES);
        _rkglTerraVertex( terra, i, j );
        _rkglTerraVertex( terra, i-1, j );
        glEnd();
      }
      if( j > 0 ){
        glBegin(GL_LINES);
        _rkglTerraVertex( terra, i, j );
        _rkglTerraVertex( terra, i, j-1 );
        glEnd();
      }
    }
  glEnable( GL_LIGHTING );
}

/* (static)
 * _rkglTerraCellNorm
 * - draw the normal vector associated with a specified grid on an elevation map.
 */
static void _rkglTerraCellNorm(zTerraCell *cell, double x, double y, double scale, zOpticalInfo *oi);
void _rkglTerraCellNorm(zTerraCell *cell, double x, double y, double scale, zOpticalInfo *oi)
{
  zVec3D center, vec;

  zVec3DCreate( &center, x, y, cell->z );
  zVec3DMul( &cell->norm, scale, &vec );
  rkglMaterial( oi );
  rkglArrow( &center, &vec, scale );
}

/* rkglTerraNorm
 * - draw normal vectors associated with grids of an elevation map.
 */
void rkglTerraNorm(zTerra *terra, zOpticalInfo *oi)
{
  register int i, j;
  double scale;

  scale = sqrt( zSqr(terra->dx) + zSqr(terra->dy) );
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ )
      _rkglTerraCellNorm( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), scale, oi );
}

/* (static)
 * _rkglTerraCellVar
 * - draw variance of estimated height at a specified grid of an elevation map.
 */
static void _rkglTerraCellVar(zTerraCell *cell, double x, double y, double dx, double dy, zOpticalInfo *oi);
void _rkglTerraCellVar(zTerraCell *cell, double x, double y, double dx, double dy, zOpticalInfo *oi)
{
  zBox3D point;
  zVec3D center;

  zVec3DCreate( &center, x, y, cell->z );
  zBox3DCreateAlign( &point, &center, dx, dy, cell->var );
  rkglMaterial( oi );
  rkglBox( &point, RKGL_FACE );
}

/* rkglTerraVar
 * - draw variance of estimate height at grids of an elevation map.
 */
void rkglTerraVar(zTerra *terra, zOpticalInfo *oi)
{
  register int i, j;

  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ )
      _rkglTerraCellVar( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), terra->dx, terra->dy, oi );
}

/* (static)
 * _rkglTerraMeshGrid
 * - register a grid of an elevation map to draw mesh.
 */
static void _rkglTerraMeshGrid(zTerra *terra, int i, int j, double zmin, double zmax, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax);
void _rkglTerraMeshGrid(zTerra *terra, int i, int j, double zmin, double zmax, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax)
{
  zOpticalInfo oi;
  zTerraCell *grid;

  grid = zTerraGridNC(terra,i,j);
  zOpticalInfoBlend( oi_zmin, oi_zmax, (zmax-grid->z)/(zmax-zmin), &oi, NULL );
  rkglMaterial( &oi );
  glVertex3f( zTerraX(terra,i), zTerraY(terra,j), grid->z );
  rkglNormal( &grid->norm );
}

/* rkglTerraMesh
 * - draw mesh of an elevation map.
 */
void rkglTerraMesh(zTerra *terra, zOpticalInfo *oi_zmin, zOpticalInfo *oi_zmax)
{
  register int i, j;
  double zmin, zmax;

  zTerraZRange( terra, &zmin, &zmax );
  glShadeModel( GL_SMOOTH );
  for( i=1; i<zTerraXSize(terra); i++ )
    for( j=1; j<zTerraYSize(terra); j++ ){
      glBegin( GL_QUADS );
      _rkglTerraMeshGrid( terra, i-1, j-1, zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( terra, i,   j-1, zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( terra, i,   j,   zmin, zmax, oi_zmin, oi_zmax );
      _rkglTerraMeshGrid( terra, i-1, j,   zmin, zmax, oi_zmin, oi_zmax );
      glEnd();
    }
}

/* rkglTerraMeshSmooth
 * - draw mesh of an elevation map smoothed by Bezier interpolation.
 */
void rkglTerraMeshSmooth(zTerra *terra, int xslice, int yslice, zOpticalInfo *oi)
{
  register int i, j;
  zVec3D *grid, *gp;

  if( !( grid = zAlloc( zVec3D, zTerraXSize(terra) * zTerraYSize(terra) ) ) ){
    ZALLOCERROR();
    return;
  }
  gp = grid;
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ )
      zVec3DCreate( gp++, zTerraX(terra,i), zTerraY(terra,j), zTerraGridNC(terra,i,j)->z );
  rkglMaterial( oi );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  glMap2d( GL_MAP2_VERTEX_3,
    0, 1, zTerraYSize(terra)*3, zTerraXSize(terra),
    0, 1,                    3, zTerraYSize(terra),
    (GLdouble *)grid );
  glMapGrid2d( xslice, 0, 1, yslice, 0, 1 );
  glEnable( GL_MAP2_VERTEX_3 );
  glEvalMesh2( GL_FILL, 0, xslice, 0, yslice );
  zFree( grid );
}

/* (static)
 * _rkglTerraCellPatch
 * - draw a patch at a specified grid of an elevation map.
 */
static void _rkglTerraCellPatch(zTerraCell *cell, double x, double y, double dx, double dy);
void _rkglTerraCellPatch(zTerraCell *cell, double x, double y, double dx, double dy)
{
  double _x, _y;

  glBegin( GL_QUADS );
  _x = x + dx, _y = y + dy; glVertex3d( _x, _y, zTerraCellZ(cell,_x,_y) );
  _x = x - dx, _y = y + dy; glVertex3d( _x, _y, zTerraCellZ(cell,_x,_y) );
  _x = x - dx, _y = y - dy; glVertex3d( _x, _y, zTerraCellZ(cell,_x,_y) );
  _x = x + dx, _y = y - dy; glVertex3d( _x, _y, zTerraCellZ(cell,_x,_y) );
  rkglNormal( &cell->norm );
  glEnd();
}

/* rkglTerraPatch
 * - draw patches of an elevation map.
 */
void rkglTerraPatch(zTerra *terra, zOpticalInfo *oi)
{
  register int i, j;
  double dx, dy;

  dx = 0.5 * terra->dx;
  dy = 0.5 * terra->dy;
  glShadeModel( GL_FLAT );
  rkglMaterial( oi );
  for( i=0; i<zTerraXSize(terra); i++ )
    for( j=0; j<zTerraYSize(terra); j++ )
      _rkglTerraCellPatch( zTerraGridNC(terra,i,j), zTerraX(terra,i), zTerraY(terra,j), dx, dy );
}
