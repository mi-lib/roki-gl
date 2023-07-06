#include <roki_gl/roki_glx.h>
#include <zx11/zximage.h>

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

  rkglBGSet( &cam, 0.5, 0.5, 0.5 );
  rkglVPCreate( &cam, 0, 0, width, height );
  rkglFrustumScale( &cam, 1.0/180, 3, 10 );
  rkglCALookAt( &cam, 5,-3, 3, 0, 0, 0, 0, 0, 1 );

  glEnable( GL_LIGHTING );
  rkglLightCreate( &light, 0.4, 0.4, 0.4, 0.8, 0.8, 0.8, 0, 0, 0 );
}

GLvoid draw(Window win)
{
  rkglWindowActivateGLX( win );
  rkglClear();
  rkglCALoad( &cam );
  rkglLightMove( &light, 10, 0, 8 );
  glPushMatrix();
  glCallList( obj );
  glPopMatrix();
  rkglWindowSwapBuffersGLX( win );
  rkglFlushGLX();
}

#define WIDTH  480
#define HEIGHT 480

int main(int argc, char **argv)
{
  Window win;
  zxImage img1, img2, img3;

  rkglInitGLX();
  win = rkglWindowCreateGLX( NULL, 0, 0, WIDTH,  HEIGHT, "image capturing test" );
  init( WIDTH, HEIGHT );
  rkglWindowOpenGLX( win );
  draw( win );
  draw( win );
  draw( win );
  glFinish();

  rkglReadRGBImageGLX( win, &img1 );
  rkglReadRGBImage( &img2 );
  rkglReadDepthImage( &img3 );
  zxImageWriteFile( &img1, "out_glx.bmp" );
  zxImageWriteFile( &img2, "out_rgb.bmp" );
  zxImageWriteFile( &img3, "out_dep.bmp" );
  zxImageDestroy( &img1 );
  zxImageDestroy( &img2 );
  zxImageDestroy( &img3 );

  rkglExitGLX();
  return 0;
}
