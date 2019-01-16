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

void rkglBGFog(rkglCamera *c, double density);

/* viewport */

void rkglVPCreate(rkglCamera *c, GLint x, GLint y, GLsizei w, GLsizei h);
void rkglVPLoad(rkglCamera *c);
void rkglVPGet(rkglCamera *c);

#define rkglVPOX(c)     ( (double)(c)->vp[0] )
#define rkglVPOY(c)     ( (double)(c)->vp[1] )
#define rkglVPWidth(c)  ( (double)(c)->vp[2] )
#define rkglVPHeight(c) ( (double)(c)->vp[3] )

#define rkglVPAspect(c) ( rkglVPWidth(c) / rkglVPHeight(c) )

/* view volume */

void rkglVVLoad(rkglCamera *c);
void rkglVVGet(rkglCamera *c);

void rkglVVInit(void);

void rkglOrtho(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
void rkglFrustum(rkglCamera *c, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
void rkglPerspective(rkglCamera *c, GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far);
void rkglOrthoScale(rkglCamera *c, double scale, GLdouble near, GLdouble far);
void rkglFrustumScale(rkglCamera *c, double scale, GLdouble near, GLdouble far);

/* camera angle */

void rkglCALoad(rkglCamera *c);
void rkglCAGet(rkglCamera *c);

void rkglCAInit(void);
void rkglCAAlign(rkglCamera *c);

void rkglCASet(rkglCamera *c, double x, double y, double z, double pan, double tilt, double roll);
void rkglCAPTR(rkglCamera *c, double pan, double tilt, double roll);
void rkglCALockonPTR(rkglCamera *c, double pan, double tilt, double roll);
void rkglCARotate(rkglCamera *c, double angle, double x, double y, double z);
void rkglCALockonRotate(rkglCamera *c, double angle, double x, double y, double z);

void rkglCAMove(rkglCamera *c, double x, double y, double z);
void rkglCARelMove(rkglCamera *c, double x, double y, double z);

void rkglCALookAt(rkglCamera *c, GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);

/*
void rkglCameraFRead(FILE *fp, rkglCamera *cam);
*/

__END_DECLS

#endif /* __RKGL_CAMERA_H__ */
