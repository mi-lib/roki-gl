/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_envmap - environment mapping (reflection/refraction/shadow).
 */

#ifndef __RKGL_ENVMAP_H__
#define __RKGL_ENVMAP_H__

#include <roki_gl/rkgl_optic.h>
#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_texture.h>
#include <roki_gl/rkgl_shape.h>

__BEGIN_DECLS

/* reflection and refraction mapping */

__ROKI_GL_EXPORT void rkglReflectionRefraction(int width, int height, rkglCamera *cam, rkglLight *light, void (* draw)(void), zVec3D *center);

/* shadow mapping */

typedef struct{
  int width;    /* texture size */
  int height;   /* texture size */
  double ratio; /* darkness ratio */
  double blur;  /* blurring distance of shadow edge */
  double radius; /* radius of bounding sphere */
  bool antizfighting; /* flag to enable anti-Z-fighting */
  /*! @cond */
  GLuint texid; /* texture name */
  GLuint fb; /* framebuffer name */
  GLuint shader_program; /* shader program in GLSL */
  double _lightview[16];
  /*! @endcond */
} rkglShadow;

__ROKI_GL_EXPORT void rkglShadowInit(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur);
__ROKI_GL_EXPORT void rkglShadowDraw(rkglShadow *shadow, rkglCamera* cam, rkglLight *light, void (* scene)(void));

#define rkglShadowEnableAntiZFighting(s)  ( (s)->antizfighting = true )
#define rkglShadowDisableAntiZFighting(s) ( (s)->antizfighting = false )

#ifdef __ROKI_GL_USE_GLEW
/* shadow map with GLSL */
#include <roki_gl/rkgl_glsl.h>

#define RKGL_SHADER_VERT_SHADOW "\
varying vec3 pos;\
varying vec3 norm;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal ).xyz;\
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_ModelViewMatrix * gl_Vertex;\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_SHADOW "\
varying vec3 pos;\
varying vec3 norm;\
varying vec4 shadow;\
uniform sampler2DShadow shadow_map;\
uniform float shadow_ratio;\
uniform float shadow_blur;\
float lookup_shadow_color(float x, float y)\
{\
  return shadow2DProj( shadow_map, gl_TexCoord[0] + vec4( x, y, 0, 0 ) ).r;\
}\
void main (void)\
{\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  float attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                            + gl_LightSource[0].linearAttenuation * d\
                            + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 ambient = gl_LightSource[0].ambient;\
  float cos_norm_lv = dot( norm, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( 0.0, cos_norm_lv );\
  vec3 view = normalize( -pos );\
  vec3 hv = normalize( lv + view );\
  float ref_shininess = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( cos_norm_lv <= 0.0 ) ref_shininess = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * ref_shininess;\
  float s = (       lookup_shadow_color(-shadow_blur,-shadow_blur )\
            + 2.0 * lookup_shadow_color(         0.0,-shadow_blur )\
                  + lookup_shadow_color( shadow_blur,-shadow_blur )\
            + 2.0 * lookup_shadow_color(-shadow_blur,         0.0 )\
            + 4.0 * lookup_shadow_color(         0.0,         0.0 )\
            + 2.0 * lookup_shadow_color( shadow_blur,         0.0 )\
                  + lookup_shadow_color(-shadow_blur, shadow_blur )\
            + 2.0 * lookup_shadow_color(         0.0, shadow_blur )\
                  + lookup_shadow_color( shadow_blur, shadow_blur ) ) / 16.0;\
  diffuse *= clamp( s, shadow_ratio, 1.0 );\
  gl_FragColor.rgb = ( ambient.rgb + diffuse.rgb + specular.rgb ) * attenuation;\
  gl_FragColor.a   =   ambient.a   + diffuse.a   + specular.a;\
}"

#define rkglShaderSetShadowMap( shader, i )   glUniform1i( glGetUniformLocation( shader, "shadow_map" ), i )
#define rkglShaderSetShadowRatio( shader, f ) glUniform1f( glGetUniformLocation( shader, "shadow_ratio" ), f )
#define rkglShaderSetShadowBlur( shader, f )  glUniform1f( glGetUniformLocation( shader, "shadow_blur" ), f )

#define rkglShaderCreateShadow() rkglShaderCreate( RKGL_SHADER_VERT_SHADOW, RKGL_SHADER_FRAG_SHADOW )

__ROKI_GL_EXPORT GLuint rkglShadowInitGLSL(rkglShadow *shadow, int width, int height, double radius, double ratio, double blur);
__ROKI_GL_EXPORT void rkglShadowDrawGLSL(rkglShadow *shadow, rkglCamera* cam, rkglLight *light, void (* scene)(void));

#define rkglShadowDeleteGLSL(s) glDeleteProgram( (s)->shader_program )

#endif /* __ROKI_GL_USE_GLEW */

__END_DECLS

#endif /* __RKGL_ENVMAP_H__ */
