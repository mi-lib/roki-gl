#include <roki_gl/roki_glfw.h>
#include <signal.h>

rkglCamera g_cam;
rkglLight g_light;

zBox3D g_box;

void drawCube(void)
{
  glLineWidth( 2 );
  rkglBox( &g_box, RKGL_FACE | RKGL_WIREFRAME );
}

void display(GLFWwindow* window)
{
  zOpticalInfo oi;

  rkglClear();
  rkglCameraPut( &g_cam );
  rkglLightPut( &g_light );
  glPushMatrix();
  zOpticalInfoCreateSimple( &oi, 0.2, 0.4, 0.9, NULL );
  rkglMaterial( &oi );
  drawCube();
  glPopMatrix();
  glfwSwapBuffers( window );
}

void init(void)
{
  zVec3D center;

  rkglCameraInit( &g_cam );
  rkglCameraSetBackground( &g_cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &g_cam, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
  rkglCameraFitPerspective( &g_cam, 30.0, 1.0, 200.0 );
  rkglSetDefaultCamera( &g_cam );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &g_light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &g_light, 10, 10, 10 );

  zVec3DCreate( &center, 0, 0, 0 );
  zBox3DCreateAlign( &g_box, &center, 1.5, 1.5, 1.5 );
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
  glfwSetCharCallback( window, rkglCharFuncGLFW );
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
  glfwDestroyWindow( window );
  glfwTerminate();
  return 0;
}
