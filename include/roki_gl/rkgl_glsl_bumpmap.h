/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl_bumpmap - programmable shaders with GLSL : bump mapping.
 */

/* NOTE: never include this header file in user programs. */

#ifndef __RKGL_GLSL_BUMPMAP_H__
#define __RKGL_GLSL_BUMPMAP_H__

#define RKGL_SHADER_VERT_BUMPMAP "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 tan1;\
varying vec3 tan2;\
attribute vec3 tangent;\
void main(void)\
{\
  pos = vec3( gl_ModelViewMatrix * gl_Vertex );\
  norm = normalize( gl_NormalMatrix * gl_Normal );\
  tan1 = normalize( gl_NormalMatrix * tangent );\
  tan2 = cross( norm, tan1 );\
  gl_TexCoord[0] = gl_MultiTexCoord0;\
  gl_Position = ftransform();\
}"

#define RKGL_SHADER_FRAG_BUMPMAP "\
varying vec3 pos;\
varying vec3 norm;\
varying vec3 tan1;\
varying vec3 tan2;\
uniform sampler2D color_map;\
uniform sampler2D normal_map;\
void main(void)\
{\
  vec3 n = texture2DProj( normal_map, gl_TexCoord[0] ).xyz * 2.0 - 1.0;\
  vec3 norm2 = n.x * tan1 + n.y * tan2 + n.z * norm;\
  vec3 pos2 = pos + n.z * norm;\
  vec3 lv = gl_LightSource[0].position.xyz - pos2;\
  float d = length( lv );\
  float attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation\
                            + gl_LightSource[0].linearAttenuation * d\
                            + gl_LightSource[0].quadraticAttenuation * d * d );\
  lv = normalize( lv );\
  vec4 ambient = gl_FrontLightProduct[0].ambient;\
  float cos_norm_lv = dot( norm2, lv );\
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * cos_norm_lv;\
  vec4 texcolor = texture2D( color_map, gl_TexCoord[0].st );\
  vec3 view = normalize( -pos2 );\
  vec3 hv = normalize( lv + view );\
  float ref_shininess = pow( max( dot( norm2, hv ), 0.0 ), gl_FrontMaterial.shininess );\
  if( cos_norm_lv <= 0.0 ) ref_shininess = 0.0;\
  vec4 specular = gl_FrontLightProduct[0].specular * ref_shininess;\
  gl_FragColor.rgb = ( ( ambient.rgb + diffuse.rgb ) * texcolor.rgb + specular.rgb ) * attenuation;\
  gl_FragColor.a   =   ambient.a   + diffuse.a   + specular.a;\
}"

#define rkglShaderSetBumpmapColor(shader,i)   glUniform1i( glGetUniformLocation( shader, "color_map" ), i )
#define rkglShaderSetBumpmapNormal(shader,i)  glUniform1i( glGetUniformLocation( shader, "normal_map" ), i )
#define rkglShaderSetBumpmapTangent(shader,v) glVertexAttrib3dv( glGetAttribLocation( shader, "tangent" ), (v)->e )

#define rkglShaderCreateBumpmap() rkglShaderCreate( RKGL_SHADER_VERT_BUMPMAP, RKGL_SHADER_FRAG_BUMPMAP )

#endif /* __RKGL_GLSL_TEXTURE_H__ */
