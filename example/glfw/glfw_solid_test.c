#include <roki_gl/roki_glfw.h>
#include <signal.h>

enum{
  TEST_CUBE = 0,
  TEST_NUM,
};

bool sw[TEST_NUM];
rkglCamera g_cam;
rkglLight light;
static GLFWwindow* g_window;

void keyfunc(GLFWwindow* window, unsigned int key)
{
  int val=-1;

  switch( key ){
  case 'a': val = TEST_CUBE;                break;
  case 'q': case 'Q': case '\033':
    raise( SIGTERM );
    exit( EXIT_SUCCESS );
  default: ;
  }
  if( val>=0 && val<TEST_NUM ) sw[val] = 1 - sw[val];
  glfwPostEmptyEvent();
}

/* Normals for the 6 faces of a cube. */
GLfloat g_normals[6][3] = { { -1.0, 0.0,  0.0 },
                            { 0.0,  1.0,  0.0 },
                            { 1.0,  0.0,  0.0 },
                            { 0.0, -1.0,  0.0 },
                            { 0.0,  0.0,  1.0 },
                            { 0.0,  0.0, -1.0 } };
/* Vertex indices for the 6 faces of a cube. */
GLint g_faces[6][4] = { {0, 1, 2, 3},
                        {3, 2, 6, 7},
                        {7, 6, 5, 4},
                        {4, 5, 1, 0},
                        {5, 6, 2, 1},
                        {7, 4, 0, 3} };
/* X,Y,Z vertexes. */
GLfloat g_v[8][3];

void initCube(void)
{
  g_v[0][0] = g_v[1][0] = g_v[2][0] = g_v[3][0] = -1;
  g_v[4][0] = g_v[5][0] = g_v[6][0] = g_v[7][0] = 1;
  g_v[0][1] = g_v[1][1] = g_v[4][1] = g_v[5][1] = -1;
  g_v[2][1] = g_v[3][1] = g_v[6][1] = g_v[7][1] = 1;
  g_v[0][2] = g_v[3][2] = g_v[4][2] = g_v[7][2] = 1;
  g_v[1][2] = g_v[2][2] = g_v[5][2] = g_v[6][2] = -1;
}

void drawCube(void)
{
  int i;

  for(i = 0; i < 6; i++){
    glBegin( GL_POLYGON );
    glNormal3fv( &g_normals[i][0] );
    glVertex3fv( &g_v[ g_faces[i][0] ][0] );
    glVertex3fv( &g_v[ g_faces[i][1] ][0] );
    glVertex3fv( &g_v[ g_faces[i][2] ][0] );
    glVertex3fv( &g_v[ g_faces[i][3] ][0] );
    glEnd();
  }
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
  if( sw[TEST_CUBE] ) drawCube();

  glPopMatrix();
  glfwSwapBuffers( window );
}

bool g_mouse_left_button_clicked;
bool g_mouse_right_button_clicked;
int g_x;
int g_y;

void mouse(GLFWwindow* window, int button, int state, int mods)
{
  if( button == GLFW_MOUSE_BUTTON_LEFT ){
    if( state == GLFW_PRESS ){
      g_mouse_left_button_clicked = true;
    } else if( state == GLFW_RELEASE ){
      g_mouse_left_button_clicked = false;
    }
  } else if( button == GLFW_MOUSE_BUTTON_RIGHT ){
    if( state == GLFW_PRESS ){
      g_mouse_right_button_clicked = true;
    } else if( state == GLFW_RELEASE ){
      g_mouse_right_button_clicked = false;
    }
  }
  rkglMouseFuncGLFW( window, button, state, g_x, g_y );
}

void motion(GLFWwindow* window, double x, double y)
{
  g_x = floor(x);
  g_y = floor(y);
  if( g_mouse_left_button_clicked || g_mouse_right_button_clicked )
  {
    rkglMouseDragFuncGLFW( window, g_x, g_y );
  }
}

void init(void)
{
  double znear = 1.0;
  double zfar = 100.0;
  double dl = 0.1;
  double da = 5.0;
  rkglSetCallbackParamGLFW( &g_cam, 2.0, znear, zfar, dl, da );

  initCube();

  rkglBGSet( &g_cam, 0.5, 0.5, 0.5 );
  rkglCASet( &g_cam, 10, 0, 0, 0, 0, 0 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 10, 10, 10 );

  memset( sw, 0, sizeof(sw) );
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

  /* not use rkglVisFuncGLFW, rkglSpecialFuncGLFW */
  glfwSetWindowSizeCallback( g_window, rkglReshapeFuncGLFW );
  glfwSetCharCallback( g_window, keyfunc );
  glfwSetMouseButtonCallback( g_window, mouse );
  glfwSetScrollCallback( g_window, rkglMouseWheelFuncGLFW );
  glfwSetCursorPosCallback( g_window, motion );

  init();

  rkglReshapeFuncGLFW( g_window, width, height );
  glfwSwapInterval(1);

  printf( "a: cube\n" );

  while ( glfwWindowShouldClose( g_window ) == GL_FALSE ){
    rkglIdleFuncGLFW();
    display(g_window);
    glfwPollEvents();
  }
  glfwDestroyWindow( g_window );
  glfwTerminate();

  return 0;
}
