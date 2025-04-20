#include <roki_gl/roki_glut.h>

zOpticalInfo silver;
int mebious_id;
rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 1, 0 );
  rkglClear();
  glCallList( mebious_id );
  glPopMatrix();
  glutSwapBuffers();
}

#define R 0.2
#define H 0.06
#define D 0.04

#define DIV_NUM 100

void make_mebious(void)
{
  int i;
  double t, t2;
  zVec3D p, r, d;
  zVec3D vert[2*DIV_NUM+2];
  zTri3D face[2*DIV_NUM];
  zPH3D ph;

  /* create vertices */
  for( i=0; i<=DIV_NUM; i++ ){
    t = 2 * zPI * (double)i/DIV_NUM;
    t2 = 2 * t;
    zVec3DCreate( &p, R * cos( t ), R * sin( t ), H * sin( t2 ) );
    zVec3DCreate( &d, -R*sin( t ), R*cos( t ), 2*H*cos( t2 ) );
    zVec3DNormalizeDRC( &d );
    zVec3DCreate( &r, D*cos(t), D*sin(t), 0 );
    zVec3DMulDRC( &d, 0.5 * t );
    zVec3DRot( &r, &d, &r );
    zVec3DAdd( &p, &r, &vert[2*i] );
    zVec3DSub( &p, &r, &vert[2*i+1] );
  }
  /* create polygons */
  for( i=0; i<DIV_NUM; i++ ){
    zTri3DCreate( &face[2*i], &vert[2*i], &vert[2*i+1], &vert[2*i+2] );
    zTri3DCreate( &face[2*i+1], &vert[2*i+1], &vert[2*i+3], &vert[2*i+2] );
  }
  /* create polyhedron */
  zPH3DInit( &ph );
  zPH3DSetVertNum( &ph, 2 * DIV_NUM + 2 );
  zPH3DSetFaceNum( &ph, 2 * DIV_NUM );
  zPH3DSetVertBuf( &ph, vert );
  zPH3DSetFaceBuf( &ph, face );

  mebious_id = rkglBeginList();
  rkglPH( &ph, RKGL_FACE );
  glEndList();
}

void init(void)
{
  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 2, 0, 1, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 100 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 10 );

  make_mebious();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
