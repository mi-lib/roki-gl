/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_fog - programmable shaders with GLSL : fog effect.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_FOG_H__
#define __RKGL_GLSL_FOG_H__

#define RKGL_SHADER_VERT_FOG "\
varying vec3 pos;\
varying vec3 norm;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  gl_FogFragCoord = abs( pos.z );\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_PHONG_SPOTLIGHT_FOG "\
varying vec3 pos;\
varying vec3 norm;\
uniform int num_of_light;\
uniform int fog_mode;\
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
  float fog;\
  float a = gl_Fog.density * gl_FogFragCoord;\
  if( fog_mode == 0x0800 )\
    fog = exp( -a );\
  else if( fog_mode == 0x0801 )\
    fog = exp( -a*a );\
  else\
    fog = ( gl_Fog.end - gl_FogFragCoord ) * gl_Fog.scale;\
  fog = clamp( fog, 0.0, 1.0 );\
  gl_FragColor = mix( gl_Fog.color, gl_FragColor, fog );\
}"

#define rkglShaderCreateFog() rkglShaderCreate( RKGL_SHADER_VERT_FOG, RKGL_SHADER_FRAG_PHONG_SPOTLIGHT_FOG )

#define rkglShaderSetFogMode(p) do{\
  GLint fog_mode;\
  glGetIntegerv( GL_FOG_MODE, &fog_mode );\
  rkglShaderSetUniform1i( p, "fog_mode", fog_mode );\
} while(0)

#endif /* __RKGL_GLSL_FOG_H__ */
