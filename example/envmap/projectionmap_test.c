#include <roki_gl/roki_glut.h>

int box_id;
int sphere_id;
int room_id;

rkglCamera cam;
rkglLight light;

zSphere3D sphere;
zBox3D box;

zTexture tex;

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScaleH( &cam, 1.0/160, 2, 100 );
}

void display(void)
{
  glMatrixMode( GL_TEXTURE );
  glLoadIdentity();
  glTranslatef( 0.5, 0.5, 0.0 );
  gluPerspective( 90.0, 1.0, 1.0, 100.0 );
  gluLookAt( light.pos[0], light.pos[1], light.pos[2], 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 );

  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();
  rkglTextureBind( &tex );
  rkglTextureEnableProjection();
  glCallList( room_id );
  glCallList( sphere_id );
  glCallList( box_id );
  glPopMatrix();
  rkglTextureDisableProjection();
  rkglTextureUnbind();
  glutSwapBuffers();
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
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c, pc0, pc1, pc2;
  zOpticalInfo oi, oi2;

  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 1.0, 1.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 10 );
  rkglLightSetAttenuationLinear( &light, 0.5 );

  rkglCALookAt( &cam, light.pos[0], light.pos[1], light.pos[2], 0, 0, 0, -1, 0, 0 );

  room_id = rkglBeginList();
  zOpticalInfoCreateSimple( &oi2, 1.0, 1.0, 1.0, NULL );
  zOpticalInfoCreateSimple( &oi,  0.7, 0.7, 0.7, NULL );
  zVec3DCreate( &pc0, -30, -30, -3 );
  zVec3DCreate( &pc1,  30, -30, -3 );
  zVec3DCreate( &pc2, -30,  30, -3 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );

  zVec3DCreate( &c, 0, 2, 3 );
  zSphere3DCreate( &sphere, &c, 1.5, 16 );
  sphere_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi2 );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();
  zVec3DCreate( &c, 0, -2, 3 );
  zBox3DCreateAlign( &box, &c, 3.0, 3.0, 3.0 );
  box_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi2 );
  rkglBox( &box, RKGL_FACE );
  glEndList();

  rkglTextureReadFile( &tex, "../fig/lena_mini.jpg" );
  rkglTextureBind( &tex );
  rkglTextureGenProjectionObject();
  rkglTextureUnbind();
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
