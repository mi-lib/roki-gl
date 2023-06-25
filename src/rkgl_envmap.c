/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_envmap - environment mapping (reflection/refraction/shadow).
 */

#include <roki_gl/rkgl_envmap.h>

/* reflection and refraction mapping */

void rkglReflectionRefraction(int width, int height, rkglCamera *cam, rkglLight *light, void (* draw)(void), zVec3D *center)
{
  rkglCamera view;
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

  rkglBGCopy( cam, &view );
  for( i=0; i<6; i++ ){
    rkglClear();
    rkglVPCreate( &view, 0, 0, width, height );
    rkglPerspective( &view, 90.0, 1.0, 0.1, 20.0 );
    rkglCALookAt( &view,
      center->c.x, center->c.y, center->c.z,
      center->c.x+viewvec[i].c.x, center->c.y+viewvec[i].c.y, center->c.z+viewvec[i].c.z,
      upvec[i].c.x, upvec[i].c.y, upvec[i].c.z );
    rkglLightPut( light );
    glPushMatrix();
    draw();
    glPopMatrix();
    glCopyTexSubImage2D( rkgl_cubemap_id[i], 0, 0, 0, 0, 0, width, height );
  }
  rkglVPLoad( cam );
  rkglVVLoad( cam );
}

/* shadow mapping */

static void _rkglShadowInit(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur)
{
  shadow->width = width;
  shadow->height = height;
  shadow->radius = radius;
  shadow->ratio = ratio;
  shadow->blur = blur; /* dummy */
  rkglVVInit();
  rkglCAInit();

  /* assign texture for shadow map. */
  glGenTextures( 1, &shadow->texid );
  glBindTexture( GL_TEXTURE_2D, shadow->texid );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );
  rkglTextureSetClamp();
  rkglTextureSetFilterLinear();

  /* use framebuffer for high-resolution texture. */
  glGenFramebuffersEXT( 1, &shadow->fb );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, shadow->fb );
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadow->texid, 0 );
  glDrawBuffer( GL_NONE );
  glReadBuffer( GL_NONE );
  if( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ) != GL_FRAMEBUFFER_COMPLETE_EXT )
    ZRUNWARN( "the current framebuffer status is unsupported" );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  rkglShadowEnableAntiZFighting( shadow );
}

GLuint rkglShadowInit(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur)
{
  _rkglShadowInit( shadow, width, height, radius, ratio, blur );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
  glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA );
  glAlphaFunc( GL_GEQUAL, 0.5f );

  glBindTexture( GL_TEXTURE_2D, shadow->texid );
  rkglTextureGenProjectionEye();
  glBindTexture( GL_TEXTURE_2D, 0 );
  return ( shadow->shader_program = 0 );
}

static void _rkglShadowSetLight(rkglShadow *shadow, rkglLight *light)
{
  double d;

  d = sqrt( zSqr(light->pos[0]) + zSqr(light->pos[1]) + zSqr(light->pos[2]) );
  gluPerspective( 2*zRad2Deg( atan2( shadow->radius, d ) ),
    (GLdouble)shadow->width/shadow->height, d*0.1, d*10 + shadow->radius );
  gluLookAt( light->pos[0], light->pos[1], light->pos[2], 0.0, 0.0, 0.0,
    light->pos[1] - light->pos[2], light->pos[2] - light->pos[0], light->pos[0] - light->pos[1] );
  glGetDoublev( GL_MODELVIEW_MATRIX, shadow->_lightview );
}

static void _rkglShadowMap(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, shadow->fb );

  glDisable( GL_SCISSOR_TEST );
  glClear( GL_DEPTH_BUFFER_BIT );
  glViewport( 0, 0, shadow->width, shadow->height );
  rkglVVInit();
  rkglCAInit();
  _rkglShadowSetLight( shadow, light );

  glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  glDisable( GL_LIGHTING );
  glCullFace( GL_FRONT );
  if( shadow->antizfighting ){
    glEnable( GL_POLYGON_OFFSET_FILL );
    rkglTextureOffsetShadow();
    scene();
    glDisable( GL_POLYGON_OFFSET_FILL );
  } else{
    scene();
  }
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

static void _rkglShadowResetProjection(rkglShadow *shadow, rkglCamera *cam, rkglLight *light)
{
  glEnable( GL_SCISSOR_TEST );
  rkglVPLoad( cam );
  rkglVVLoad( cam );
  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  glEnable( GL_LIGHTING );
  glCullFace( GL_BACK );

  rkglClear();
  rkglCALoad( cam );
  rkglLightPut( light );
}

static void _rkglShadowPut(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  GLfloat dim[4], blk[4];

  _rkglShadowResetProjection( shadow, cam, light );
  /* shadowy lighting */
  dim[0] = light->dif[0] * shadow->ratio;
  dim[1] = light->dif[1] * shadow->ratio;
  dim[2] = light->dif[2] * shadow->ratio;
  dim[3] = 1.0;
  blk[0] = light->spc[0] * shadow->ratio;
  blk[1] = light->spc[1] * shadow->ratio;
  blk[2] = light->spc[2] * shadow->ratio;
  blk[3] = 1.0;
  glLightfv( light->id, GL_DIFFUSE, dim );
  glLightfv( light->id, GL_SPECULAR, blk );
  scene();
}

static void _rkglShadowXformMap(rkglShadow *shadow, rkglCamera *cam)
{
  glMatrixMode( GL_TEXTURE );
  glLoadIdentity();
  glTranslated( 0.5, 0.5, 0.5 );
  glScaled( 0.5, 0.5, 0.5 );
  glMultMatrixd( shadow->_lightview );
  rkglMultInvMatrixd( cam->ca );
}

static void _rkglShadowSunnyside(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  _rkglShadowXformMap( shadow, cam );
  glMatrixMode( GL_MODELVIEW );
  rkglLightLoad( light );

  glBindTexture( GL_TEXTURE_2D, shadow->texid );
  rkglTextureEnableProjection();
  glEnable( GL_ALPHA_TEST );
  glDepthFunc( GL_LEQUAL );
  scene();
  glDepthFunc( GL_LESS );
  glDisable( GL_ALPHA_TEST );
  rkglTextureDisableProjection();
  glBindTexture( GL_TEXTURE_2D, 0 );
}

void rkglShadowDraw(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  /* create shadow texture */
  _rkglShadowMap( shadow, cam, light, scene );
  /* draw base image as shadow */
  _rkglShadowPut( shadow, cam, light, scene );
  /* draw sunnyside */
  _rkglShadowSunnyside( shadow, cam, light, scene );
}

#ifdef __ROKI_GL_USE_GLEW
/* shadow map using GLSL */

GLuint rkglShadowInitGLSL(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur)
{
  _rkglShadowInit( shadow, width, height, radius, ratio, blur );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
  glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );

  return ( shadow->shader_program = rkglShaderCreateShadow() );
}

static void _rkglShadowPutGLSL(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  _rkglShadowResetProjection( shadow, cam, light );
  _rkglShadowXformMap( shadow, cam );

  glMatrixMode( GL_MODELVIEW );
  glEnable( GL_TEXTURE_2D );
  glUseProgram( shadow->shader_program );
  rkglShaderSetShadowMap( shadow->shader_program, 0 );
  rkglShaderSetShadowRatio( shadow->shader_program, shadow->ratio );
  rkglShaderSetShadowBlur( shadow->shader_program, shadow->blur );
  scene();
  glDisable( GL_TEXTURE_2D );
  glUseProgram( 0 );
}

void rkglShadowDrawGLSL(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  /* create shadow texture */
  _rkglShadowMap( shadow, cam, light, scene );
  /* draw image with shadow */
  _rkglShadowPutGLSL( shadow, cam, light, scene );
}

#endif /* __ROKI_GL_USE_GLEW */
