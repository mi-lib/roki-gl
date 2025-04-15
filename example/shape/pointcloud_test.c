#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

int pc_id;     /* display list of point cloud */
int pc_cov_id; /* display list of covariance ellipsoid */
bool show_cov_ellipsoid = false;

void display(void)
{
  rkglCameraLoadViewframe( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  glCallList( pc_id );
  if( show_cov_ellipsoid )
    glCallList( pc_cov_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/5120, 0.1, 5 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'e': show_cov_ellipsoid = 1 - show_cov_ellipsoid; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

void init(void)
{
  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0.01, 0.01 );

  rkglCameraSetBackground( &cam, 0.1, 0.1, 0.1 );
  rkglCameraLookAt( &cam, 0.3, 0.0, 0.1, 0.0, 0.0, 0.1, 0.0, 0.0, 1.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 3, 10 );
}

void generate_pc(zVec3DData *pointdata, char *filename)
{
  zMShape3D ms;

  if( !zMShape3DReadZTK( &ms, filename ) )
    exit( EXIT_FAILURE );
  zMShape3DVertData( &ms, pointdata );
  zMShape3DDestroy( &ms );
}

void generate_covariance_ellipsoid(zVec3DData *pointdata)
{
  zVec3D center;
  zMat3D cov;
  zOpticalInfo oi;

  zOpticalInfoCreate( &oi, 0.5, 0.5, 0.5, 0.8, 0.6, 0.3, 0, 0, 0, 0, 0, 0.6, NULL );
  zVec3DDataBaryCov( pointdata, &center, &cov );
  rkglMaterial( &oi );
  rkglEllipsBaryCov( &center, &cov );
}

int main(int argc, char *argv[])
{
  zVec3DData pointdata;

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  generate_pc( &pointdata, argc > 1 ? argv[1] : "../model/bunny.ztk" );
  pc_id = rkglBeginList();
  rkglPointCloud( &pointdata, 1 );
  glEndList();
  pc_cov_id = rkglBeginList();
  generate_covariance_ellipsoid( &pointdata );
  glEndList();
  zVec3DDataDestroy( &pointdata );
  glutMainLoop();
  return 0;
}
