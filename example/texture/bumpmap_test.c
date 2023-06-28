#include <roki_gl/roki_glut.h>
#include <zx11/zximage.h>

rkglCamera cam;
rkglLight light;

zTexture tex[2];

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
      *pt++ = 0x00;
      *pt++ = 0xff - color;
      *pt++ = 0xff;
    }
  }
  rkglTextureInit( texture, buf );
  free( buf );
  return true;
}

void square_tex(zVec3D *norm, zVec3D *v1, zVec3D *v2, zVec3D *v3, zVec3D *v4)
{
  glBegin( GL_QUADS );
    rkglNormal( norm );
    glTexCoord2f( 0.0, 1.0 ); rkglVertex( v1 );
    glTexCoord2f( 1.0, 1.0 ); rkglVertex( v2 );
    glTexCoord2f( 1.0, 0.0 ); rkglVertex( v3 );
    glTexCoord2f( 0.0, 0.0 ); rkglVertex( v4 );
  glEnd();
}

void square_bump(zVec3D *norm, zVec3D *v1, zVec3D *v2, zVec3D *v3, zVec3D *v4)
{
  GLdouble m[16];
  zVec3D lp_world, lp, lv;

  glGetDoublev( GL_MODELVIEW_MATRIX, m );
  _zVec3DCreate( &lp_world, light.pos[0], light.pos[1], light.pos[2] );
  rkglXformInvd( m, lp_world.e, lp.e );
  glBegin( GL_QUADS );
    rkglNormal( norm );
    glTexCoord2f( 0.0, 1.0 );
    _zVec3DSub( &lp, v1, &lv ); glMultiTexCoord3d( GL_TEXTURE1, lv.c.x, lv.c.y, lv.c.z ); rkglVertex( v1 );
    glTexCoord2f( 1.0, 1.0 );
    _zVec3DSub( &lp, v2, &lv ); glMultiTexCoord3d( GL_TEXTURE1, lv.c.x, lv.c.y, lv.c.z ); rkglVertex( v2 );
    glTexCoord2f( 1.0, 0.0 );
    _zVec3DSub( &lp, v3, &lv ); glMultiTexCoord3d( GL_TEXTURE1, lv.c.x, lv.c.y, lv.c.z ); rkglVertex( v3 );
    glTexCoord2f( 0.0, 0.0 );
    _zVec3DSub( &lp, v4, &lv ); glMultiTexCoord3d( GL_TEXTURE1, lv.c.x, lv.c.y, lv.c.z ); rkglVertex( v4 );
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

  glEnable( GL_TEXTURE_2D );
  glActiveTexture( GL_TEXTURE0 );
  rkglTextureBind( &tex[0] );
  square_tex( &norm[0], &vert[0], &vert[1], &vert[2], &vert[3] );
  square_tex( &norm[1], &vert[2], &vert[1], &vert[4], &vert[7] );
  square_tex( &norm[2], &vert[3], &vert[2], &vert[7], &vert[6] );
  square_tex( &norm[3], &vert[4], &vert[5], &vert[6], &vert[7] );
  square_tex( &norm[4], &vert[0], &vert[3], &vert[6], &vert[5] );
  square_tex( &norm[5], &vert[1], &vert[0], &vert[5], &vert[4] );

  glEnable( GL_BLEND );
  glBlendFunc( GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glActiveTexture( GL_TEXTURE0 );
  glEnable( GL_TEXTURE_2D );
  rkglTextureBind( &tex[1] );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, tex[1].id_bump );
  rkglTextureOffsetBump();
  glEnable( GL_TEXTURE_CUBE_MAP );
  square_bump( &norm[0], &vert[0], &vert[1], &vert[2], &vert[3] );
  glDisable( GL_TEXTURE_CUBE_MAP );
  glDisable( GL_POLYGON_OFFSET_FILL );
  glDisable( GL_BLEND );

  glActiveTexture( GL_TEXTURE0 );
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

void init(double depth)
{
  rkglSetCallbackParamGLUT( &cam, 2.0, 2, 60, 1.0, 5.0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 5, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 5 );
  rkglLightSetAttenuationConst( &light, 1.0 );

  make_check_texture( &tex[0], 256, 256, 4 );
  tex[1].depth = depth;
  rkglTextureBumpReadFile( &tex[1], "../fig/bump.bmp" );
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
