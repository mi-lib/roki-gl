#include <roki_gl/roki_glut.h>

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

void generate_pc(zVec3DData *pointdata, char *filename)
{
  if( !zVec3DDataReadPCDFile( pointdata, filename ) )
    exit( EXIT_FAILURE );
}

int main(int argc, char *argv[])
{
  zVec3DData pointdata;
  zVec3DData normaldata;

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  zVec3DDataReadPCDFile( &pointdata, "bun_zipper.pcd" );
  zVec3DDataReadPCDFile( &normaldata, "normal.pcd" );
  pc_id = rkglBeginList();
  rkglPointCloudNormal( &pointdata, &normaldata, 1, 0.01 );
  glEndList();
  zVec3DDataDestroy( &pointdata );
  zVec3DDataDestroy( &normaldata );
  glutMainLoop();
  return 0;
}
