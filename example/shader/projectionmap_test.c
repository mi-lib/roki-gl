#include <roki_gl/roki_glut.h>

int box_id;
int sphere_id;
int room_id;

rkglCamera cam;
rkglLight light;

zSphere3D sphere;
zBox3D box;

zTexture tex;

GLuint shader_program;

void resize(int w, int h)
{
  rkglCameraSetViewport( &cam, 0, 0, w, h );
  rkglCameraScaleFrustumHeight( &cam, 1.0/160, 2, 100 );
}

void display(void)
{
  glMatrixMode( GL_TEXTURE );
  glLoadIdentity();
  glTranslatef( 0.5, 0.5, 0.0 );
  gluPerspective( 90.0, 1.0, 1.0, 100.0 );
  gluLookAt( light.pos[0], light.pos[1], light.pos[2], 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 );

  rkglCameraLoadViewframe( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();

  rkglTextureAssignUnit( &tex, 0 );
  rkglShaderSetProjectionTexture( shader_program, 0 );
  glUseProgram( shader_program );
  glCallList( box_id );
  glCallList( sphere_id );
  glCallList( room_id );
  glPopMatrix();
  glUseProgram( 0 );
  rkglTextureUnbind();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'q': case 'Q': case '\033':
    rkglTextureDelete( &tex );
    glDeleteProgram( shader_program );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c, pc0, pc1, pc2;
  zOpticalInfo oi, oi2;

  rkglSetDefaultCallbackParam( &cam, 0, 0, 0, 0, 0 );

  rkglCameraSetBackground( &cam, 0.5, 1.0, 1.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 15 );
  rkglLightSetAttenuationConst( &light, 1.0 );

  rkglCameraLookAt( &cam, light.pos[0], light.pos[1], light.pos[2], 0, 0, 0, -1, 0, 0 );

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
  shader_program = rkglShaderCreateProjection();
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
