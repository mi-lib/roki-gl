/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glsl - programmable shaders with GLSL.
 */

#include <roki-gl/rkgl_glsl.h>

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

static void _rkglShaderCreateFromFile(GLuint program, uint shadertype, const char *file)
{
  GLuint shader;

  shader = glCreateShader( shadertype );
  _rkglShaderCompileFile( shader, file );
  glAttachShader( program, shader );
  glDeleteShader( shader );
}

GLuint rkglShaderCreateFromFile(const char *vsfile, const char *fsfile)
{
  GLuint program;

  program = glCreateProgram();
  if( vsfile )
    _rkglShaderCreateFromFile( program, GL_VERTEX_SHADER, vsfile );
  if( fsfile )
    _rkglShaderCreateFromFile( program, GL_FRAGMENT_SHADER, fsfile );
  return _rkglShaderLink( program ) ? program : 0;
}

static void _rkglShaderCreate(GLuint program, uint shadertype, const char *source)
{
  GLuint shader;

  shader = glCreateShader( shadertype );
  _rkglShaderCompile( shader, source );
  glAttachShader( program, shader );
  glDeleteShader( shader );
}

GLuint rkglShaderCreate(const char *vssource, const char *fssource)
{
  GLuint program;

  program = glCreateProgram();
  if( vssource )
    _rkglShaderCreate( program, GL_VERTEX_SHADER, vssource );
  if( fssource )
    _rkglShaderCreate( program, GL_FRAGMENT_SHADER, fssource );
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

/* texture mapping */

GLuint rkglShaderCreateTexture(void)
{
  GLuint program;

  program = rkglShaderCreate( RKGL_SHADER_VERT_TEXTURE, RKGL_SHADER_FRAG_TEXTURE );
  glUseProgram( program );
  rkglShaderSetTextureMixRate( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  glUseProgram( 0 );
  return program;
}

GLuint rkglShaderCreateTexture2(void)
{
  GLuint program;

  program = rkglShaderCreate( RKGL_SHADER_VERT_TEXTURE, RKGL_SHADER_FRAG_TEXTURE2 );
  glUseProgram( program );
  rkglShaderSetTextureMixFactor( program, RKGL_SHADER_DEFAULT_MIX_FACTOR );
  rkglShaderSetTextureMixRate( program, RKGL_SHADER_DEFAULT_MIX_RATE );
  glUseProgram( 0 );
  return program;
}
