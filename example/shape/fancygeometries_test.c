#include <roki_gl/roki_glut.h>

int ax_id, ay_id, g_id, f_id, a_id;
int checker_id;

double r = 0;

rkglCamera cam;
rkglLight light;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 0, 1 );
  rkglClear();
  glCallList( checker_id );
  glCallList( g_id );
  glCallList( ax_id );
  glCallList( ay_id );
  glCallList( f_id );
  glCallList( a_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScaleH( &cam, 1.0/160, 1, 10 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'u': rkglCALockonPTR( &cam, 5, 0, 0 );  break;
  case 'U': rkglCALockonPTR( &cam,-5, 0, 0 );  break;
  case 'i': rkglCALockonPTR( &cam, 0, 5, 0 );  break;
  case 'I': rkglCALockonPTR( &cam, 0,-5, 0 );  break;
  case 'o': rkglCALockonPTR( &cam, 0, 0, 5 );  break;
  case 'O': rkglCALockonPTR( &cam, 0, 0,-5 );  break;
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

void init(void)
{
  zVec3D pc0, pc1, pc2;
  zVec3D bot, vec;
  GLfloat rgba_white[4] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat rgba_red[4] = { 1.0, 0.0, 0.0, 1.0 };
  zOpticalInfo red, white, yellow;

  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 4, 0, 2.4, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 3, 6 );

  /* checkerboard */
  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );
  zOpticalInfoCreateSimple( &white, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &pc0, -3.5, -5, -1 );
  zVec3DCreate( &pc1,  3.5, -5, -1 );
  zVec3DCreate( &pc2, -3.5,  5, -1 );
  checker_id = rkglBeginList();
  rkglCheckerBoard( &pc0, &pc1, &pc2, 7, 10, &red, &white );
  glEndList();
  /* gauge & axes */
  g_id = rkglBeginList();
  rkglGauge( zX, 3.01, zY, 4.51, 1.0, 0.2, rgba_white );
  glEndList();
  ax_id = rkglBeginList();
  rkglAxis( zX, 3.5, 2.0, rgba_red );
  glEndList();
  ay_id = rkglBeginList();
  rkglAxis( zY, 5.0, 2.0, rgba_red );
  glEndList();
  /* frame */
  f_id = rkglBeginList();
  rkglFrame( ZFRAME3DIDENT, 2, 3 );
  glEndList();
  /* arrow */
  zVec3DCreate( &bot, 0, 0, 0 );
  zVec3DCreate( &vec, -2.0, -1.0, 3.0 );
  zOpticalInfoCreateSimple( &yellow, 0.9, 0.9, 0.3, NULL );
  a_id = rkglBeginList();
    rkglMaterial( &yellow );
    rkglArrow( &bot, &vec, 1.0 );
  glEndList();
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
