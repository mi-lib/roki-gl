#include <roki-gl/rkgl_glut.h>
#include <roki-gl/rkgl_camera.h>
#include <roki-gl/rkgl_shape.h>

zOpticalInfo red, cyan, yellow;
zSphere3D sphere[3];
zCyl3D cylinder[3];
zCone3D cone[3];

rkglCamera cam;
rkglLight light;

double r = 0;

void display(void)
{
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  glRotated( r, 0, 1, 0 );
  rkglClear();
  rkglMaterial( &red );
  rkglSphere( &sphere[0] ); rkglSphere( &sphere[1] ); rkglSphere( &sphere[2] );
  rkglMaterial( &cyan );
  rkglCyl( &cylinder[0] );  rkglCyl( &cylinder[1] );  rkglCyl( &cylinder[2] );
  rkglMaterial( &yellow );
  rkglCone( &cone[0] );     rkglCone( &cone[1] );     rkglCone( &cone[2] );
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 2, 10 );
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
  case ' ': r += 10; break;
  case 'q': case 'Q': case '\033':
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c1, c2;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0, 0 );
  rkglLightSetPos( &light, 1, 3, 6 );

  /* course */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1,-2, 0, 0 );
  zSphere3DCreate( &sphere[0], &c1, 1, 8 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1,-2, 2,-1.5 );
  zVec3DCreate( &c2,-2, 1, 1.5 );
  zCyl3DCreate( &cylinder[0], &c1, &c2, 0.5, 8 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1,-2,-2,-2 );
  zVec3DCreate( &c2,-2,-1, 1 );
  zCone3DCreate( &cone[0], &c1, &c2, 1, 8 );

  /* middle */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 0, 0, 0 );
  zSphere3DCreate( &sphere[1], &c1, 1, 16 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 0, 2,-1.5 );
  zVec3DCreate( &c2, 0, 1, 1.5 );
  zCyl3DCreate( &cylinder[1], &c1, &c2, 0.5, 16 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 0,-2,-2 );
  zVec3DCreate( &c2, 0,-1, 1 );
  zCone3DCreate( &cone[1], &c1, &c2, 1, 16 );

  /* fine */
  zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
  zVec3DCreate( &c1, 2, 0, 0 );
  zSphere3DCreate( &sphere[2], &c1, 1, 0 );

  zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
  zVec3DCreate( &c1, 2, 2,-1.5 );
  zVec3DCreate( &c2, 2, 1, 1.5 );
  zCyl3DCreate( &cylinder[2], &c1, &c2, 0.5, 0 );

  zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
  zVec3DCreate( &c1, 2,-2,-2 );
  zVec3DCreate( &c2, 2,-1, 1 );
  zCone3DCreate( &cone[2], &c1, &c2, 1, 0 );
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
