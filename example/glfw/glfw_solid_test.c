#include <roki_gl/roki_glfw.h>
#include <signal.h>

enum{
  TEST_CUBE = 0,
  TEST_NUM,
};

rkglCamera g_cam;
rkglLight light;
static GLFWwindow* g_window;

zBox3D g_box;

void drawCube(void)
{
  rkglBox( &g_box, RKGL_FACE | RKGL_WIREFRAME );
}

void display(GLFWwindow* window)
{
  zOpticalInfo oi;

  zOpticalInfoCreateSimple( &oi, 0.2, 0.4, 0.9, NULL );
  rkglClear();
  rkglCALoad( &g_cam );
  rkglLightPut( &light );
  glPushMatrix();

  rkglMaterial( &oi );
  drawCube();

  glPopMatrix();
  glfwSwapBuffers( window );
}

void init(void)
{
  rkglSetDefaultCallbackParam( &g_cam, 1.0, 1.0, 20.0, 1.0, 5.0 );

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 10, 10 );

  zVec3D center;
  zVec3DCreate( &center, 0, 0, 0 );
  zBox3DCreateAlign( &g_box, &center, 1.5, 1.5, 1.5 );

  zRandInit();
}

int main(int argc, char *argv[])
{
  if( rkglInitGLFW( &argc, argv ) < 0 ){
    return 1;
  }
  int width = 640;
  int height = 480;
  glfwWindowHint( GLFW_VISIBLE, false );
  g_window = glfwCreateWindow(width, height, argv[0], NULL, NULL);
  if( rkglWindowCreateGLFW( g_window, 0, 0, width, height, argv[0] ) < 0 ) {
    return 1;
  }

  glfwSetWindowSizeCallback( g_window, rkglReshapeFuncGLFW );
  glfwSetCharCallback( g_window, rkglCharFuncGLFW );
  glfwSetKeyCallback( g_window, rkglKeyFuncGLFW );
  glfwSetMouseButtonCallback( g_window, rkglMouseFuncGLFW );
  glfwSetScrollCallback( g_window, rkglMouseWheelFuncGLFW );
  glfwSetCursorPosCallback( g_window, rkglMouseDragFuncGLFW );

  init();

  rkglReshapeFuncGLFW( g_window, width, height );
  glfwSwapInterval(1);

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE ){
    display(g_window);
    glfwPollEvents();
  }
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
