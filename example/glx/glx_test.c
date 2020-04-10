#include <roki-gl/rkgl_camera.h>
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
  rkglCyl( &cyl, RKGL_FACE );
  /* cone */
  rkglMaterial( &con_oi );
  rkglCone( &con, RKGL_FACE );
  /* box */
  rkglMaterial( &box_oi );
  rkglBox( &box, RKGL_FACE );

  glEndList();
}

rkglCamera cam;
rkglLight light;

GLvoid init(GLsizei width, GLsizei height) 
{
  enter();

  rkglBGSet( &cam, 0.1, 0.1, 0.1 );
  rkglVPCreate( &cam, 0, 0, width, height );
  rkglFrustumScale( &cam, 1.0/640, 1, 20 );
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
  while( 1 ){
    switch( zxGetEvent() ){
    case KeyPress:
      switch( zxKeySymbol() ){
      case XK_q:
        rkglCloseGLX();
        exit( 0 );
      }
      break;
    case ConfigureNotify:
      /* NOTE: this careless resize is only valid in single window programs. */
      rkglVPCreate( &cam, 0, 0, zxevent.xconfigure.width, zxevent.xconfigure.height );
      break;
    default: ;
    }
    draw( win );
  }
}

#define WIDTH  500
#define HEIGHT 500

int main(int argc, char **argv)
{
  Window win;

  rkglInitGLX();
  win = rkglWindowCreateGLX( NULL, 0, 0, WIDTH,  HEIGHT, "glx test" );
  rkglWindowAddEventGLX( win, KeyPressMask | KeyReleaseMask );
  rkglWindowOpenGLX( win );

  init( WIDTH, HEIGHT );
  mainloop( win );
  return 0;
}
