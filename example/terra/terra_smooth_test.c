#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zOpticalInfo oi;
zTerra terra;
int id_curve = -1;
int id_cp = -1;

bool show_net = false;

void refresh(void)
{
  register int i, j;
  zRGB rgb;

  rkglDeleteList( id_curve );
  rkglDeleteList( id_cp );
  for( i=1; i<zTerraXSize(&terra)-1; i++ )
    for( j=1; j<zTerraYSize(&terra)-1; j++ )
      zTerraGridNC(&terra,i,j)->z = zRandI(-1,1);
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );
  /* curve */
  id_curve = rkglBeginList();
  rkglTerraMeshSmooth( &terra, 20, 20, &oi );
  glEndList();
  /* control points */
  id_cp = rkglBeginList();
  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  rkglTerraPointNet( &terra, &rgb );
  glEndList();
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  glCallList( id_curve );
  if( show_net )
    glCallList( id_cp );
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 30.0, (GLdouble)w/(GLdouble)h, 1.0, 30.0 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q':
    zTerraDestroy( &terra );
    exit( 1 );
    break;
  case 'i':
    refresh();
    glutPostRedisplay();
    break;
  case 'p':
    show_net = 1 - show_net;
    glutPostRedisplay();
    break;
  default: ;
  }
}

void init()
{
  zRandInit();
  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.8, 0.8, 0.8 );
  rkglCALookAt( &cam, 5, 0, 5, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 20 );

  zTerraAllocRegion( &terra, -1.0, 1.0, -1.0, 1.0, -HUGE_VAL, HUGE_VAL, 0.4, 0.5 );
  refresh();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 500, 500, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
