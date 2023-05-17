/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions.
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/rkgl_input.h>

void rkglKeyCARotateUp(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCATiltUp( cam, d ) : rkglCAAngleUp( cam, d );
}
void rkglKeyCARotateDown(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCATiltDown( cam, d ) : rkglCAAngleDown( cam, d );
}
void rkglKeyCARotateLeft(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCAPanLeft( cam, d ) : rkglCARoundLeft( cam, d );
}
void rkglKeyCARotateRight(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCAPanRight( cam, d ) : rkglCARoundRight( cam, d );
}

/* mouse state */
int rkgl_mouse_button;
int rkgl_mouse_x;
int rkgl_mouse_y;
int rkgl_key_mod;

void rkglMouseStoreInput(int button, int state, int presscode, int x, int y, int mod)
{
  rkgl_mouse_button = state == presscode ? button : -1;
  rkglMouseStoreXY( x, y );
  rkgl_key_mod = mod;
}

void rkglMouseDragGetIncrementer(rkglCamera *cam, int x, int y, double *dx, double *dy)
{
  *dx = (double)( x - rkgl_mouse_x ) / cam->vp[3];
  *dy =-(double)( y - rkgl_mouse_y ) / cam->vp[2];
}

void rkglMouseDragCARotate(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  double r;

  r = 180 * sqrt( dx*dx + dy*dy );
  rkgl_key_mod & ctrlkey ? rkglCARotate( cam, r, -dy, dx, 0 ) : rkglCALockonRotate( cam, r, -dy, dx, 0 );
}

void rkglMouseDragCATranslate(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  rkgl_key_mod & ctrlkey ? rkglCAMove( cam, 0, dx, dy ) : rkglCARelMove( cam, 0, dx, dy );
}

void rkglMouseDragCAZoom(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  rkgl_key_mod & ctrlkey ? rkglCAMove( cam, -dy, 0, 0 ) : rkglCARelMove( cam, -dy, 0, 0 );
}
