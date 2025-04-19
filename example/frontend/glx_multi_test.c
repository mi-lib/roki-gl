#include <zx11/zxwidget.h>
#include <roki_gl/roki_glx.h>

Window glwin[2];
rkglCamera cam[2];
rkglLight light;
int obj;

int enter(void)
{
  int obj;

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
  zBox3DCreateAlign( &box, &box_c, 3.0, 4.0, 0.4 );
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
  return obj;
}

GLvoid init(int width, int height)
{
  obj = enter();

  rkglCameraSetBackground( &cam[0], 0.1, 0.1, 0.1 );
  rkglCameraSetViewport( &cam[0], 0, 0, width, height );
  rkglCameraLookAt( &cam[0], 5, 0, 3, 0, 0, 0, 0, 0, 1 );

  rkglCameraSetBackground( &cam[1], 0.1, 0.1, 0.1 );
  rkglCameraSetViewport( &cam[1], 0, 0, width, height );
  rkglCameraLookAt( &cam[1], 0, 5, 3, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.4, 0.4, 0.4, 0.8, 0.8, 0.8, 0, 0, 0 );
}

void draw_button(zxWindow *win)
{
  zxwButton btn;

  zxWidgetInit( win );
  zxwButtonCreate( &btn, 20, zxWindowHeight(win)-50, 80, 24, "hello" );
  zxwButtonDrawRaise( win, &btn );
}

void glwin_resize(Window win, int x, int y, int w, int h)
{
  XMoveWindow( zxdisplay, win, x, y );
  XResizeWindow( zxdisplay, win, w, h );
}

void resize(zxWindow *win)
{
  int w, h;

  w = zxWindowWidth(win) / 2 - 40;
  h = zxWindowHeight(win) - 40 - 50;
  glwin_resize( glwin[0],   20, 20, w, h );
  glwin_resize( glwin[1], w+60, 20, w, h );
  draw_button( win );

  rkglCameraSetViewport( &cam[0], 0, 0, w, h );
  rkglCameraFitPerspective( &cam[0], 60, 1, 20 );

  rkglCameraSetViewport( &cam[1], 0, 0, w, h );
  rkglCameraFitPerspective( &cam[1], 60, 1, 20 );
}

GLvoid draw(Window win, rkglCamera *cam, rkglLight *light)
{
  rkglWindowActivateGLX( win );
  rkglClear();
  rkglCameraPut( cam );
  rkglLightPut( light );
  glPushMatrix();
    glCallList( obj );
  glPopMatrix();
  rkglWindowSwapBuffersGLX( win );
  rkglFlushGLX();
}

GLvoid mainloop(zxWindow *win)
{
  double phase;

  while( 1 ){
    switch( zxGetEvent() ){
    case KeyPress:
      switch( zxKeySymbol() ){
      case XK_q:
        rkglExitGLX();
        exit( 0 );
      }
      break;
    case ConfigureNotify:
      zxWindowUpdateRegion( win );
      resize( win );
      break;
    default: ;
    }
    phase = 2 * zPI * (double)clock() / CLOCKS_PER_SEC;
    rkglLightMove( &light, 10*cos(phase), 10*sin(phase), 8 );
    draw( glwin[0], &cam[0], &light );
    draw( glwin[1], &cam[1], &light );
  }
}

#define WIDTH  800
#define HEIGHT 450

int main(int argc, char **argv)
{
  zxWindow mainwin;

  rkglInitGLX();
  zxWindowCreate( &mainwin, 0, 0, WIDTH, HEIGHT );
  zxWindowSetBGColorByName( &mainwin, "lightgray" );
  zxWindowKeyEnable( &mainwin );
  zxWindowSetTitle( &mainwin, "glx test" );
  zxWindowOpen( &mainwin );

  glwin[0] = rkglWindowCreateGLX( &mainwin, 20, 20, WIDTH, HEIGHT, NULL );
  rkglWindowOpenGLX( glwin[0] );
  glwin[1] = rkglWindowCreateGLX( &mainwin, WIDTH+60, 20, WIDTH, HEIGHT, NULL );
  rkglWindowOpenGLX( glwin[1] );

  init( 360, 260 );
  resize( &mainwin );
  mainloop( &mainwin );
  return 0;
}
