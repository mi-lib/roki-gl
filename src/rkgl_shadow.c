/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shadow - shadow map
 */

#define GL_GLEXT_PROTOTYPES
#include <roki-gl/rkgl_shadow.h>

void rkglShadowInit(rkglShadow *shadow, int width, int height, double radius, double ratio)
{
  const GLdouble genfunc[][4] = {
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 },
  };

  shadow->width = width;
  shadow->height = height;
  shadow->radius = radius;
  shadow->ratio = ratio;
  rkglVVInit();
  rkglCAInit();

  glGenTextures( 1, &shadow->texid );
  glBindTexture( GL_TEXTURE_2D, shadow->texid );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
  glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA );
  glAlphaFunc( GL_GEQUAL, 0.5f );

  glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );

  glTexGendv( GL_S, GL_EYE_PLANE, genfunc[0] );
  glTexGendv( GL_T, GL_EYE_PLANE, genfunc[1] );
  glTexGendv( GL_R, GL_EYE_PLANE, genfunc[2] );
  glTexGendv( GL_Q, GL_EYE_PLANE, genfunc[3] );

  glBindTexture( GL_TEXTURE_2D, 0 );

  /* initialize framebuffer */
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

static void _rkglShadowSetLight(rkglShadow *shadow, rkglLight *light)
{
  double d, upx, upz;

  rkglVVInit();
  rkglCAInit();
  d = sqrt( zSqr(light->pos[0]) + zSqr(light->pos[1]) + zSqr(light->pos[2]) );
  gluPerspective( 2*zRad2Deg( asin(shadow->radius/d) ),
    (GLdouble)shadow->width/(GLdouble)shadow->height,
    d > shadow->radius ? d-shadow->radius : d*0.9, d+shadow->radius );
  if( zIsTiny( light->pos[0] ) && zIsTiny( light->pos[1] ) ){
    upx = 1.0;
    upz = 0.0;
  } else{
    upx = 0.0;
    upz = 1.0;
  }
  gluLookAt( light->pos[0], light->pos[1], light->pos[2], 0.0, 0.0, 0.0, upx, 0.0, upz );
  glGetDoublev( GL_MODELVIEW_MATRIX, shadow->_lightview );
}

static void _rkglShadowMap(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, shadow->fb );

  glDisable( GL_SCISSOR_TEST );
  glClear( GL_DEPTH_BUFFER_BIT );
  glViewport( 0, 0, shadow->width, shadow->height );
  _rkglShadowSetLight( shadow, light );

  glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  glDisable( GL_LIGHTING );
  glCullFace( GL_FRONT );
  if( shadow->antizfighting ){
    glEnable( GL_POLYGON_OFFSET_FILL );
    rkglAntiZFighting();
    scene();
    glDisable( GL_POLYGON_OFFSET_FILL );
  } else{
    scene();
  }
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  /* reset projection and camera angle */
  glEnable( GL_SCISSOR_TEST );
  rkglVPLoad( cam );
  rkglVVLoad( cam );
  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  glEnable( GL_LIGHTING );
  glCullFace( GL_BACK );
}

static void _rkglShadowPut(rkglShadow *shadow, rkglCamera *cam, rkglLight *light, void (* scene)(void))
{
  GLfloat dim[4], blk[4];

  rkglClear();
  rkglCALoad( cam );
  rkglLightPut( light );
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

static void _rkglShadowEnable(void)
{
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_TEXTURE_GEN_S );
  glEnable( GL_TEXTURE_GEN_T );
  glEnable( GL_TEXTURE_GEN_R );
  glEnable( GL_TEXTURE_GEN_Q );

  glEnable( GL_ALPHA_TEST );
  glDepthFunc( GL_LEQUAL );
}

static void _rkglShadowDisable(void)
{
  glDepthFunc( GL_LESS );
  glDisable( GL_ALPHA_TEST );
  glDisable( GL_TEXTURE_GEN_S );
  glDisable( GL_TEXTURE_GEN_T );
  glDisable( GL_TEXTURE_GEN_R );
  glDisable( GL_TEXTURE_GEN_Q );
  glDisable( GL_TEXTURE_2D );
}

static void _rkglShadowSunnyside(rkglShadow *shadow, rkglCamera* cam, rkglLight *light, void (* scene)(void))
{
  glMatrixMode( GL_TEXTURE );
  glLoadIdentity();
  glTranslated( 0.5, 0.5, 0.5 );
  glScaled( 0.5, 0.5, 0.5 );
  glMultMatrixd( shadow->_lightview );
  rkglMultInvMatrixd( cam->ca );

  glMatrixMode( GL_MODELVIEW );
  rkglLightLoad( light );

  glBindTexture( GL_TEXTURE_2D, shadow->texid );
  _rkglShadowEnable();
  scene();
  _rkglShadowDisable();
  glBindTexture( GL_TEXTURE_2D, 0 );
}

void rkglShadowDraw(rkglShadow *shadow, rkglCamera* cam, rkglLight *light, void (* scene)(void))
{
  /* create shadow texture */
  _rkglShadowMap( shadow, cam, light, scene );
  /* draw base image as shadow */
  _rkglShadowPut( shadow, cam, light, scene );
  /* draw sunnyside */
  _rkglShadowSunnyside( shadow, cam, light, scene );
}
