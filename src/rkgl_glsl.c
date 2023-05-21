/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl - programmable shaders with GLSL.
 */

#include <roki_gl/rkgl_glsl.h>

static bool _rkglShaderOutputLog(void (* get_info_log)(GLuint,GLsizei,GLsizei*,GLchar*), GLuint id)
{
  GLsizei size, len;
  char *logbuf;

  glGetProgramiv( id, GL_INFO_LOG_LENGTH, &size );
  if( !( logbuf = zAlloc( GLchar, size ) ) ){
    ZALLOCERROR();
  } else{
    get_info_log( id, size, &len, logbuf );
    eprintf( "%s.\n", logbuf );
    free( logbuf );
  }
  return false;
}

static bool _rkglShaderCompile(GLuint shader, const char *source)
{
  GLsizei size;
  GLint compiled;

  size = strlen( source );
  glShaderSource( shader, sizeof(GLchar), (const GLchar **)&source, &size );
  glCompileShader( shader );
  glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
  if( compiled == GL_FALSE ){
    ZRUNERROR( "Failed to compile shader" );
    return _rkglShaderOutputLog( glGetShaderInfoLog, shader );
  }
  return true;
}

static bool _rkglShaderCompileFile(GLuint shader, const char *file)
{
  FILE *fp;
  char *source;
  GLsizei size;
  bool ret = false;

  if( !( fp = fopen( file, "r" ) ) ){
    ZOPENERROR( file );
    return false;
  }
  fseek( fp, 0, SEEK_END );
  size = ftell( fp );
  if( !( source = zAlloc( GLchar, size ) ) ){
    ZALLOCERROR();
    goto TERMINATE;
  }
  fseek( fp, 0, SEEK_SET );
  if( (GLsizei)fread( source, sizeof(GLchar), size, fp ) == size )
    ret = _rkglShaderCompile( shader, source );
  free( source );
 TERMINATE:
  fclose( fp );
  return ret;
}

static bool _rkglShaderLink(GLuint program)
{
  GLint linked;

  glLinkProgram( program );
  glGetProgramiv( program, GL_LINK_STATUS, &linked );
  if( linked == GL_FALSE ){
    ZRUNERROR( "Cannot link shader" );
    return _rkglShaderOutputLog( glGetProgramInfoLog, program );
  }
  return true;
}

static bool _rkglShaderCreateFromFile(GLuint program, uint shadertype, const char *file)
{
  GLuint shader;
  bool ret;

  shader = glCreateShader( shadertype );
  ret = _rkglShaderCompileFile( shader, file );
  glAttachShader( program, shader );
  glDeleteShader( shader );
  return ret;
}

GLuint rkglShaderCreateFromFile(const char *vsfile, const char *fsfile)
{
  GLuint program;

  program = glCreateProgram();
  if( vsfile )
    if( !_rkglShaderCreateFromFile( program, GL_VERTEX_SHADER, vsfile ) ) return 0;
  if( fsfile )
    if( !_rkglShaderCreateFromFile( program, GL_FRAGMENT_SHADER, fsfile ) ) return 0;
  return _rkglShaderLink( program ) ? program : 0;
}

static bool _rkglShaderCreate(GLuint program, uint shadertype, const char *source)
{
  GLuint shader;
  bool ret;

  shader = glCreateShader( shadertype );
  ret = _rkglShaderCompile( shader, source );
  glAttachShader( program, shader );
  glDeleteShader( shader );
  return ret;
}

GLuint rkglShaderCreate(const char *vssource, const char *fssource)
{
  GLuint program;

  program = glCreateProgram();
  if( vssource )
    if( !_rkglShaderCreate( program, GL_VERTEX_SHADER, vssource ) ) return 0;
  if( fssource )
    if( !_rkglShaderCreate( program, GL_FRAGMENT_SHADER, fssource ) ) return 0;
  return _rkglShaderLink( program ) ? program : 0;
}

void rkglShaderSetUniform1i(GLuint shader, const char *varname, int val)
{
  GLint var;
  var = glGetUniformLocation( shader, varname );
  glUniform1i( var, val );
}

void rkglShaderSetUniform1f(GLuint shader, const char *varname, float val)
{
  GLfloat var;
  var = glGetUniformLocation( shader, varname );
  glUniform1f( var, val );
}

static void _rkglShaderSetUniformMat(GLuint shader, const char *varname, GLuint matid, GLboolean transpose)
{
  GLfloat m[16];
  GLint var;

  var = glGetUniformLocation( shader, varname );
  glGetFloatv( matid, m );
  glUniformMatrix4fv( var, 1, transpose, m );
}

void rkglShaderSetUniformMat(GLuint shader, const char *varname, GLuint matid)
{
  _rkglShaderSetUniformMat( shader, varname, matid, GL_FALSE );
}

void rkglShaderSetUniformMatT(GLuint shader, const char *varname, GLuint matid)
{
  _rkglShaderSetUniformMat( shader, varname, matid, GL_TRUE );
}

/* texture mapping */

GLuint rkglShaderCreateTexture(void)
{
  GLuint program;

  program = rkglShaderCreate( RKGL_SHADER_VERT_TEXTURE, RKGL_SHADER_FRAG_TEXTURE );
  glUseProgram( program );
  rkglShaderSetTextureNum( program, 1 );
  rkglShaderSetTextureMixRate0( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate1( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate2( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate3( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate4( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate5( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate6( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  rkglShaderSetTextureMixRate7( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  glUseProgram( 0 );
  return program;
}
