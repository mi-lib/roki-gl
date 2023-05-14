/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_texture - programmable shaders with GLSL : texture mapping.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_TEXTURE_H__
#define __RKGL_GLSL_TEXTURE_H__

#define RKGL_SHADER_VERT_TEXTURE "\
varying vec3 pos;\
varying vec3 norm;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal ).xyz;\
  gl_TexCoord[0] = gl_MultiTexCoord0;\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_TEXTURE "\
varying vec3 pos;\
varying vec3 norm;\
uniform sampler2D sampler;\
uniform float mix_rate;\
void main(void)\
{\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  float attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                            + gl_LightSource[0].linearAttenuation * d\
                            + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 ambient = gl_FrontLightProduct[0].ambient;\
  float dotNL = dot( norm, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( 0.0, dotNL );\
  vec3 view = normalize( -pos );\
  vec3 hv = normalize( lv +  view );\
  float powNH = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( dotNL <= 0.0 ) powNH = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * powNH;\
  vec4 tex_color = texture2D( sampler, gl_TexCoord[0].st );\
  gl_FragColor = ( ambient +  mix( diffuse, tex_color, mix_rate ) + specular ) * attenuation;\
}"

#define rkglShaderSetTexture(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler" ), i )

#define rkglShaderSetTextureMixRate(shader,r) glUniform1f( glGetUniformLocation( shader, "mix_rate" ), r )

#define RKGL_SHADER_DEFAULT_MIX_RATE 1.0
__EXPORT GLuint rkglShaderCreateTexture(void);

/* multitexturing */

#define RKGL_SHADER_FRAG_TEXTURE2 "\
varying vec3 pos;\
varying vec3 norm;\
uniform sampler2D sampler0;\
uniform sampler2D sampler1;\
uniform float mix_factor;\
uniform float mix_rate;\
void main(void)\
{\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  float attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                            + gl_LightSource[0].linearAttenuation * d\
                            + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 ambient = gl_FrontLightProduct[0].ambient;\
  float dotNL = dot( norm, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( 0.0, dotNL );\
  vec3 view = normalize( -pos );\
  vec3 hv = normalize( lv +  view );\
  float powNH = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( dotNL <= 0.0 ) powNH = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * powNH;\
  vec4 tex_color0 = texture2D( sampler0, gl_TexCoord[0].st );\
  vec4 tex_color1 = texture2D( sampler1, gl_TexCoord[0].st );\
  vec4 tex_color = mix( tex_color0, tex_color1, mix_factor );\
  gl_FragColor = ( ambient +  mix( diffuse, tex_color, mix_rate ) + specular ) * attenuation;\
}"

#define rkglShaderSetTexture0(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler0" ), i )
#define rkglShaderSetTexture1(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler1" ), i )

#define rkglShaderSetTextureMixFactor(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_factor" ), f )

#define rkglShaderSetTextureMixRate(shader,r) glUniform1f( glGetUniformLocation( shader, "mix_rate" ), r )

#define RKGL_SHADER_DEFAULT_MIX_FACTOR 0.5
#define RKGL_SHADER_DEFAULT_MIX_RATE   1.0
__EXPORT GLuint rkglShaderCreateTexture2(void);

#endif /* __RKGL_GLSL_TEXTURE_H__ */
