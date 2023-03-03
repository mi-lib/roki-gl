/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_glx - X11/OpenGL wrapper
 */

#ifndef __RKGL_GLX_H__
#define __RKGL_GLX_H__

#include <zx11/zxinput.h>
#include <roki-gl/rkgl_input.h>
#include <GL/glx.h>

__BEGIN_DECLS

__EXPORT GLXContext rkgl_ctx;

__EXPORT int rkglInitGLX(void);
__EXPORT void rkglExitGLX(void);

__EXPORT Window rkglWindowCreateGLX(zxWindow *parent, int x, int y, int w, int h, const char *title);
#define rkglWindowOpenGLX(win)    XMapWindow( zxdisplay, win )
#define rkglWindowCloseGLX(win)   XUnmapWindow( zxdisplay, win )
#define rkglWindowDestroyGLX(win) XDestroyWindow( zxdisplay, win )

#define rkglWindowMoveGLX(win,x,y)   XMoveWindow( zxdisplay, win, x, y )
#define rkglWindowResizeGLX(win,w,h) XResizeWindow( zxdisplay, win, w, h )

#define rkglWindowActivateGLX(win)    glXMakeCurrent( zxdisplay, win, rkgl_ctx )
#define rkglWindowSwapBuffersGLX(win) glXSwapBuffers( zxdisplay, win )

#define rkglFlushGLX() do{\
  glFlush();\
  zxFlush();\
} while(0)

__EXPORT void rkglWindowAddEventGLX(Window win, long event);

#define rkglWindowKeyEnableGLX(win) \
  rkglWindowAddEventGLX( (win), KeyPressMask | KeyReleaseMask )
#define rkglWindowMouseEnableGLX(win) \
  rkglWindowAddEventGLX( (win), ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask )

/* default callback functions */

__EXPORT void rkglReshapeGLX(rkglCamera *cam, int w, int h, double vvwidth, double vvnear, double vvfar);
__EXPORT int rkglKeyFuncGLX(rkglCamera *cam, double dl, double da);
__EXPORT void rkglMouseFuncGLX(rkglCamera *cam, int event, double dl);
__EXPORT void rkglMouseDragFuncGLX(rkglCamera *cam);

__END_DECLS

#endif /* __RKGL_GLX_H__ */
