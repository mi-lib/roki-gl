/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_camera - camera work
 */

#ifndef __RKGL_CAMERA_H__
#define __RKGL_CAMERA_H__

#include <roki-gl/rkgl_misc.h>

__BEGIN_DECLS

typedef struct{
  GLclampf bg[4];  /* background color */
  GLint vp[4];     /* viewport */
  GLdouble vv[16]; /* view volume */
  GLdouble ca[16]; /* camera angle */
} rkglCamera;

/* background color */

#define rkglBGSet(c,r,g,b) do{\
  (c)->bg[0] = (r);\
  (c)->bg[1] = (g);\
  (c)->bg[2] = (b);\
  (c)->bg[3] = 1.0;\
} while(0)

/* viewport */

__EXPORT void rkglVPCreate(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h);
__EXPORT void rkglVPLoad(rkglCamera *c);
__EXPORT void rkglVPGet(rkglCamera *c);

#define rkglVPOX(c)     ( (double)(c)->vp[0] )
#define rkglVPOY(c)     ( (double)(c)->vp[1] )
#define rkglVPWidth(c)  ( (double)(c)->vp[2] )
#define rkglVPHeight(c) ( (double)(c)->vp[3] )

#define rkglVPAspect(c) ( rkglVPWidth(c) / rkglVPHeight(c) )

/* view volume */

__EXPORT void rkglVVLoad(rkglCamera *c);
__EXPORT void rkglVVGet(rkglCamera *c);

__EXPORT void rkglVVInit(void);

__EXPORT void rkglOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
__EXPORT void rkglFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
__EXPORT void rkglPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far);
__EXPORT void rkglOrthoScale(rkglCamera *c, double scale, GLdouble near, GLdouble far);
__EXPORT void rkglFrustumScale(rkglCamera *c, double scale, GLdouble near, GLdouble far);

/* camera angle */

__EXPORT void rkglCALoad(rkglCamera *c);
__EXPORT void rkglCAGet(rkglCamera *c);

__EXPORT void rkglCAInit(void);
__EXPORT void rkglCAAlign(rkglCamera *c);

__EXPORT void rkglCASet(rkglCamera *c, double x, double y, double z, double pan, double tilt, double roll);
__EXPORT void rkglCAPTR(rkglCamera *c, double pan, double tilt, double roll);
__EXPORT void rkglCALockonPTR(rkglCamera *c, double pan, double tilt, double roll);
__EXPORT void rkglCARotate(rkglCamera *c, double angle, double x, double y, double z);
__EXPORT void rkglCALockonRotate(rkglCamera *c, double angle, double x, double y, double z);

__EXPORT void rkglCAMove(rkglCamera *c, double x, double y, double z);
__EXPORT void rkglCARelMove(rkglCamera *c, double x, double y, double z);

__EXPORT void rkglCALookAt(rkglCamera *c, GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);

__EXPORT void rkglCARelMoveLeft(rkglCamera *cam, double d);
__EXPORT void rkglCARelMoveRight(rkglCamera *cam, double d);
__EXPORT void rkglCARelMoveUp(rkglCamera *cam, double d);
__EXPORT void rkglCARelMoveDown(rkglCamera *cam, double d);
__EXPORT void rkglCAZoomIn(rkglCamera *cam, double d);
__EXPORT void rkglCAZoomOut(rkglCamera *cam, double d);

__EXPORT void rkglCATiltUp(rkglCamera *cam, double angle);
__EXPORT void rkglCATiltDown(rkglCamera *cam, double angle);
__EXPORT void rkglCAPanLeft(rkglCamera *cam, double angle);
__EXPORT void rkglCAPanRight(rkglCamera *cam, double angle);

__EXPORT void rkglCAAngleUp(rkglCamera *cam, double angle);
__EXPORT void rkglCAAngleDown(rkglCamera *cam, double angle);
__EXPORT void rkglCARoundLeft(rkglCamera *cam, double angle);
__EXPORT void rkglCARoundRight(rkglCamera *cam, double angle);

/*
void rkglCameraFRead(FILE *fp, rkglCamera *cam);
*/

__END_DECLS

#endif /* __RKGL_CAMERA_H__ */
