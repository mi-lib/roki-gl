/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glx - X11/OpenGL wrapper
 */

#ifndef __RKGL_GLX_H__
#define __RKGL_GLX_H__

#include <zx11/zxinput.h>
#include <roki-gl/rkgl_camera.h>
#include <GL/glx.h>

__BEGIN_DECLS

extern GLXContext rkgl_ctx;

int rkglInitGLX(void);
void rkglCloseGLX(void);

Window rkglWindowCreateGLX(zxWindow *parent, int x, int y, int w, int h, const char *title);
#define rkglWindowOpenGLX(w)    XMapWindow( zxdisplay, w )
#define rkglWindowCloseGLX(w)   XUnmapWindow( zxdisplay, w )
#define rkglWindowDestroyGLX(w) XDestroyWindow( zxdisplay, w )

#define rkglWindowMoveGLX(win,x,y)   XMoveWindow( zxdisplay, win, x, y )
#define rkglWindowResizeGLX(win,w,h) XResizeWindow( zxdisplay, win, w, h )

#define rkglActivateGLX(w)    glXMakeCurrent( zxdisplay, w, rkgl_ctx )
#define rkglSwapBuffersGLX(w) glXSwapBuffers( zxdisplay, w )

#define rkglFlushGLX() do{\
  glFlush();\
  zxFlush();\
} while(0)

void rkglWindowAddEventGLX(Window win, long event);

#define rkglKeyEnableGLX(win) \
  rkglWindowAddEventGLX( (win), KeyPressMask | KeyReleaseMask )
#define rkglMouseEnableGLX(win) \
  rkglWindowAddEventGLX( (win), ButtonPressMask | ButtonReleaseMask | PointerMotionMask )

/* default callback functions */

void rkglReshapeGLX(rkglCamera *cam, int w, int h, double vvwidth, double vvnear, double vvfar);
int rkglKeyFuncGLX(rkglCamera *cam, KeySym key, int x, int y, double dl, double da);
void rkglMouseFuncGLX(int button, int state, int x, int y);
int rkglMouseDragFuncGLX(rkglCamera *cam, int x, int y);

__END_DECLS

#endif /* __RKGL_GLX_H__ */
