/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_reflection - programmable shaders with GLSL : reflection mapping.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_REFLECTION_H__
#define __RKGL_GLSL_REFLECTION_H__

#define RKGL_SHADER_VERT_REFLECTION "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 reflect_val;\
uniform mat4 ViewTranspose;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  view = normalize( -pos );\
  vec3 reflect0 = reflect( -view, norm );\
  reflect_val = ViewTranspose * vec4( reflect0, 0.0 );\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_REFLECTION "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 view;\
varying vec4 reflect_val;\
uniform samplerCube sampler;\
uniform float reflectivity;\
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
  gl_FragColor.rgb = ambient.rgb + mix( diffuse.rgb, textureCube( sampler, reflect_val.stp ).rgb, reflectivity ) + specular.rgb;\
  gl_FragColor.a = ambient.a + diffuse.a + specular.a;\
}"

#define rkglShaderCreateReflection() rkglShaderCreate( RKGL_SHADER_VERT_REFLECTION, RKGL_SHADER_FRAG_REFLECTION )

#endif /* __RKGL_GLSL_REFLECTION_H__ */
