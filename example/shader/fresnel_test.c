#include <roki_gl/roki_glut.h>

int box_id;
int sphere_id;
int room_id;

bool disp_sphere = true;

rkglCamera cam;
rkglLight light;

#define TEX_WIDTH  256
#define TEX_HEIGHT 256

GLuint shader_program0;
GLuint shader_program1;

zSphere3D sphere;
zBox3D box;

void draw(void)
{
  glUseProgram( shader_program0 );
  glCallList( room_id );
  glUseProgram( 0 );
}

void display(void)
{
  rkglReflectionRefraction( TEX_WIDTH, TEX_HEIGHT, &cam, &light, draw, zSphere3DCenter(&sphere) );
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();
  glUseProgram( shader_program1 );
  rkglShaderSetFresnelTransparency( shader_program1, 0.8 );
  rkglShaderSetFresnelReflectivity( shader_program1, 0.3 );
  rkglShaderSetFresnelRefractionIndex( shader_program1, 1.03 );
  rkglShaderSetViewMatTranspose( shader_program1 );
  rkglShaderSetFresnelEnvironmentSampler( shader_program1, 0 );
  if( disp_sphere )
    glCallList( sphere_id );
  else
    glCallList( box_id );
  glUseProgram( 0 );
  draw();
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'b': disp_sphere = 1 - disp_sphere; break;
  case 'q': case 'Q': case '\033':
    glDeleteProgram( shader_program0 );
    glDeleteProgram( shader_program1 );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c, pc0, pc1, pc2;
  zOpticalInfo oi, oi2;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 18, 0, 2, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 45, 0.1, 200 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 5 );
  rkglLightSetAttenuationLinear( &light, 0.5 );

  room_id = rkglBeginList();
  zOpticalInfoCreateSimple( &oi2, 1.0, 1.0, 1.0, NULL ); /* white */
  zOpticalInfoCreateSimple( &oi,  1.0, 0.0, 0.0, NULL ); /* red */
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2, -10,  10, -3 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 1.0, 1.0, NULL ); /* cyan */
  zVec3DCreate( &pc0, -10,  10, -3 );
  zVec3DCreate( &pc1,  10,  10, -3 );
  zVec3DCreate( &pc2, -10,  10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  1.0, 1.0, 0.0, NULL ); /* yellow */
  zVec3DCreate( &pc0,  10, -10, -3 );
  zVec3DCreate( &pc1, -10, -10, -3 );
  zVec3DCreate( &pc2,  10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 0.0, 1.0, NULL ); /* blue */
  zVec3DCreate( &pc0,  10,  10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2,  10,  10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 1.0, 0.0, NULL ); /* green */
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1, -10,  10, -3 );
  zVec3DCreate( &pc2, -10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 0.0, NULL ); /* ball 1 */
  oi.shininess = 3.0;
  zVec3DCreate( &c, 7.0, 7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c, 7.0, 7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 0.0, 0.0, 1.0, NULL ); /* ball 2 */
  oi.shininess = 3.0;
  zVec3DCreate( &c,-7.0, 7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c,-7.0, 7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 1.0, 0.0, 0.0, NULL ); /* ball 3 */
  oi.shininess = 3.0;
  zVec3DCreate( &c,-7.0,-7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c,-7.0,-7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 0.0, 1.0, 0.0, NULL ); /* ball 4 */
  oi.shininess = 3.0;
  zVec3DCreate( &c, 7.0,-7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c, 7.0,-7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();

  zOpticalInfoCreateSimple( &oi, 0.0, 0.0, 1.0, NULL );
  zVec3DCreate( &c, 0, 0, 3 );
  zSphere3DCreate( &sphere, &c, 2.5, 16 );
  sphere_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();
  zBox3DCreateAlign( &box, &c, 4.0, 4.0, 4.0 );
  box_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglBox( &box, RKGL_FACE );
  glEndList();

  rkglTextureGenCubeMap( TEX_WIDTH, TEX_HEIGHT );
  shader_program0 = rkglShaderCreatePhong();
  shader_program1 = rkglShaderCreateFresnel();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutIdleFunc( rkglIdleFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
