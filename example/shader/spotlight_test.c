#include <roki_gl/roki_glut.h>

int sphere_solid_id, sphere_wf_id;
int cylinder_solid_id, cylinder_wf_id;
int cone_solid_id, cone_wf_id;
int checkerboard_id;

rkglCamera cam;
rkglLight light;
rkglLight spotlight[3];

GLuint shader_program;

ubyte dispswitch = 0;

void display(void)
{
  rkglLightPut( &light );
  rkglLightPut( &spotlight[0] );
  rkglLightPut( &spotlight[1] );
  rkglLightPut( &spotlight[2] );

  rkglCameraPut( &cam );

  glPushMatrix();
  rkglClear();

  glUseProgram( shader_program );
  rkglShaderSetLightNum( shader_program );

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

void keyboard(unsigned char key, int x, int y)
{
  switch( key ){
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

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraLookAt( &cam, 15, 0, 5, 0, 0, 0, 0, 0, 1 );
  rkglCameraFitPerspective( &cam, 30, 1, 100 );
  rkglSetDefaultCamera( &cam );
  glLineWidth( 2 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.2, 0.2, 0.2, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 10 );
  rkglLightSetAttenuationQuad( &light, 1.0 );

  rkglLightCreate( &spotlight[0], 0.3, 0.0, 0.0, 0.6, 0.0, 0.0, 0, 0, 0 );
  rkglLightSetSpot( &spotlight[0], 0.0, -5.0, -3.0, 20.0, 30.0 );
  rkglLightMove( &spotlight[0], 0, 5, 3 );

  rkglLightCreate( &spotlight[1], 0.0, 0.3, 0.0, 0.0, 0.6, 0.0, 0, 0, 0 );
  rkglLightSetSpot( &spotlight[1], 0.0, 0.0, -3.0, 20.0, 30.0 );
  rkglLightMove( &spotlight[1], 0, 0, 3 );

  rkglLightCreate( &spotlight[2], 0.0, 0.0, 0.3, 0.0, 0.0, 0.6, 0, 0, 0 );
  rkglLightSetSpot( &spotlight[2], 0.0, 5.0, -3.0, 20.0, 30.0 );
  rkglLightMove( &spotlight[2], 0,-5, 3 );

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

  shader_program = rkglShaderCreateSpotlight();
}

int main(int argc, char *argv[])
{
  rkglInitGLUT( &argc, argv );
  rkglWindowCreateGLUT( 0, 0, 640, 480, argv[0] );

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
