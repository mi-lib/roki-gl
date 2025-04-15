/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions.
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/rkgl_input.h>

void rkglKeyCameraRotateUp(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCameraRotate( cam, d,-1, 0, 0 ) : rkglCameraLockonAndRotate( cam, d, 1, 0, 0 );
}
void rkglKeyCameraRotateDown(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCameraRotate( cam, d, 1, 0, 0 ) : rkglCameraLockonAndRotate( cam, d,-1, 0, 0 );
}
void rkglKeyCameraRotateLeft(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCameraRotate( cam, d, 0,-1, 0 ) : rkglCameraLockonAndRotate( cam, d, 0, 1, 0 );
}
void rkglKeyCameraRotateRight(rkglCamera *cam, double d, bool ctrlison){
  ctrlison ? rkglCameraRotate( cam, d, 0, 1, 0 ) : rkglCameraLockonAndRotate( cam, d, 0,-1, 0 );
}

/* mouse state */
int rkgl_mouse_button;
int rkgl_mouse_x;
int rkgl_mouse_y;
int rkgl_key_mod;

void rkglMouseStoreButtonMod(int button, int state, int presscode, int mod)
{
  rkgl_mouse_button = state == presscode ? button : -1;
  rkgl_key_mod = mod;
}

void rkglMouseStoreInput(int button, int state, int presscode, int x, int y, int mod)
{
  rkglMouseStoreButtonMod( button, state, presscode, mod );
  rkglMouseStoreXY( x, y );
}

void rkglMouseDragGetIncrementer(rkglCamera *cam, int x, int y, double *dx, double *dy)
{
  *dx = (double)( x - rkgl_mouse_x ) / cam->viewport[3];
  *dy =-(double)( y - rkgl_mouse_y ) / cam->viewport[2];
}

void rkglMouseDragCameraRotate(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  double r;

  r = 180 * sqrt( dx*dx + dy*dy );
  rkgl_key_mod & ctrlkey ? rkglCameraRotate( cam, r, -dy, dx, 0 ) : rkglCameraLockonAndRotate( cam, r, -dy, dx, 0 );
}

void rkglMouseDragCameraTranslate(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  rkgl_key_mod & ctrlkey ? rkglCameraMove( cam, 0, dx, dy ) : rkglCameraRelMove( cam, 0, dx, dy );
}

void rkglMouseDragCameraZoom(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  rkgl_key_mod & ctrlkey ? rkglCameraMove( cam, -dy, 0, 0 ) : rkglCameraRelMove( cam, -dy, 0, 0 );
}
