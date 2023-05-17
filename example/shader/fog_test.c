#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glsl.h>

/* sample test */

int sphere_solid_id, sphere_wf_id;
int cylinder_solid_id, cylinder_wf_id;
int cone_solid_id, cone_wf_id;
int checkerboard_id;

rkglCamera cam;
rkglLight light;
rkglLight spotlight;

GLuint shader_program;

ubyte dispswitch = 0;

void display(void)
{
  rkglLightPut( &light );
  rkglLightPut( &spotlight );

  rkglCALoad( &cam );

  glPushMatrix();
  rkglClear();

  glUseProgram( shader_program );
  rkglShaderSetLightNum( shader_program );
  rkglShaderSetFogMode( shader_program );

  glCallList( checkerboard_id );
  glCallList( sphere_solid_id );
  glCallList( cylinder_solid_id );
  glCallList( cone_solid_id );
  glUseProgram( 0 );
  if( dispswitch ){
    glCallList( sphere_wf_id );
    glCallList( cylinder_wf_id );
    glCallList( cone_wf_id );
  }
  glPopMatrix();
  glutSwapBuffers();
}

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 2, 30 );
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
  case 'w': dispswitch = 1 - dispswitch; break;
  case 'q': case 'Q': case '\033':
    glDeleteProgram( shader_program );
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

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 6, 0, 3, 0, -30, 0 );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 10 );
  rkglLightSetAttenuationQuad( &light );

  rkglLightCreate( &spotlight, 0.3, 0.3, 0.3, 0.6, 0.6, 0.6, 0, 0, 0 );
  rkglLightSetSpot( &spotlight, 0.0, 0.0, -3.0, 20.0, 30.0 );
  rkglLightMove( &spotlight, 0, 5, 3 );

  rkglFogExp( 0.5, 0.5, 0.5, 0.1 );

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
  shader_program = rkglShaderCreateFog();
  glutMainLoop();
  return 0;
}
