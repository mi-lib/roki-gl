/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_fresnel - programmable shaders with GLSL : reflection and refraction mapping.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_FRESNEL_H__
#define __RKGL_GLSL_FRESNEL_H__

#define RKGL_SHADER_VERT_FRESNEL "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 reflect_val;\
varying vec4 refract_val;\
varying float reflection_index;\
uniform float refraction_index;\
uniform float reflectivity;\
uniform mat4 ViewTranspose;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  view = normalize( -pos );\
  refract_val = ViewTranspose * vec4( refract( -view, norm, 1.0/refraction_index ), 0.0 );\
  reflect_val = ViewTranspose * vec4( reflect( -view, norm ), 0.0 );\
  reflection_index = reflectivity + ( 1.0 - reflectivity ) * pow( 1.0 + dot( -view, norm ), 5.0 );\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_FRESNEL "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 reflect_val;\
varying vec4 refract_val;\
varying float reflection_index;\
uniform samplerCube environment_sampler;\
uniform float transparency;\
void main(void)\
{\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  lv = normalize( lv );\
  vec4 ambient = gl_FrontLightProduct[0].ambient;\
  float cos_norm_lv = dot( norm, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( 0.0, cos_norm_lv );\
  vec3 hv = normalize( lv + view );\
  float ref_shininess = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( cos_norm_lv <= 0.0 ) ref_shininess = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * ref_shininess;\
  vec4 color_reflect = textureCube( environment_sampler, reflect_val.stp );\
  vec4 color_refract = textureCube( environment_sampler, refract_val.stp );\
  gl_FragColor.rgb = ambient.rgb + mix( mix( diffuse, color_refract, transparency ), color_reflect, reflection_index ).rgb + specular.rgb;\
  gl_FragColor.a = 1.0;\
}"

#define rkglShaderSetFresnelTransparency(s,r)       rkglShaderSetUniform1f( s, "transparency", r )
#define rkglShaderSetFresnelReflectivity(s,r)       rkglShaderSetUniform1f( s, "reflectivity", r )
#define rkglShaderSetFresnelRefractionIndex(s,n)    rkglShaderSetUniform1f( s, "refraction_index", n )
#define rkglShaderSetFresnelEnvironmentSampler(s,i) rkglShaderSetUniform1i( s, "environment_sampler", i )

#define rkglShaderSetViewMatTranspose(s) rkglShaderSetUniformMatT( s, "ViewTranspose", GL_MODELVIEW_MATRIX )

#define rkglShaderCreateFresnel() rkglShaderCreate( RKGL_SHADER_VERT_FRESNEL, RKGL_SHADER_FRAG_FRESNEL )

#endif /* __RKGL_GLSL_FRESNEL_H__ */
