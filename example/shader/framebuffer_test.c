#include <roki_gl/roki_glut.h>

int sphere_solid_id, sphere_wf_id;
int cylinder_solid_id, cylinder_wf_id;
int cone_solid_id, cone_wf_id;
int checkerboard_id;

rkglCamera cam;
rkglLight light;

#define TEX_WIDTH  640
#define TEX_HEIGHT 480
GLuint rb_id;  /* render buffer ID */
GLuint fb_id;  /* frame buffer ID */
GLuint tex_id; /* texture ID */

GLuint shader_program[2];

ubyte dispswitch = 0;

void square_tex(GLfloat norm[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat v4[3])
{
  glBegin( GL_TRIANGLE_STRIP );
    glNormal3fv( norm );
    glTexCoord2f( 0.0, 0.0 ); glVertex3fv( v1 );
    glTexCoord2f( 1.0, 0.0 ); glVertex3fv( v2 );
    glTexCoord2f( 0.0, 1.0 ); glVertex3fv( v4 );
    glTexCoord2f( 1.0, 1.0 ); glVertex3fv( v3 );
  glEnd();
}

void square(GLfloat norm[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat v4[3])
{
  glBegin( GL_TRIANGLE_STRIP );
    glNormal3fv( norm );
    glVertex3fv( v1 );
    glVertex3fv( v2 );
    glVertex3fv( v4 );
    glVertex3fv( v3 );
  glEnd();
}

void draw(void)
{
  GLfloat vert[8][3] = {
    { 2, -2, -2 },
    { 2,  2, -2 },
    { 2,  2,  2 },
    { 2, -2,  2 },
    {-2,  2, -2 },
    {-2, -2, -2 },
    {-2, -2,  2 },
    {-2,  2,  2 },
  };
  GLfloat norm[6][3] = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
    {-1, 0, 0 },
    { 0,-1, 0 },
    { 0, 0,-1 },
  };
  zOpticalInfo oi;

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterial( &oi );
  glEnable( GL_TEXTURE_2D );
  rkglShaderSetTexture0( shader_program[1], 0 );
  square_tex( norm[0], vert[0], vert[1], vert[2], vert[3] );
  rkglTextureUnbind();
  square( norm[1], vert[2], vert[1], vert[4], vert[7] );
  square( norm[2], vert[3], vert[2], vert[7], vert[6] );
  square( norm[3], vert[4], vert[5], vert[6], vert[7] );
  square( norm[4], vert[0], vert[3], vert[6], vert[5] );
  square( norm[5], vert[1], vert[0], vert[5], vert[4] );
}

void display(void)
{
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb_id );
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();
  glUseProgram( shader_program[0] );
  glCallList( checkerboard_id );
  glCallList( sphere_solid_id );
  glCallList( cylinder_solid_id );
  glCallList( cone_solid_id );
  if( dispswitch ){
    glCallList( sphere_wf_id );
    glCallList( cylinder_wf_id );
    glCallList( cone_wf_id );
  }
  glUseProgram( 0 );
  glPopMatrix();
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  rkglTextureBindUnit( 0, tex_id );

  rkglCameraPut( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();
  glUseProgram( shader_program[0] );
  glCallList( checkerboard_id );
  glUseProgram( 0 );
  glUseProgram( shader_program[1] );
  draw();
  glUseProgram( 0 );
  glPopMatrix();

  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
  case 'w': dispswitch = 1 - dispswitch; break;
  case 'q': case 'Q': case '\033':
    glDeleteTextures( 1, &tex_id );
    glDeleteRenderbuffersEXT( 1, &rb_id );
    glDeleteFramebuffersEXT( 1, &fb_id );
    glDeleteProgram( shader_program[0] );
    glDeleteProgram( shader_program[1] );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c1, c2;
  zVec3D pc0, pc1, pc2;
  zOpticalInfo oi, oi2;
  zSphere3D sphere;
  zCyl3D cylinder;
  zCone3D cone;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 15, 0, 5, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 100 );
  rkglSetDefaultCamera( &cam );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 0, 20 );
  rkglLightSetAttenuationQuad( &light, 1.0 );

  zOpticalInfoCreateSimple( &oi, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 0, 0, 0 );
  zSphere3DCreate( &sphere, &c1, 1, 16 );
  sphere_solid_id = rkglBeginList();
  rkglMaterial( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();
  sphere_wf_id = rkglBeginList();
  rkglSphere( &sphere, RKGL_WIREFRAME );
  glEndList();

  zOpticalInfoCreateSimple( &oi, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 0, 2,-1.5 );
  zVec3DCreate( &c2, 0, 1, 1.5 );
  zCyl3DCreate( &cylinder, &c1, &c2, 0.5, 16 );
  cylinder_solid_id = rkglBeginList();
  rkglMaterial( &oi );
  rkglCyl( &cylinder, RKGL_FACE );
  glEndList();
  cylinder_wf_id = rkglBeginList();
  rkglCyl( &cylinder, RKGL_WIREFRAME );
  glEndList();

  zOpticalInfoCreateSimple( &oi, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 0,-2,-2 );
  zVec3DCreate( &c2, 0,-1, 1 );
  zCone3DCreate( &cone, &c1, &c2, 1, 16 );
  cone_solid_id = rkglBeginList();
  rkglMaterial( &oi );
  rkglCone( &cone, RKGL_FACE );
  glEndList();
  cone_wf_id = rkglBeginList();
  rkglCone( &cone, RKGL_WIREFRAME );
  glEndList();

  zOpticalInfoCreateSimple( &oi,  1.0, 0.0, 0.0, NULL ); /* red */
  zOpticalInfoCreateSimple( &oi2, 1.0, 1.0, 1.0, NULL ); /* white */
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2, -10,  10, -3 );
  checkerboard_id = rkglBeginList();
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  glEndList();

  tex_id = rkglTextureAssign( TEX_WIDTH, TEX_HEIGHT, NULL );
  fb_id = rkglFramebufferAttachTexture( tex_id );
  rb_id = rkglFramebufferAttachRenderbuffer( TEX_WIDTH, TEX_HEIGHT );

  shader_program[0] = rkglShaderCreatePhong();
  shader_program[1] = rkglShaderCreateTexture();
  glUseProgram( shader_program[1] );
  rkglShaderSetTextureMixRate0( shader_program[1], 1.0 );
  glUseProgram( 0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, TEX_WIDTH, TEX_HEIGHT, argv[0] );

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
