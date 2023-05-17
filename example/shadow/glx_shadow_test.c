#include <roki_gl/rkgl_camera.h>
#include <roki_gl/rkgl_shape.h>
#include <roki_gl/rkgl_shadow.h>
#include <roki_gl/rkgl_glx.h>

int sphere, cylinder, cone, box;
void scene(void)
{
  glCallList( sphere );
  glCallList( cylinder );
  glCallList( cone );
  glCallList( box );
}

rkglCamera cam;
rkglLight light;
rkglShadow shadow;

void display(Window win)
{
  rkglWindowActivateGLX( win );
  rkglShadowDraw( &shadow, &cam, &light, scene );
  rkglWindowSwapBuffersGLX( win );
  rkglFlushGLX();
}

void reshape(int w, int h)
{
  double x, y;

  rkglVPCreate( &cam, 0, 0, w, h );
  x = 0.5 * 5.0;
  y = x / rkglVPAspect(&cam);
  rkglFrustum( &cam, -x, x, -y, y, 5.0, 40.0 );
}

#define TEXWIDTH  1024
#define TEXHEIGHT 1024

void init(void)
{
  zOpticalInfo red, cyan, yellow, brown;
  zSphere3D sphere3d;
  zCyl3D cylinder3d;
  zCone3D cone3d;
  zBox3D box3d;
  zVec3D c1, c2;

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglCASet( &cam, 10, 0, 4, 0, -30, 0 );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 3, 6, 20 );
  rkglShadowInit( &shadow, TEXWIDTH, TEXHEIGHT, 10.0, 0.2 );
  rkglShadowDisableAntiZFighting( &shadow );

  sphere = rkglBeginList();
    zOpticalInfoCreateSimple( &red, 0.8, 0, 0, NULL );
    zVec3DCreate( &c1, 0, 0, 0 );
    zSphere3DCreate( &sphere3d, &c1, 1, 0 );
    rkglMaterial( &red );
    rkglSphere( &sphere3d, RKGL_FACE );
  glEndList();

  cylinder = rkglBeginList();
    zOpticalInfoCreateSimple( &cyan, 0.0, 0.8, 0.8, NULL );
    zVec3DCreate( &c1, 0, 2,-1.5 );
    zVec3DCreate( &c2, 0, 1, 1.5 );
    zCyl3DCreate( &cylinder3d, &c1, &c2, 0.5, 0 );
    rkglMaterial( &cyan );
    rkglCyl( &cylinder3d, RKGL_FACE );
  glEndList();

  cone = rkglBeginList();
    zOpticalInfoCreateSimple( &yellow, 0.8, 0.8, 0, NULL );
    zVec3DCreate( &c1, 0,-2,-2 );
    zVec3DCreate( &c2, 0,-1, 1 );
    zCone3DCreate( &cone3d, &c1, &c2, 1, 0 );
    rkglMaterial( &yellow );
    rkglCone( &cone3d, RKGL_FACE );
  glEndList();

  box = rkglBeginList();
    zOpticalInfoCreateSimple( &brown, 0.6, 0.4, 0.2, NULL );
    zVec3DCreate( &c1, 0, 0,-3 );
    zBox3DCreateAlign( &box3d, &c1, 5, 8, 0.5 );
    rkglMaterial( &brown );
    rkglBox( &box3d, RKGL_FACE );
  glEndList();
}

GLvoid mainloop(Window win)
{
  int event;
  zxRegion reg;
  int count = 0;

  while( 1 ){
    switch( ( event = zxGetEvent() ) ){
    case ButtonPress:
    case ButtonRelease:
      rkglMouseFuncGLX( &cam, event, 1.0 );
      break;
    case MotionNotify:
      rkglMouseDragFuncGLX( &cam );
      break;
    case KeyPress:
      switch( zxKeySymbol() ){
      case XK_q:
        rkglExitGLX();
        exit( 0 );
      }
      break;
    case Expose:
    case ConfigureNotify:
      zxGetGeometry( win, &reg );
      rkglReshapeGLX( &cam, reg.width, reg.height, 2.0, 2, 20 );
      break;
    default: ;
    }
    if( ++count > 5 ){
      display( win );
      count = 0;
    }
  }
}

#define WIDTH  480
#define HEIGHT 320

int main(int argc, char **argv)
{
  Window win;

  rkglInitGLX();
  win = rkglWindowCreateGLX( NULL, 0, 0, WIDTH, HEIGHT, "glx test" );
  rkglWindowKeyEnableGLX( win );
  rkglWindowMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  init();
  reshape( WIDTH, HEIGHT );
  mainloop( win );
  return 0;
}
