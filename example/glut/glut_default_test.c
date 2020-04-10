#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

zOpticalInfo red, cyan, yellow;
zSphere3D sphere;
zCyl3D cylinder;
zCone3D cone;

rkglCamera cam;
rkglLight light;

void display(void)
{
  rkglClear();
  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
    rkglMaterial( &red );    rkglSphere( &sphere, RKGL_FACE );
    rkglMaterial( &cyan );   rkglCyl( &cylinder, RKGL_FACE );
    rkglMaterial( &yellow ); rkglCone( &cone, RKGL_FACE );
  glPopMatrix();
  glutSwapBuffers();
}

void init(void)
{
  zVec3D c1, c2;

  rkglSetCallbackParamGLUT( &cam, 2.0, 2, 60, 1.0, 5.0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 5, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 1, 3, 6 );

  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 0, 0, 0 );
  zSphere3DCreate( &sphere, &c1, 1, 0 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 0, 2,-1.5 );
  zVec3DCreate( &c2, 0, 1, 1.5 );
  zCyl3DCreate( &cylinder, &c1, &c2, 0.5, 0 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 0,-2,-2 );
  zVec3DCreate( &c2, 0,-1, 1 );
  zCone3DCreate( &cone, &c1, &c2, 1, 0 );
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
