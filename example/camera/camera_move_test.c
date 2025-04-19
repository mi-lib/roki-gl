#include <roki_gl/roki_glut.h>

rkglCamera cam;
rkglLight light;
GLint obj_id;
bool move = false;

zFrame3D platform;

void camera_home(void)
{
  rkglCameraSetViewframe( &cam, 6, 0, 0, 0, 0, 0 );
}

void move_platform(double angle)
{
  platform.pos.c.z = 1.0 * sin( angle );
  zMat3DRotYawDRC( &platform.att, 0.00001 * angle );
}

void display(void)
{
  static int i = 0;

  rkglLightPut( &light );
  rkglCameraPut( &cam );
  if( move )
    move_platform( 0.001 * zPIx2 * i++ );
  rkglXformInv( &platform );
  glPushMatrix();
  rkglClear();
  glCallList( obj_id );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/320, 1, 30 );
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'a': camera_home(); break;
  case 'h': rkglCameraMoveLeft(  &cam, 0.1 ); break;
  case 'j': rkglCameraMoveDown(  &cam, 0.1 ); break;
  case 'k': rkglCameraMoveUp(    &cam, 0.1 ); break;
  case 'l': rkglCameraMoveRight( &cam, 0.1 ); break;
  case 'z': rkglCameraZoomIn(    &cam, 0.1 ); break;
  case 'Z': rkglCameraZoomOut(   &cam, 0.1 ); break;
  case 'y': rkglCameraRotate(    &cam, 2.0, 0, 0, 1 ); break;
  case 'Y': rkglCameraRotate(    &cam,-2.0, 0, 0, 1 ); break;
  case 'p': rkglCameraRotate(    &cam, 2.0, 0, 1, 0 ); break;
  case 'P': rkglCameraRotate(    &cam,-2.0, 0, 1, 0 ); break;
  case 'r': rkglCameraRotate(    &cam, 2.0, 1, 0, 0 ); break;
  case 'R': rkglCameraRotate(    &cam,-2.0, 1, 0, 0 ); break;
  case 'g': rkglCameraLookAt(    &cam, 6, 2, 1, 0, 0, 0, 0, 0, 1 ); break;
  case 'm': move = 1 - move; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zBox3D box;
  zOpticalInfo red;
  GLfloat rgba_white[4] = { 1.0, 1.0, 1.0, 1.0 };

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 1, 0, 10 );

  rkglSetDefaultCamera( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  camera_home();

  zOpticalInfoCreateSimple( &red, 1.0, 0, 0, NULL );
  zBox3DCreateAlign( &box, ZVEC3DZERO, 1.0, 0.8, 0.6 );
  obj_id = rkglBeginList();
  rkglGauge( zX, 6.0, zY, 6.0, 1.0, 0.2, rgba_white );
  rkglFrame( ZFRAME3DIDENT, 2, 3 );
  rkglMaterial( &red );
  rkglBox( &box, RKGL_FACE | RKGL_WIREFRAME );
  glEndList();

  zFrame3DIdent( &platform );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutReshapeFunc( resize );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  glutIdleFunc( rkglIdleFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
