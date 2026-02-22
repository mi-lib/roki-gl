#include <roki_gl/roki_glut.h>

#define ENTRY_POINT 0x1
#define ENTRY_NORM  0x2
#define ENTRY_VAR   0x4
#define ENTRY_MESH  0x8
#define ENTRY_PATCH 0x10

rkglCamera cam;
rkglLight light;

int terra_id[5];
byte terra_entry_sw;

void display(void)
{
  rkglCameraPut( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  if( terra_entry_sw & ENTRY_POINT ) glCallList( terra_id[0] );
  if( terra_entry_sw & ENTRY_NORM  ) glCallList( terra_id[1] );
  if( terra_entry_sw & ENTRY_VAR   ) glCallList( terra_id[2] );
  if( terra_entry_sw & ENTRY_MESH  ) glCallList( terra_id[3] );
  if( terra_entry_sw & ENTRY_PATCH ) glCallList( terra_id[4] );
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'p': terra_entry_sw ^= ENTRY_POINT; break;
  case 'n': terra_entry_sw ^= ENTRY_NORM;  break;
  case 'v': terra_entry_sw ^= ENTRY_VAR;   break;
  case 'm': terra_entry_sw ^= ENTRY_MESH;  break;
  case 'f': terra_entry_sw ^= ENTRY_PATCH; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zMapNet mn;
  zTerra *terra;
  zOpticalInfo oi1, oi2;
  zRGB rgb1, rgb2;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.3, 0.3, 0.3 );
  rkglCameraLookAt( &cam, 15, 2, 3, 0, 2, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 200 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 0, 10 );

  zMapNetReadZTK( &mn, "test.ztk" );
  terra = zMapNetMap( &mn, 0 )->body;
  /* points */
  zRGBSet( &rgb1, 1, 0, 0 );
  zRGBSet( &rgb2, 0, 0, 1 );
  terra_id[0] = rkglBeginList();
  rkglTerraPoint( terra, &rgb1, &rgb2 );
  glEndList();
  /* normal vectors */
  zOpticalInfoCreate( &oi1, 0, 0, 0, 0.6, 0.4, 0, 0, 0, 0, 0, 0, 0.8, NULL );
  terra_id[1] = rkglBeginList();
  rkglTerraNorm( terra, &oi1 );
  glEndList();
  /* height variance */
  terra_id[2] = rkglBeginList();
  zOpticalInfoCreate( &oi1, 0, 0, 0, 0, 0.8, 0, 0, 0, 0, 0, 0, 0.6, NULL );
  rkglTerraVar( terra, &oi1 );
  glEndList();
  /* mesh */
  zOpticalInfoCreate( &oi1, 0, 0, 0, 0, 0.4, 0, 0, 0, 0, 0, 0, 1, NULL );
  zOpticalInfoCreate( &oi2, 0, 0, 0, 0.7, 0.7, 0, 0, 0, 0, 0, 0, 1, NULL );
  terra_id[3] = rkglBeginList();
  rkglTerraMesh( terra, &oi1, &oi2 );
  glEndList();
  /* patch */
  zOpticalInfoCreate( &oi1, 0, 0, 0, 0.4, 0.8, 0, 0, 0, 0, 0, 0, 0.8, NULL );
  terra_id[4] = rkglBeginList();
  rkglTerraPatch( terra, &oi1 );
  glEndList();

  zMapNetDestroy( &mn );
  terra_entry_sw = ENTRY_MESH;
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
