#include <roki-gl/rkgl_texture.h>
#include <roki-gl/rkgl_glut.h>

zOpticalInfo red;
zBox3D box;

rkglCamera cam;
rkglLight light;

rkglTexture texture1, texture2;

zVec3D vert1[] = {
  { { 1.0,-2.0,-2.0 } },
  { { 1.0, 2.0,-2.0 } },
  { { 1.0, 2.0, 2.0 } },
  { { 1.0,-2.0, 2.0 } },
};
zVec3D vert2[] = {
  { {-1.0, 2.0,-2.0 } },
  { {-1.0,-2.0,-2.0 } },
  { {-1.0,-2.0, 2.0 } },
  { {-1.0, 2.0, 2.0 } },
};

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
    rkglMaterial( &red );
    rkglBox( &box, RKGL_FACE );
    rkglTextureDraw( &texture1 );
    rkglTextureDraw( &texture2 );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  rkglSetCallbackParamGLUT( &cam, 2.0, 2, 60, 1.0, 5.0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 5, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 10, 3, 10 );

  zOpticalInfoCreate( &red, 0.3, 0.3, 0.3, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.5, 1.0, NULL );
  zBox3DCreateAlign( &box, ZVEC3DZERO, 2.0, 4.0, 4.0 );

  zxInit();
  rkglTextureCreate( &texture1, "lena_mini.jpg", vert1 );
  rkglTextureCreate( &texture2, "lena_flop_mini.jpg", vert2 );
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
