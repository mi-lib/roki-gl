/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * roki_glx - GLX wrapper (on X11/OpenGL)
 */

#ifndef __RKGL_GLX_H__
#define __RKGL_GLX_H__

#include <zx11/zxinput.h>
#include <roki_gl/roki_gl.h>
#include <GL/glx.h>

__BEGIN_DECLS

__ROKI_GL_EXPORT GLXContext rkgl_ctx;

__ROKI_GL_EXPORT int rkglInitGLX(void);
__ROKI_GL_EXPORT void rkglExitGLX(void);

__ROKI_GL_EXPORT Window rkglWindowCreateGLX(zxWindow *parent, int x, int y, int w, int h, const char *title);
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

__ROKI_GL_EXPORT void rkglWindowAddEventGLX(Window win, long event);

#define rkglWindowKeyEnableGLX(win) \
  rkglWindowAddEventGLX( (win), KeyPressMask | KeyReleaseMask )
#define rkglWindowMouseEnableGLX(win) \
  rkglWindowAddEventGLX( (win), ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask )

/* default callback functions */

__ROKI_GL_EXPORT void rkglReshapeGLX(rkglCamera *cam, int w, int h, double vvwidth, double vvnear, double vvfar);
__ROKI_GL_EXPORT int rkglKeyFuncGLX(rkglCamera *cam, double dl, double da);
__ROKI_GL_EXPORT void rkglMouseFuncGLX(rkglCamera *cam, int event, double dl);
__ROKI_GL_EXPORT void rkglMouseDragFuncGLX(rkglCamera *cam);

/* buffer-to-image conversion */

__ROKI_GL_EXPORT void rkglReadRGBImage(zxImage *img);
__ROKI_GL_EXPORT void rkglReadDepthImage(zxImage *img);
__ROKI_GL_EXPORT void rkglReadRGBImageGLX(Window win, zxImage *img);

__END_DECLS

#endif /* __RKGL_GLX_H__ */
