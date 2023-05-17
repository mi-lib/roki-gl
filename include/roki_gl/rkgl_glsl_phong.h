/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_phong - programmable shaders with GLSL : Phong shading.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_PHONG_H__
#define __RKGL_GLSL_PHONG_H__

#define RKGL_SHADER_VERT_POS_NORM "\
varying vec3 pos;\
varying vec3 norm;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_PHONG "\
varying vec3 pos;\
varying vec3 norm;\
void main(void)\
{\
  float attenuation = 1.0;\
  vec3 lv = gl_LightSource[0].position.xyz - pos;\
  float d = length( lv );\
  attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                      + gl_LightSource[0].linearAttenuation * d\
                      + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 amb = gl_FrontLightProduct[0].ambient;\
  float dotNL = dot( norm, lv );\
  vec4 dif = gl_FrontLightProduct[0].diffuse * max( 0.0, dotNL );\
  vec3 view = normalize( -pos );\
  vec3 hv = normalize( lv + view );\
  float powNH = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( dotNL <= 0.0 ) powNH = 0.0;\
  vec4 spc = gl_FrontLightProduct[0].specular * powNH;\
  gl_FragColor = ( amb + dif + spc ) * attenuation;\
}"

#define rkglShaderCreatePhong() rkglShaderCreate( RKGL_SHADER_VERT_POS_NORM, RKGL_SHADER_FRAG_PHONG )

#endif /* __RKGL_GLSL_PHONG_H__ */
