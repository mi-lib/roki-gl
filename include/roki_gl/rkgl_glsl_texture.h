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
  gl_TexCoord[1] = gl_MultiTexCoord1;\
  gl_TexCoord[2] = gl_MultiTexCoord2;\
  gl_TexCoord[3] = gl_MultiTexCoord3;\
  gl_TexCoord[4] = gl_MultiTexCoord4;\
  gl_TexCoord[5] = gl_MultiTexCoord5;\
  gl_TexCoord[6] = gl_MultiTexCoord6;\
  gl_TexCoord[7] = gl_MultiTexCoord7;\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_TEXTURE "\
varying vec3 pos;\
varying vec3 norm;\
uniform int num_sampler;\
uniform sampler2D sampler[8];\
uniform float mix_rate[8];\
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
  if( num_sampler > 0 ) diffuse = mix( diffuse, texture2D( sampler[0], gl_TexCoord[0].st ), mix_rate[0] );\
  if( num_sampler > 1 ) diffuse = mix( diffuse, texture2D( sampler[1], gl_TexCoord[1].st ), mix_rate[1] );\
  if( num_sampler > 2 ) diffuse = mix( diffuse, texture2D( sampler[2], gl_TexCoord[2].st ), mix_rate[2] );\
  if( num_sampler > 3 ) diffuse = mix( diffuse, texture2D( sampler[3], gl_TexCoord[3].st ), mix_rate[3] );\
  if( num_sampler > 4 ) diffuse = mix( diffuse, texture2D( sampler[4], gl_TexCoord[4].st ), mix_rate[4] );\
  if( num_sampler > 5 ) diffuse = mix( diffuse, texture2D( sampler[5], gl_TexCoord[5].st ), mix_rate[5] );\
  if( num_sampler > 6 ) diffuse = mix( diffuse, texture2D( sampler[6], gl_TexCoord[6].st ), mix_rate[6] );\
  if( num_sampler > 7 ) diffuse = mix( diffuse, texture2D( sampler[7], gl_TexCoord[7].st ), mix_rate[7] );\
  gl_FragColor = ( ambient + diffuse + specular ) * attenuation;\
}"

#define rkglShaderSetTextureNum(shader,n) glUniform1i( glGetUniformLocation( shader, "num_sampler" ), n )

#define rkglShaderSetTexture0(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[0]" ), i )
#define rkglShaderSetTexture1(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[1]" ), i )
#define rkglShaderSetTexture2(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[2]" ), i )
#define rkglShaderSetTexture3(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[3]" ), i )
#define rkglShaderSetTexture4(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[4]" ), i )
#define rkglShaderSetTexture5(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[5]" ), i )
#define rkglShaderSetTexture6(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[6]" ), i )
#define rkglShaderSetTexture7(shader,i) glUniform1i( glGetUniformLocation( shader, "sampler[7]" ), i )

#define rkglShaderSetTextureMixRate0(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[0]" ), f )
#define rkglShaderSetTextureMixRate1(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[1]" ), f )
#define rkglShaderSetTextureMixRate2(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[2]" ), f )
#define rkglShaderSetTextureMixRate3(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[3]" ), f )
#define rkglShaderSetTextureMixRate4(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[4]" ), f )
#define rkglShaderSetTextureMixRate5(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[5]" ), f )
#define rkglShaderSetTextureMixRate6(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[6]" ), f )
#define rkglShaderSetTextureMixRate7(shader,f) glUniform1f( glGetUniformLocation( shader, "mix_rate[7]" ), f )

#define RKGL_SHADER_DEFAULT_MIX_RATE 0.5

__ROKI_GL_EXPORT GLuint rkglShaderCreateTexture(void);

#endif /* __RKGL_GLSL_TEXTURE_H__ */
