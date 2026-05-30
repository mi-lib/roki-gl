/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_input - key and mouse actions.
 */

#include <signal.h>
#include <math.h>
#include <roki_gl/rkgl_input.h>

/* key modifier state */
int rkgl_key_mod;

/* delta amounts of translation and rotation corresponding to key actions. */
#define RKG_DEFAULT_KEY_DELTA_TRA 0.1
#define RKG_DEFAULT_KEY_DELTA_ROT 1.0

double rkgl_key_delta_tra = RKG_DEFAULT_KEY_DELTA_TRA;
double rkgl_key_delta_rot = RKG_DEFAULT_KEY_DELTA_ROT;

void rkglSetKeyDelta(double tra, double rot)
{
  rkgl_key_delta_tra = tra;
  rkgl_key_delta_rot = rot;
}

void rkglKeyCameraRotate(rkglCamera *cam, double angle, double dx, double dy, double dz, bool ctrlisin)
{
  zVec3D gaze_point;
  double distance;

  if( ctrlisin ){
    distance = zVec3DNorm( zFrame3DPos(&cam->viewframe) );
    zVec3DCat( zFrame3DPos(&cam->viewframe), -distance, zFrame3DVec(&cam->viewframe,zX), &gaze_point );
    angle *= 0.5;
    rkglCameraGazeAndRotate( cam, gaze_point.c.x, gaze_point.c.y, gaze_point.c.z, distance, angle*dx, angle*dy, angle*dz );
  } else{
    rkglCameraRotate( cam, angle * sqrt( dx*dx + dy*dy ), dz, dy, dx );
  }
}

/* mouse state */
int rkgl_mouse_button;
int rkgl_mouse_x;
int rkgl_mouse_y;

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
  *dx = (double)( x - rkgl_mouse_x ) / rkglCameraViewportWidth(cam);
  *dy =-(double)( y - rkgl_mouse_y ) / rkglCameraViewportHeight(cam);
}

void rkglMouseDragCameraRotate(rkglCamera *cam, double dx, double dy, int ctrlkey)
{
  rkglKeyCameraRotate( cam, 180, -dx, dy, 0, rkgl_key_mod & ctrlkey );
}

void rkglMouseDragCameraTranslate(rkglCamera *cam, double dx, double dy)
{
  rkglCameraTranslate( cam, 0, dx, dy );
}

void rkglMouseDragCameraZoom(rkglCamera *cam, double dx, double dy)
{
  /* dx is not used. */
  rkglCameraZoomIn( cam, -dy );
}
