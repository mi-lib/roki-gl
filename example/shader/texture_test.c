#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_glsl.h>
#include <zx11/zximage.h>

GLuint shader_program;

rkglCamera cam;
rkglLight light;

zTexture tex[2];

bool make_check_texture(zTexture *texture, int width, int height, int div)
{
  int i, j, dw, dh;
  GLubyte *pt, color;

  if( !( texture->buf = zAlloc( ubyte, width*height*4 ) ) ) return false;
  dw = ( texture->width  = width  ) / div;
  dh = ( texture->height = height ) / div;
  for( pt=texture->buf, i=0; i<texture->height; i++ ){
    for( j=0; j<texture->width; j++ ){
      color = ( i/dh + j/dw ) % 2 ? 0xff : 0;
      *pt++ = color;
      *pt++ = 0;
      *pt++ = 0xff - color;
      *pt++ = 0xff;
    }
  }
  rkglTextureInit( texture );
  zFree( texture->buf );
  return true;
}

void square(GLfloat norm[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat v4[3])
{
  glBegin( GL_QUADS );
    glNormal3fv( norm );
    glTexCoord2f( 0.0, 1.0 ); glVertex3fv( v1 );
    glTexCoord2f( 1.0, 1.0 ); glVertex3fv( v2 );
    glTexCoord2f( 1.0, 0.0 ); glVertex3fv( v3 );
    glTexCoord2f( 0.0, 0.0 ); glVertex3fv( v4 );
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

  zOpticalInfoCreateSimple( &oi, 0.8, 0.4, 0.4, NULL );
  rkglMaterial( &oi );
  glEnable( GL_TEXTURE_2D );
  rkglShaderSetTexture0( shader_program, 1 );
  square( norm[0], vert[0], vert[1], vert[2], vert[3] );
  rkglShaderSetTexture0( shader_program, 0 );
  square( norm[1], vert[2], vert[1], vert[4], vert[7] );
  square( norm[2], vert[3], vert[2], vert[7], vert[6] );
  square( norm[3], vert[4], vert[5], vert[6], vert[7] );
  square( norm[4], vert[0], vert[3], vert[6], vert[5] );
  square( norm[5], vert[1], vert[0], vert[5], vert[4] );
  rkglTextureUnbind();
  glDisable( GL_TEXTURE_2D );
}

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();

  rkglTextureAssignUnit( 0, tex[0].id );
  rkglTextureAssignUnit( 1, tex[1].id );
  glUseProgram( shader_program );
  draw();
  glUseProgram( 0 );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  rkglSetCallbackParamGLUT( &cam, 2.0, 2, 60, 1.0, 5.0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 5, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 3, 10 );
  rkglLightSetAttenuationQuad( &light, 1.0 );

  make_check_texture( &tex[0], 256, 256, 4 );
  rkglTextureReadFile( &tex[1], "lena_mini.jpg" );

  shader_program = rkglShaderCreateTexture();
  glUseProgram( shader_program );
  rkglShaderSetTextureMixRate0( shader_program, 0.5 );
  glUseProgram( 0 );
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

  glutDisplayFunc( display );
  glutVisibilityFunc( rkglVisFuncGLUT );
  glutReshapeFunc( rkglReshapeFuncGLUT );
  glutKeyboardFunc( rkglKeyFuncGLUT );
  glutSpecialFunc( rkglSpecialFuncGLUT );
  glutMouseFunc( rkglMouseFuncGLUT );
  glutMotionFunc( rkglMouseDragFuncGLUT );
  init();
  glutMainLoop();
  return 0;
}
