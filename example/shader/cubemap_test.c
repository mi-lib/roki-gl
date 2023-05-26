#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glsl.h>

/* sample test */

int sphere_id;

int width = 640;
int height = 480;

rkglCamera cam;
rkglLight light;

#define TEX_WIDTH  128
#define TEX_HEIGHT 128

bool make_texture(GLenum target, int width, int height, ulong color)
{
  int i, j;
  GLubyte *buf, *pt;

  if( !( buf = zAlloc( ubyte, width*height*4 ) ) ) return false;
  for( pt=buf, i=0; i<height; i++ )
    for( j=0; j<width; j++, pt+=4 )
      memcpy( pt, &color, 4 );
  glTexImage2D( target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf );
  free( buf );
  return true;
}

void make_cubemap(void)
{
  make_texture( GL_TEXTURE_CUBE_MAP_POSITIVE_X, TEX_WIDTH, TEX_HEIGHT, 0xff0000ff ); /* ( 1, 0, 0) red */
  make_texture( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, TEX_WIDTH, TEX_HEIGHT, 0xffff0000 ); /* (-1, 0, 0) blue */
  make_texture( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, TEX_WIDTH, TEX_HEIGHT, 0xff00ff00 ); /* ( 0, 1, 0) green */
  make_texture( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, TEX_WIDTH, TEX_HEIGHT, 0xffff00ff ); /* ( 0,-1, 0) magenta */
  make_texture( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, TEX_WIDTH, TEX_HEIGHT, 0xffffff00 ); /* ( 0, 0, 1) cyan */
  make_texture( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, TEX_WIDTH, TEX_HEIGHT, 0xff00ffff ); /* ( 0, 0,-1) yellow */

  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

GLuint shader_program;

void resize(int w, int h)
{
  width = w;
  height = h;
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 2, 30 );
}

void display(void)
{
  resize( width, height );
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glUseProgram( shader_program );
  rkglShaderSetUniformMatT( shader_program, "ViewTranspose", GL_MODELVIEW_MATRIX );
  rkglShaderSetUniform1i( shader_program, "sampler", 0 );
  rkglShaderSetUniform1f( shader_program, "reflectivity", 0.5 );

  glPushMatrix();
  rkglClear();
  glCallList( sphere_id );
  glPopMatrix();
  glUseProgram( 0 );
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
    glDeleteProgram( shader_program );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c;
  zOpticalInfo oi;
  zSphere3D sphere;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 4, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 8 );

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &c, 0, 0, 0 );
  zSphere3DCreate( &sphere, &c, 2.5, 16 );
  sphere_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();

  make_cubemap();
  shader_program = rkglShaderCreateReflection();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, ( width = 640 ), ( height = 480 ), argv[0] );

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
