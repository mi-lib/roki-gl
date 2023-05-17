/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_spotlight - programmable shaders with GLSL : Spotlighting.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_SPOTLIGHT_H__
#define __RKGL_GLSL_SPOTLIGHT_H__

#define RKGL_SHADER_FRAG_PHONG_SPOTLIGHT "\
varying vec3 pos;\
varying vec3 norm;\
uniform int num_of_light;\
void main(void)\
{\
  gl_FragColor = vec4( 0.0 );\
  float attenuation = 1.0;\
  for(int i=0; i<num_of_light; i++ ){\
    vec3 lv = gl_LightSource[i].position.xyz - pos;\
    if( gl_LightSource[i].spotCutoff == 180.0 ){\
      float d = length( lv );\
      attenuation = 1.0 / ( gl_LightSource[i].constantAttenuation\
                          + gl_LightSource[i].linearAttenuation * d\
                          + gl_LightSource[i].quadraticAttenuation * d * d );\
    }\
    lv = normalize( lv );\
    if( gl_LightSource[i].spotCutoff <= 90.0 ){\
      float ls = dot( -lv, normalize( gl_LightSource[i].spotDirection ) );\
      attenuation = ( ls < gl_LightSource[i].spotCosCutoff ) ? 0.0 : pow( ls, gl_LightSource[i].spotExponent );\
    }\
    vec4 amb = gl_FrontLightProduct[i].ambient;\
    float dotNL = dot( norm, lv );\
    vec4 dif = gl_FrontLightProduct[i].diffuse * max( 0.0, dotNL );\
    vec3 view = normalize( -pos );\
    vec3 hv = normalize( lv + view );\
    float powNH = pow( max( dot( norm, hv ), 0.0 ), gl_FrontMaterial.shininess );\
    if( dotNL <= 0.0 ) powNH = 0.0;\
    vec4 spc = gl_FrontLightProduct[i].specular * powNH;\
    gl_FragColor += ( amb + dif + spc ) * attenuation;\
  }\
}"

#define rkglShaderCreateSpotlight() rkglShaderCreate( RKGL_SHADER_VERT_POS_NORM, RKGL_SHADER_FRAG_PHONG_SPOTLIGHT )

#define rkglShaderSetLightNum(p) rkglShaderSetUniform1i( p, "num_of_light", rkglLightNum() )

#endif /* __RKGL_GLSL_SPOTLIGHT_H__ */
