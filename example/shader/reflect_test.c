#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glsl.h>

/* sample test */

int sphere_id;
int room_id;

int width = 640;
int height = 480;

rkglCamera cam;
rkglLight light;

#define TEX_WIDTH  128
#define TEX_HEIGHT 128
float reflectivity0 = 0.5;

struct Target{
  GLenum name;
  GLint x, y; /* original point of partial image */
  GLdouble cx, cy, cz; /* viewpoint */
  GLdouble ux, uy, uz; /* up-vector */
};

struct Target target[] = {
  { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    0, TEX_HEIGHT,
    1.0, 0.0, 0.0,
    0.0, 0.0,-1.0 },
  { GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    0, 0,
   -1.0, 0.0, 0.0,
    0.0, 0.0,-1.0 },
  { GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    TEX_WIDTH, TEX_HEIGHT,
    0.0, 1.0, 0.0,
    0.0, 0.0,-1.0 },
  { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    TEX_WIDTH,0,
    0.0,-1.0, 0.0,
    0.0, 0.0,-1.0 },
  { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    2*TEX_WIDTH, TEX_HEIGHT,
    0.0, 0.0, 1.0,
   -1.0, 0.0, 0.0 },
  { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    2*TEX_WIDTH, 0,
    0.0, 0.0,-1.0,
    1.0, 0.0, 0.0 },
};

void rkglTextureGenCube(void)
{
  int i;

  for( i=0; i<6; i++ )
    glTexImage2D( target[i].name, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void rkglTextureCreateCube(rkglLight *light, void (* draw)(void), GLfloat pos[])
{
  int i;

  rkglClear();
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 90.0, 1.0, 0.1, 30.0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  for( i=0; i<6; i++ ){
    glViewport( target[i].x, target[i].y, TEX_WIDTH, TEX_HEIGHT );
    gluLookAt( pos[0], pos[1], pos[2],
      pos[0]+target[i].cx, pos[1]+target[i].cy, pos[2]+target[i].cz,
      target[i].ux, target[i].uy, target[i].uz );
    rkglLightPut( light );
    glPushMatrix();
    draw();
    glPopMatrix();
    glCopyTexSubImage2D( target[i].name, 0, 0, 0, target[i].x, target[i].y, TEX_WIDTH, TEX_HEIGHT );
  }
}



/*
GLuint shader_program0;
*/
GLuint shader_program1;


void resize(int w, int h)
{
  width = w;
  height = h;
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 2, 30 );
}

void draw(void)
{
/*
  glUseProgram( shader_program0 );
*/
  glCallList( room_id );
/*
  glUseProgram( 0 );
*/
}

void display(void)
{
#if 1
  GLfloat pos[] = { 0.0, 0.0, 0.0 };
  rkglTextureCreateCube( &light, draw, pos );
#endif

  glUseProgram( shader_program1 );
  GLint refLoc = glGetUniformLocation( shader_program1, "reflectivity" );
  glUniform1f( refLoc, reflectivity0 );
  float m[16];
  glGetFloatv( GL_MODELVIEW_MATRIX, m );
  GLint ViewTransLoc = glGetUniformLocation( shader_program1, "ViewTranspose" );
  glUniformMatrix4fv( ViewTransLoc, 1, GL_TRUE, m );

  resize( width, height );
  rkglCALoad( &cam );
  rkglLightPut( &light );

  glPushMatrix();
  rkglClear();
  GLint samplerLoc = glGetUniformLocation( shader_program1, "sampler" );
  glUniform1i( samplerLoc, 0 );
  draw();
  glCallList( sphere_id );
  glPopMatrix();
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
/*
    glDeleteProgram( shader_program0 );
*/
    glDeleteProgram( shader_program1 );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c, pc0, pc1, pc2;
  zOpticalInfo oi, oi2;
  zSphere3D sphere;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 1.0, 1.0 );
  rkglCASet( &cam, 6, 0, 6, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 5, 0, 8 );
  rkglLightSetAttenuationConst( &light );

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &c, 0, 0, 0 );
  zSphere3DCreate( &sphere, &c, 2.5, 16 );
  sphere_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();

  zOpticalInfoCreateSimple( &oi2, 1.0, 1.0, 1.0, NULL ); /* white */
  room_id = rkglBeginList();
  zOpticalInfoCreateSimple( &oi,  1.0, 0.0, 0.0, NULL ); /* red */
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2, -10,  10, -3 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 1.0, 1.0, NULL ); /* cyan */
  zVec3DCreate( &pc0, -10,  10, -3 );
  zVec3DCreate( &pc1,  10,  10, -3 );
  zVec3DCreate( &pc2, -10,  10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zVec3DCreate( &pc0,  10, -10, -3 );
  zVec3DCreate( &pc1, -10, -10, -3 );
  zVec3DCreate( &pc2,  10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 0.0, 1.0, NULL ); /* blue */
  zVec3DCreate( &pc0,  10,  10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2,  10,  10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1, -10,  10, -3 );
  zVec3DCreate( &pc2, -10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  glEndList();

/*
  shader_program0 = rkglShaderCreatePhong();
*/
  shader_program1 = rkglShaderCreateFromFile( "cubeReflect2.vert", "cubeReflect2.frag" );
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
