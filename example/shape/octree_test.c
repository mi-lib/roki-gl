#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

int pc_octree_id; /* display list of octree of point cloud */
int pc_normal_id; /* display list of normal vectors of point cloud */
bool show_octree = true;
bool show_normal = false;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  if( show_octree )
    glCallList( pc_octree_id );
  if( show_normal )
    glCallList( pc_normal_id );
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'n': show_normal = 1 - show_normal; break;
  case 'o': show_octree = 1 - show_octree; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
  glutPostRedisplay();
}

void init(void)
{
  rkglCameraSetBackground( &cam, 0.1, 0.1, 0.5 );
  rkglCameraLookAt( &cam, 0.5, 0.0, 0.2, 0.0, 0.0, 0.1, 0.0, 0.0, 1.0 );
  rkglCameraFitPerspective( &cam, 45, 0.1, 200 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 0, 10 );
}

void generate_octree(zVec3DOctree *octree, char *filename, double resolution)
{
  zMShape3D ms;
  zVec3DData pointdata;
  zAABox3D aabb;

  if( !zMShape3DReadZTK( &ms, filename ) )
    exit( EXIT_FAILURE );
  zMShape3DVertData( &ms, &pointdata );
  zMShape3DDestroy( &ms );

  zVec3DDataAABB( &pointdata, &aabb, NULL );
  zVec3DOctreeInitAuto( octree, &aabb, resolution );
  zVec3DOctreeAddData( octree, &pointdata );
  zVec3DOctreeUpdateNormal( octree );
  zVec3DDataDestroy( &pointdata );
}

int main(int argc, char *argv[])
{
  zVec3DOctree octree;

  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 640, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  generate_octree( &octree, argc > 1 ? argv[1] : "../model/bunny.ztk", 0.002 );

  pc_octree_id = rkglBeginList();
  rkglOctree( &octree );
  glEndList();
  pc_normal_id = rkglBeginList();
  rkglOctreeNormal( &octree, 0.005 );
  glEndList();
  glutMainLoop();
  return 0;
}
