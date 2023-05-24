#include <roki_gl/rkgl_glut.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_glsl.h>

#define RKGL_SHADER_VERT_REFRACTION "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 refract_val;\
uniform float refraction_index;\
uniform mat4 ViewTranspose;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  view = normalize( -pos );\
  vec3 refract0 = refract( -view, norm, 1.0/refraction_index );\
  refract_val = ViewTranspose * vec4( refract0, 0.0 );\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_REFRACTION "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 refract_val;\
uniform samplerCube sampler;\
uniform float transparency;\
void main(void)\
{\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  float attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                            + gl_LightSource[0].linearAttenuation * d\
                            + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 ambient = gl_FrontLightProduct[0].ambient;\
  float cos_norm_lv = dot( norm, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( 0.0, cos_norm_lv );\
  vec3 hv = normalize( lv + view );\
  float ref_shininess = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( cos_norm_lv <= 0.0 ) ref_shininess = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * ref_shininess;\
  gl_FragColor.rgb = ambient.rgb + mix( diffuse.rgb, textureCube( sampler, refract_val.stp ).rgb, transparency ) + specular.rgb;\
  gl_FragColor.a = ambient.a + diffuse.a + specular.a;\
}"

#define rkglShaderCreateRefraction() rkglShaderCreate( RKGL_SHADER_VERT_REFRACTION, RKGL_SHADER_FRAG_REFRACTION )



static GLenum _rkgl_cubemap_id[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

void rkglTextureGenCubeMap(int width, int height)
{
  int i;

  for( i=0; i<6; i++ )
    glTexImage2D( _rkgl_cubemap_id[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void rkglTextureCreateSurround(int width, int height, rkglCamera *cam, rkglLight *light, void (* draw)(void), zVec3D *center)
{
  rkglCamera mirrorview;
  static const zVec3D viewvec[] = {
    { { 1.0, 0.0, 0.0 } },
    { {-1.0, 0.0, 0.0 } },
    { { 0.0, 1.0, 0.0 } },
    { { 0.0,-1.0, 0.0 } },
    { { 0.0, 0.0, 1.0 } },
    { { 0.0, 0.0,-1.0 } },
  };
  static const zVec3D upvec[] = {
    { { 0.0,-1.0, 0.0 } },
    { { 0.0,-1.0, 0.0 } },
    { { 0.0, 0.0, 1.0 } },
    { { 0.0, 0.0,-1.0 } },
    { { 0.0,-1.0, 0.0 } },
    { { 0.0,-1.0, 0.0 } },
  };
  int i;

  rkglBGCopy( cam, &mirrorview );
  for( i=0; i<6; i++ ){
    rkglClear();
    rkglVPCreate( &mirrorview, 0, 0, width, height );
    rkglPerspective( &mirrorview, 90.0, 1.0, 0.1, 20.0 );
    rkglCALookAt( &mirrorview,
      center->c.x, center->c.y, center->c.z,
      center->c.x+viewvec[i].c.x, center->c.y+viewvec[i].c.y, center->c.z+viewvec[i].c.z,
      upvec[i].c.x, upvec[i].c.y, upvec[i].c.z );
    rkglLightPut( light );
    glPushMatrix();
    draw();
    glPopMatrix();
    glCopyTexSubImage2D( _rkgl_cubemap_id[i], 0, 0, 0, 0, 0, width, height );
  }
  rkglVPLoad( cam );
  rkglVVLoad( cam );
}



/* sample test */

int box_id;
int sphere_id;
int room_id;

int fd_id; /* framebuffer */
int rd_id; /* renderbuffer */

bool disp_sphere = true;

rkglCamera cam;
rkglLight light;

#define TEX_WIDTH  256
#define TEX_HEIGHT 256

GLuint shader_program0;
GLuint shader_program1;

zSphere3D sphere;
zBox3D box;

void resize(int w, int h)
{
  rkglVPCreate( &cam, 0, 0, w, h );
  rkglFrustumScale( &cam, 1.0/160, 2, 30 );
}

void draw(void)
{
  glUseProgram( shader_program0 );
  glCallList( room_id );
  glUseProgram( 0 );
}

void display(void)
{
  rkglTextureCreateSurround( TEX_WIDTH, TEX_HEIGHT, &cam, &light, draw, zSphere3DCenter(&sphere) );

  rkglCALoad( &cam );
  rkglLightPut( &light );
  glPushMatrix();
  rkglClear();
  glUseProgram( shader_program1 );
  rkglShaderSetUniform1f( shader_program1, "transparency", 0.8 );
  rkglShaderSetUniform1f( shader_program1, "refraction_index", 1.5 );
  rkglShaderSetUniformMatT( shader_program1, "ViewTranspose", GL_MODELVIEW_MATRIX );
  rkglShaderSetUniform1i( shader_program1, "sampler", 0 );
  if( disp_sphere )
    glCallList( sphere_id );
  else
    glCallList( box_id );
  glUseProgram( 0 );
  draw();
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
  case 'b': disp_sphere = 1 - disp_sphere; break;
  case 'q': case 'Q': case '\033':
    glDeleteProgram( shader_program0 );
    glDeleteProgram( shader_program1 );
    exit( EXIT_SUCCESS );
  default: ;
  }
}

void init(void)
{
  zVec3D c, pc0, pc1, pc2;
  zOpticalInfo oi, oi2;

  rkglSetCallbackParamGLUT( &cam, 0, 0, 0, 0, 0 );

  rkglBGSet( &cam, 0.5, 1.0, 1.0 );
  rkglCASet( &cam, 6, 0, 6, 0, -30, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0, 0, 0 );
  rkglLightMove( &light, 0, 0, 5 );
  rkglLightSetAttenuationLinear( &light, 0.5 );

  room_id = rkglBeginList();
  zOpticalInfoCreateSimple( &oi2, 1.0, 1.0, 1.0, NULL ); /* white */
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
  zOpticalInfoCreateSimple( &oi,  1.0, 1.0, 0.0, NULL ); /* yellow */
  zVec3DCreate( &pc0,  10, -10, -3 );
  zVec3DCreate( &pc1, -10, -10, -3 );
  zVec3DCreate( &pc2,  10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 0.0, 1.0, NULL ); /* blue */
  zVec3DCreate( &pc0,  10,  10, -3 );
  zVec3DCreate( &pc1,  10, -10, -3 );
  zVec3DCreate( &pc2,  10,  10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi,  0.0, 1.0, 0.0, NULL ); /* green */
  zVec3DCreate( &pc0, -10, -10, -3 );
  zVec3DCreate( &pc1, -10,  10, -3 );
  zVec3DCreate( &pc2, -10, -10, 10 );
  rkglCheckerBoard( &pc0, &pc1, &pc2, 10, 10, &oi, &oi2 );
  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 0.0, NULL ); /* ball 1 */
  zOpticalInfoSetShininess( &oi, 3.0 );
  zVec3DCreate( &c, 7.0, 7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c, 7.0, 7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 0.0, 0.0, 1.0, NULL ); /* ball 2 */
  zOpticalInfoSetShininess( &oi, 3.0 );
  zVec3DCreate( &c,-7.0, 7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c,-7.0, 7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 1.0, 0.0, 0.0, NULL ); /* ball 3 */
  zOpticalInfoSetShininess( &oi, 3.0 );
  zVec3DCreate( &c,-7.0,-7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c,-7.0,-7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 0.0, 1.0, 0.0, NULL ); /* ball 4 */
  zOpticalInfoSetShininess( &oi, 3.0 );
  zVec3DCreate( &c, 7.0,-7.0, 7.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  zVec3DCreate( &c, 7.0,-7.0,-2.0 );
  zSphere3DCreate( &sphere, &c, 1.0, 16 );
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();

  zOpticalInfoCreateSimple( &oi, 1.0, 1.0, 1.0, NULL );
  zVec3DCreate( &c, 0, 0, 3 );
  zSphere3DCreate( &sphere, &c, 2.5, 16 );
  sphere_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglSphere( &sphere, RKGL_FACE );
  glEndList();
  zBox3DCreateAlign( &box, &c, 4.0, 4.0, 4.0 );
  box_id = rkglBeginList();
  rkglMaterialOpticalInfo( &oi );
  rkglBox( &box, RKGL_FACE );
  glEndList();

  rkglTextureGenCubeMap( TEX_WIDTH, TEX_HEIGHT );
  shader_program0 = rkglShaderCreatePhong();
  shader_program1 = rkglShaderCreateRefraction();
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
