#include <roki-gl/rkgl_shape.h>
#include <roki-gl/rkgl_glx.h>

int obj;

void enter(void)
{
  zOpticalInfo cyl_oi, con_oi, box_oi;
  zVec3D cyl_c1 = { { 0.0,-0.5,-0.5 } };
  zVec3D cyl_c2 = { { 0.0,-0.5, 2.0 } };
  zVec3D con_c = { { 0.0, 0.5,-0.5 } };
  zVec3D con_v = { { 0.0, 0.5, 1.5 } };
  zVec3D box_c = { { 0.0, 0.0,-0.7 } };
  zCyl3D cyl;
  zCone3D con;
  zBox3D box;

  zOpticalInfoCreate( &cyl_oi, 0.4, 1.0, 1.0, 0.4, 1.0, 1.0, 0.4, 1.0, 1.0, 0, 0.4, 1.0, NULL );
  zOpticalInfoCreate( &con_oi, 0.2, 1.0, 0.2, 0.2, 1.0, 0.2, 0.2, 1.0, 0.2, 0, 0.6, 1.0, NULL );
  zOpticalInfoCreate( &box_oi, 0.8, 0.6, 0.0, 0.6, 0.4, 0.0, 0.8, 0.6, 0.2, 0, 1.0, 1.0, NULL );

  zCyl3DCreate( &cyl, &cyl_c1, &cyl_c2, 0.3, 0 );
  zCone3DCreate( &con, &con_c, &con_v, 0.7, 0 );
  zBox3DCreateAlign( &box, &box_c, 2.0, 3.0, 0.4 );
  obj = rkglBeginList();
  /* cylinder */
  rkglMaterial( &cyl_oi );
  rkglCyl( &cyl );
  /* cone */
  rkglMaterial( &con_oi );
  rkglCone( &con );
  /* box */
  rkglMaterial( &box_oi );
  rkglBox( &box );

  glEndList();
}

rkglCamera cam;
rkglLight light;

GLvoid init(GLsizei width, GLsizei height) 
{
  enter();

  rkglBGSet( &cam, 0.1, 0.1, 0.1 );
  rkglCALookAt( &cam, 5,-3, 3, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0, 0.4, 0.4, 0.4, 0.8, 0.8, 0.8, 0, 0, 0, 0 );
}

GLvoid draw(Window win)
{
  rkglActivateGLX( win );
  rkglClear();
  rkglCALoad( &cam );
  rkglLightSetPos( &light,
    8, 10*sin(4*zPI*(double)clock()/CLOCKS_PER_SEC), 5 );
  glPushMatrix();
  glCallList( obj );
  glPopMatrix();
  rkglSwapBuffersGLX( win );
  rkglFlushGLX();
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
      rkglMouseFuncGLX( zxMouseButton, event, zxMouseX, zxMouseY );
      break;
    case MotionNotify:
      rkglMouseDragFuncGLX( &cam, zxMouseX, zxMouseY );
      break;
    case KeyPress:
      if( rkglKeyFuncGLX( &cam, zxKeySymbol(), zxMouseX, zxMouseY, 1.0, 5.0 ) < 0 )
        return;
      break;
    case KeyRelease:
      zxModkeyOff( zxKeySymbol() );
      break;
    case Expose:
    case ConfigureNotify:
      zxGetGeometry( win, &reg );
      rkglReshapeGLX( &cam, reg.width, reg.height, 2.0, 2, 20 );
      break;
    default: ;
    }
    if( ++count > 5 ){
      draw( win );
      count = 0;
    }
  }
}

#define WIDTH  500
#define HEIGHT 500

int main(int argc, char **argv)
{
  Window win;

  rkglInitGLX();
  win = rkglWindowCreateGLX( NULL, 0, 0, WIDTH,  HEIGHT, "glx test" );
  rkglKeyEnableGLX( win );
  rkglMouseEnableGLX( win );
  rkglWindowOpenGLX( win );

  init( WIDTH, HEIGHT );
  mainloop( win );
  rkglWindowCloseGLX( win );
  rkglCloseGLX();
  return 0;
}
