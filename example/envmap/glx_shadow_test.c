#include <roki_gl/roki_glx.h>

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

  rkglCameraInit( &cam );
  rkglCameraSetBackground( &cam, 0.5, 0.5, 0.5 );
  rkglCameraSetViewframe( &cam, 15, 0, 6, 0, -30, 0 );
  rkglLightCreate( &light, 0.8, 0.8, 0.8, 1, 1, 1, 0, 0, 0 );
  rkglLightMove( &light, 3, 6, 20 );
  rkglShadowInit( &shadow, TEXWIDTH, TEXHEIGHT, 10.0, 0.2, 0 );
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
    case ButtonRelease: rkglMouseFuncGLX( &cam, event ); break;
    case MotionNotify:  rkglMouseDragFuncGLX( &cam ); break;
    case KeyPress:      if( rkglKeyPressFuncGLX( &cam ) < 0 ) exit( 0 ); break;
    case KeyRelease:    rkglKeyReleaseFuncGLX( &cam ); break;
    case Expose: case ConfigureNotify:
      zxGetGeometry( win, &reg );
      rkglReshapeGLX( &cam, reg.width, reg.height );
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
  mainloop( win );
  return 0;
}
