/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_nurbs3d - visualization of nurbs3d
 * contributer: 2018 Naoki Wakisaka
 */

#include <roki-gl/rkgl_nurbs3d.h>

void rkglNURBS3DSetObjDefault(rkglNURBS3D *glns)
{
  gluNurbsProperty( glns->globj, GLU_SAMPLING_METHOD, GLU_PATH_LENGTH );
  gluNurbsProperty( glns->globj, GLU_SAMPLING_TOLERANCE, 10.0 );
  gluNurbsProperty( glns->globj, GLU_DISPLAY_MODE, GLU_FILL );
  gluNurbsProperty( glns->globj, GLU_CULLING, GL_TRUE );
  gluNurbsProperty( glns->globj, GLU_AUTO_LOAD_MATRIX, GL_TRUE );
  /* gluNurbsCallback( glns->globj, GLU_ERROR, (void *)callback ); */
}

void rkglNURBS3DSetObjSamplingTolerance(rkglNURBS3D *glns, float tol)
{
  gluNurbsProperty(glns->globj, GLU_SAMPLING_TOLERANCE, tol );
}

void rkglNURBS3DOnDrawBothFaces(rkglNURBS3D *glns, bool flag)
{
  glns->on_both_faces = flag;
}

void rkglNURBS3DOnDrawFill(rkglNURBS3D *glns)
{
  gluNurbsProperty( glns->globj, GLU_DISPLAY_MODE, GLU_FILL );
}

void rkglNURBS3DOnDrawPolygon(rkglNURBS3D *glns)
{
  gluNurbsProperty( glns->globj, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON );
}

static void _rkglNURBS3DInit(rkglNURBS3D *glns);
void _rkglNURBS3DInit(rkglNURBS3D *glns)
{
  glns->ns = NULL;
  glns->knot[0] = NULL;
  glns->knot[1] = NULL;
  glns->cp = NULL;
  glns->knotn[0] = 0;
  glns->knotn[1] = 0;
  glns->cpn = 0;
}

bool rkglNURBS3DCreate(rkglNURBS3D *glns)
{
  glns->globj = gluNewNurbsRenderer();
  rkglNURBS3DSetObjDefault( glns );
  glns->on_both_faces = false;
  _rkglNURBS3DInit( glns );
  zOpticalInfoCreateSimple( &glns->oi, 0.6, 0.3, 0.1, "nurbs" );
  return true;
}

void rkglNURBS3DDestroy(rkglNURBS3D *glns)
{
  gluDeleteNurbsRenderer( glns->globj );
  if( !glns->ns )
    rkglNURBS3DUnLoad( glns );
  zOpticalInfoDestroy( &glns->oi );

}

bool rkglNURBS3DSetNURBS(rkglNURBS3D *glns, zNURBS3D *ns)
{
  register int i, j;

  for( i=0; i<2; i++ )
    if( glns->knotn[i] != zNURBS3DKnotNum(ns,i) || glns->cpn != zNURBS3DCPNum(ns) )
      return false;

  glns->ns = ns;
  for( i=0; i<2; i++ )
    for( j=0; j<zNURBS3DKnotNum(ns,i); j++ )
      glns->knot[i][j] = zNURBS3DKnot(ns,i,j);

  for( i=0; i<zNURBS3DCPNum(ns); i++ ){
    for( j=0; j<3; j++ )
      glns->cp[4*i+j] = zNURBS3DWeightE(ns,i) * zNURBS3DCPE(ns,i)->e[j];
    glns->cp[4*i+3] = zNURBS3DWeightE(ns,i);
  }
  return true;
}

bool rkglNURBS3DLoad(rkglNURBS3D *glns, zNURBS3D *ns)
{
  register int i;

  /* allocate buffers */
  for( i=0; i<2; i++ )
    if( zNURBS3DKnotNum(ns,i) > glns->knotn[i] ){
      glns->knotn[i] = zNURBS3DKnotNum(ns,i);
      glns->knot[i] = zAlloc( GLfloat, zNURBS3DKnotNum(ns,i) );
    }
  if( zNURBS3DCPNum(ns) > glns->cpn ){
    glns->cpn = zNURBS3DCPNum(ns);
    glns->cp = zAlloc( GLfloat, zNURBS3DCPNum(ns)*4 );
  }

  if( !glns->knot[0] || !glns->knot[1] || !glns->cp ){
    rkglNURBS3DUnLoad( glns );
    ZALLOCERROR();
    return false;
  }

  rkglNURBS3DSetNURBS( glns, ns );

  return true;
}

void rkglNURBS3DUnLoad(rkglNURBS3D *glns)
{
  zFree( glns->knot[0] );
  zFree( glns->knot[1] );
  zFree( glns->cp );
  _rkglNURBS3DInit( glns );
}

void rkglNURBS3DCreateOpticalInfo(rkglNURBS3D *glns, float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, double ns, double sns, double alpha, char *name)
{
  zOpticalInfoCreate( &glns->oi, ar, ag, ab, dr, dg, db, sr, sg, sb, ns, sns, alpha, name );
}

void rkglNURBS3DCreateOpticalInfoSimple(rkglNURBS3D *glns, float r, float g, float b, char *name)
{
  zOpticalInfoCreateSimple( &glns->oi, r, g, b, name );
}

void rkglNURBS3DSetOpticalInfo(rkglNURBS3D *glns, zOpticalInfo *oi)
{
  zOpticalInfoDestroy( &glns->oi );
  zOpticalInfoClone( oi, &glns->oi );
}

void rkglNURBS3DDraw(rkglNURBS3D *glns)
{
  glShadeModel( GL_SMOOTH );
  glEnable( GL_AUTO_NORMAL );
  glEnable( GL_NORMALIZE );
  if( glns->on_both_faces )
    glDisable(GL_CULL_FACE);

  rkglMaterial( &glns->oi );
  gluBeginSurface( glns->globj );
  gluNurbsSurface( glns->globj,
                   zNURBS3DKnotNum(glns->ns,0), glns->knot[0],
                   zNURBS3DKnotNum(glns->ns,1), glns->knot[1],
                   4 * zNURBS3DCPSize(glns->ns,1), 4,
                   glns->cp,
                   zNURBS3DOrder(glns->ns,0), zNURBS3DOrder(glns->ns,1),
                   GL_MAP2_VERTEX_4);
  gluEndSurface( glns->globj );

  if( glns->on_both_faces )
    glEnable(GL_CULL_FACE);
  glDisable( GL_NORMALIZE );
  glDisable( GL_AUTO_NORMAL );
}

/* for debug */
void rkglNURBS3DFWrite(FILE *fp, rkglNURBS3D *glns)
{
  register int i, j;

  fprintf( fp, "globj: %p\n", glns->globj );
  fprintf( fp, "optic\n" );
  zOpticalInfoFWrite( fp, &glns->oi );
  fprintf( fp, "knotn: %d %d\n", glns->knotn[0], glns->knotn[1] );
  for( i=0; i<2; i++ ){
    fprintf( fp, "knot %d\n", i );
    for( j=0; j<zNURBS3DKnotNum(glns->ns,i); j++ )
      fprintf( fp, " %g", glns->knot[i][j] );
    fprintf( fp, "\n" );
  }
  fprintf( fp, "cpn: %d\n", glns->cpn );
  fprintf( fp, "cp\n" );
  for( i=0; i<zNURBS3DCPNum(glns->ns); i++ ){
    for( j=0; j<4; j++ )
      fprintf( fp, " %g", glns->cp[4*i+j] );
    fprintf( fp, "\n" );
  }
}
