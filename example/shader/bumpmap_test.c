#include <roki_gl/roki_glut.h>
#include <zx11/zximage.h>

rkglCamera cam;
rkglLight light;

zTexture tex[2];

GLint shader_program;

bool make_check_texture(zTexture *texture, int width, int height, int div)
{
  int i, j, dw, dh;
  GLubyte *pt, color;
  ubyte *buf;

  if( !( buf = zAlloc( ubyte, width*height*4 ) ) ) return false;
  dw = ( texture->width  = width  ) / div;
  dh = ( texture->height = height ) / div;
  for( pt=buf, i=0; i<texture->height; i++ ){
    for( j=0; j<texture->width; j++ ){
      color = ( i/dh + j/dw ) % 2 ? 0xff : 0;
      *pt++ = color;
      *pt++ = 0;
      *pt++ = 0xff - color;
      *pt++ = 0xff;
    }
  }
  rkglTextureInit( texture, buf );
  free( buf );
  return true;
}

void square_bump(zVec3D *norm, zVec3D *v1, zVec3D *v2, zVec3D *v3, zVec3D *v4)
{
  zVec3D tan1;

  rkglShaderSetBumpmapTangent( shader_program, zVec3DSub( v2, v1, &tan1 ) );
  glBegin( GL_TRIANGLE_STRIP );
    rkglNormal( norm );
    glTexCoord2f( 0.0, 1.0 ); rkglVertex( v1 );
    glTexCoord2f( 1.0, 1.0 ); rkglVertex( v2 );
    glTexCoord2f( 0.0, 0.0 ); rkglVertex( v4 );
    glTexCoord2f( 1.0, 0.0 ); rkglVertex( v3 );
  glEnd();
}

void draw(void)
{
  zVec3D vert[8] = {
    { { 2, -2, -2 } },
    { { 2,  2, -2 } },
    { { 2,  2,  2 } },
    { { 2, -2,  2 } },
    { {-2,  2, -2 } },
    { {-2, -2, -2 } },
    { {-2, -2,  2 } },
    { {-2,  2,  2 } },
  };
  zVec3D norm[6] = {
    { { 1, 0, 0 } },
    { { 0, 1, 0 } },
    { { 0, 0, 1 } },
    { {-1, 0, 0 } },
    { { 0,-1, 0 } },
    { { 0, 0,-1 } },
  };
  zOpticalInfo oi;

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  rkglMaterial( &oi );
  rkglTextureAssignUnit( &tex[0], 0 );
  rkglTextureAssignUnit( &tex[1], 1 );
  glUseProgram( shader_program );
  rkglShaderSetBumpmapColor( shader_program, 0 );
  rkglShaderSetBumpmapNormal( shader_program, 1 );
  glEnable( GL_TEXTURE_2D );
  square_bump( &norm[0], &vert[0], &vert[1], &vert[2], &vert[3] );
  square_bump( &norm[1], &vert[1], &vert[4], &vert[7], &vert[2] );
  square_bump( &norm[2], &vert[3], &vert[2], &vert[7], &vert[6] );
  square_bump( &norm[3], &vert[4], &vert[5], &vert[6], &vert[7] );
  square_bump( &norm[4], &vert[5], &vert[0], &vert[3], &vert[6] );
  square_bump( &norm[5], &vert[4], &vert[1], &vert[0], &vert[5] );
  glDisable( GL_TEXTURE_2D );
  rkglTextureUnbind();
}

void display(void)
{
  rkglClear();
  rkglCameraPut( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  draw();
  glPopMatrix();
  glutSwapBuffers();
}

void init(double depth)
{
  rkglSetDefaultCamera( &cam, 30.0, 2, 60 );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 12, 0, 5, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 10 );
  rkglLightSetAttenuationConst( &light, 1.0 );

  make_check_texture( &tex[0], 256, 256, 4 );
  tex[1].depth = depth;
  rkglTextureBumpReadFileGLSL( &tex[1], "../fig/bump.bmp" );
  shader_program = rkglShaderCreateBumpmap();

  glEnable( GL_CULL_FACE );
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
  init( argc > 1 ? atof( argv[1] ) : 1.0 );
  glutMainLoop();
  return 0;
}
