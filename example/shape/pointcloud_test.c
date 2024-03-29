#include <roki_gl/roki_glut.h>

zVec3DList pc;

rkglCamera cam;
rkglLight light;

double azim = 0;

int pc_id; /* display list of point cloud */

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  glCallList( pc_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScaleH( &cam, 1.0/5120, 0.1, 5 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case ' ':
    azim += zDeg2Rad(10.0);
    break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.1, 0.1, 0.1 );
  rkglCALookAt( &cam, 0.3*cos(azim), 0.3*sin(azim), 0.6, 0.0, 0.0, 0.1, 0.0, 0.0, 1.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 3, 10 );
}

void generate_pc(char *filename)
{
  zMShape3D ms;
  zShape3D *s;
  zTri3D *t;
  register int i;

  zListInit( &pc );
  if( !zMShape3DReadZTK( &ms, filename ) )
    exit( EXIT_FAILURE );
  s = zMShape3DShape( &ms, 0 );
  for( i=0; i<zShape3DFaceNum(s); i++ ){
    t = zShape3DFace(s,i);
    if( zTri3DNorm(t)->e[zX] < 0 ) continue;
    zVec3DListAdd( &pc, zTri3DVert(t,0) );
    zVec3DListAdd( &pc, zTri3DVert(t,1) );
    zVec3DListAdd( &pc, zTri3DVert(t,2) );
  }
}

int main(int argc, char *argv[])
{
  char *filename = argc > 1 ? argv[1] : "../model/bunny.ztk";
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  generate_pc( filename );
  pc_id = rkglBeginList();
  rkglPointCloud( &pc, ZVEC3DZERO, 1 );
  glEndList();
  zVec3DListDestroy( &pc );
  glutMainLoop();
  return 0;
}
