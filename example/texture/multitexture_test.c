#include <roki-gl/rkgl_shape.h>
#include <roki-gl/rkgl_glut.h>
#include <zx11/zximage.h>

rkglCamera cam;
rkglLight light;

zTexture tex[2];

bool make_check_texture(zTexture *texture, int width, int height, int div)
{
  int i, j, dw, dh;
  GLubyte *pt, color;

  if( !( texture->buf = zAlloc( ubyte, width*height*3 ) ) ) return false;
  dw = ( texture->width  = width  ) / div;
  dh = ( texture->height = height ) / div;
  for( pt=texture->buf, i=0; i<texture->height; i++ ){
    for( j=0; j<texture->width; j++ ){
      color = ( i/dh + j/dw ) % 2 ? 0xff : 0;
      *pt++ = color;
      *pt++ = 0;
      *pt++ = 0xff - color;
    }
  }
  rkglTextureInit( texture );
  zFree( texture->buf );
  return true;
}

#define rkglMultiTexCoord(s,t) do{\
  glMultiTexCoord2f( GL_TEXTURE0, s, t );\
  glMultiTexCoord2f( GL_TEXTURE1, s, t );\
} while(0)

void square(GLfloat norm[3], GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat v4[3])
{
  glBegin( GL_QUADS );
    glNormal3fv( norm );
    rkglMultiTexCoord( 0.0, 1.0 ); glVertex3fv( v1 );
    rkglMultiTexCoord( 1.0, 1.0 ); glVertex3fv( v2 );
    rkglMultiTexCoord( 1.0, 0.0 ); glVertex3fv( v3 );
    rkglMultiTexCoord( 0.0, 0.0 ); glVertex3fv( v4 );
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
  glActiveTexture( GL_TEXTURE0 );
  glEnable( GL_TEXTURE_2D );
  rkglTextureBind( &tex[0] );
  rkglTextureSetModulate();
  glActiveTexture( GL_TEXTURE1 );
  glEnable( GL_TEXTURE_2D );
  rkglTextureBind( &tex[1] );
  rkglTextureSetModulate();
  square( norm[0], vert[0], vert[1], vert[2], vert[3] );
  square( norm[1], vert[2], vert[1], vert[4], vert[7] );
  square( norm[2], vert[3], vert[2], vert[7], vert[6] );
  square( norm[3], vert[4], vert[5], vert[6], vert[7] );
  square( norm[4], vert[0], vert[3], vert[6], vert[5] );
  square( norm[5], vert[1], vert[0], vert[5], vert[4] );
  glActiveTexture( GL_TEXTURE0 );
  glDisable( GL_TEXTURE_2D );
  glActiveTexture( GL_TEXTURE1 );
  glDisable( GL_TEXTURE_2D );
  rkglTextureUnbind();
}

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  draw();
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
  rkglLightSetAttenuationQuad( &light );

  make_check_texture( &tex[0], 256, 256, 4 );
  rkglTextureReadFile( &tex[1], "lena_mini.jpg" );
/*
  rkglTextureSetDecal();
*/
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
  rkglInitGLEW();
  init();
  glutMainLoop();
  return 0;
}
