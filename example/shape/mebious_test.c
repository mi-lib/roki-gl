#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

zOpticalInfo silver;
int mebious_id;
rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 1, 0 );
  rkglClear();
  glCallList( mebious_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 0.001, 2, 10 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 ); break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 ); break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 ); break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 ); break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 ); break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 ); break;
  case '8': rkglCARelMove( &cam, 0.05, 0, 0 ); break;
  case '*': rkglCARelMove( &cam,-0.05, 0, 0 ); break;
  case '9': rkglCARelMove( &cam, 0, 0.05, 0 ); break;
  case '(': rkglCARelMove( &cam, 0,-0.05, 0 ); break;
  case '0': rkglCARelMove( &cam, 0, 0, 0.05 ); break;
  case ')': rkglCARelMove( &cam, 0, 0,-0.05 ); break;
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

#define R 0.2
#define H 0.06
#define D 0.04

#define DIV_NUM 100

void make_mebious(void)
{
  register int i;
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
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCALookAt( &cam, 5, 0, 3, 0, 0, 0, 0, 0, 1 );

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
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
