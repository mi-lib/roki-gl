#include <roki_gl/roki_glfw.h>
#include <signal.h>

rkglCamera cam;
rkglLight light;

byte boxflag = 0;
zBox3D box[2];

void display(GLFWwindow* window)
{
  zRGB linecolor;
  zOpticalInfo oi;

  rkglClear();
  rkglCameraPut( &cam );
  glPushMatrix();
  rkglBox( &box[boxflag], RKGL_FACE );
  rkglCameraReadDepthBuffer( &cam );

  rkglClear();
  rkglLightPut( &light );
  zOpticalInfoCreateSimple( &oi, 0.2, 0.4, 0.9, NULL );
  rkglMaterial( &oi );
  rkglBox( &box[0], RKGL_FACE );
  zOpticalInfoCreateSimple( &oi, 0.8, 0.2, 0.2, NULL );
  rkglMaterial( &oi );
  rkglBox( &box[1], RKGL_FACE );
  glPopMatrix();
  /* contour */
  glPointSize( 2.0 );
  zRGBSet( &linecolor, 0, 1, 0 );
  rkglRGB( &linecolor );
  rkglContour( &cam );
  glfwSwapBuffers( window );
}

void init(void)
{
  zVec3D center;

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
  rkglCameraFitPerspective( &cam, 30.0, 1.0, 200.0 );
  rkglSetDefaultCamera( &cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 10, 10 );

  zVec3DCreate( &center, 0, 0, 0 );
  zBox3DCreateAlign( &box[0], &center, 1.5, 1.5, 1.5 );
  zVec3DCreate( &center,-2,-2, 0 );
  zBox3DCreateAlign( &box[1], &center, 1.5, 1.5, 1.5 );
}

void charfunc(GLFWwindow* window, unsigned int codepoint)
{
  switch( codepoint ){
  case 's': boxflag = 1 - boxflag; break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  glfwPostEmptyEvent();
}

int main(int argc, char *argv[])
{
  GLFWwindow* window;
  int width, height;

  if( rkglInitGLFW( &argc, argv ) < 0 )
    return 1;
  glfwWindowHint( GLFW_VISIBLE, false );
  width = 640;
  height = 480;
  if( !( window = rkglWindowCreateAndOpenGLFW( 0, 0, width, height, argv[0] ) ) )
    return 1;

  glfwSetWindowSizeCallback( window, rkglReshapeFuncGLFW );
  glfwSetCharCallback( window, charfunc );
  glfwSetKeyCallback( window, rkglKeyFuncGLFW );
  glfwSetMouseButtonCallback( window, rkglMouseFuncGLFW );
  glfwSetScrollCallback( window, rkglMouseWheelFuncGLFW );
  glfwSetCursorPosCallback( window, rkglMouseDragFuncGLFW );

  init();
  rkglReshapeFuncGLFW( window, width, height );
  glfwSwapInterval( 1 );
  while( glfwWindowShouldClose( window ) == GL_FALSE ){
    display( window );
    glfwPollEvents();
  }
  rkglCameraDestroy( &cam );
  glfwDestroyWindow( window );
  glfwTerminate();
  return 0;
}
