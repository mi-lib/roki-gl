#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;

zRGB rgb;
zOpticalInfo oi;
zNURBS3D nurbs;

bool show_ctl = false;
bool show_wf = false;

void init_surface(void)
{
  int i, j;

  for( i=0; i<zNURBS3DCPNum(&nurbs,0); i++ )
    for( j=0; j<zNURBS3DCPNum(&nurbs,1); j++ ){
      zNURBS3DCP(&nurbs,i,j)->e[0] = 2 * ( (double)i/zNURBS3DCPNum(&nurbs,0) - 0.5 );
      zNURBS3DCP(&nurbs,i,j)->e[1] = 2 * ( (double)j/zNURBS3DCPNum(&nurbs,1) - 0.5 );
      zNURBS3DCP(&nurbs,i,j)->e[2] =
        ( i == 0 || i == zNURBS3DCPNum(&nurbs,0)-1 || j == 0 || j == zNURBS3DCPNum(&nurbs,1)-1 ) ? 0 : zRandF( -1.0, 1.0 );
    }
  zOpticalInfoCreateSimple( &oi, zRandF(0.0,1.0), zRandF(0.0,1.0), zRandF(0.0,1.0), NULL );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q':
    zNURBS3DDestroy( &nurbs );
    exit(1);
    break;
  case 'i':
    init_surface();
    glutPostRedisplay();
    break;
  case 'w':
    show_wf = 1 - show_wf;
    glutPostRedisplay();
    break;
  case 'p':
    show_ctl = 1 - show_ctl;
    glutPostRedisplay();
    break;
  default: ;
  }
}

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );
  rkglClear();
  glPushMatrix();

  rkglMaterial( &oi );
  rkglNURBS( &nurbs, RKGL_FACE | ( show_wf ? 0 : RKGL_WIREFRAME ) );
  if( show_ctl )
    rkglNURBSCP( &nurbs, 5.0, &rgb );

  glPopMatrix();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglPerspective( &cam, 45.0, (GLdouble)w/(GLdouble)h, 1.0, 10.0 );
}

void idle(void){ glutPostRedisplay(); }

void init(void)
{
  zRandInit();
  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.0, 0.0, 0.0 );
  rkglCALookAt( &cam, 3, 0, 1, 0, 0, 0, 0, 0, 1 );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
  glEnable( GL_AUTO_NORMAL );
  glDisable( GL_CULL_FACE );
  rkglLightCreate( &light, 0.0, 0.0, 0.0, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );

  zRGBSet( &rgb, 0.5, 1.0, 0.5 );
  zNURBS3DAlloc( &nurbs, 6, 6, 3, 3 );
  zNURBS3DSetSliceNum( &nurbs, 50, 50 );
  init_surface();
}

int main(int argc, char **argv)
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutReshapeFunc( reshape );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  glutKeyboardFunc( keyboard );
  init();
  glutMainLoop();
  return 0;
}
