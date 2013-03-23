#ifndef __glx_h__
#define __glx_h__

/*
** The contents of this file are subject to the GLX Public License Version 1.0
** (the "License"). You may not use this file except in compliance with the
** License. You may obtain a copy of the License at Silicon Graphics, Inc.,
** attn: Legal Services, 2011 N. Shoreline Blvd., Mountain View, CA 94043
** or at http://www.sgi.com/software/opensource/glx/license.html.
**
** Software distributed under the License is distributed on an "AS IS"
** basis. ALL WARRANTIES ARE DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY
** IMPLIED WARRANTIES OF MERCHANTABILITY, OF FITNESS FOR A PARTICULAR
** PURPOSE OR OF NON- INFRINGEMENT. See the License for the specific
** language governing rights and limitations under the License.
**
** The Original Software is GLX version 1.2 source code, released February,
** 1999. The developer of the Original Software is Silicon Graphics, Inc.
** Those portions of the Subject Software created by Silicon Graphics, Inc.
** are Copyright (c) 1991-9 Silicon Graphics, Inc. All Rights Reserved.
**
** $Header: //sw/main/drivers/OpenGL/win/glx/include/glx.h#21 $
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Names for attributes to glXGetConfig.
 */
#define GLX_USE_GL              1       /* support GLX rendering */
#define GLX_BUFFER_SIZE         2       /* depth of the color buffer */
#define GLX_LEVEL               3       /* level in plane stacking */
#define GLX_RGBA                4       /* true if RGBA mode */
#define GLX_DOUBLEBUFFER        5       /* double buffering supported */
#define GLX_STEREO              6       /* stereo buffering supported */
#define GLX_AUX_BUFFERS         7       /* number of aux buffers */
#define GLX_RED_SIZE            8       /* number of red component bits */
#define GLX_GREEN_SIZE          9       /* number of green component bits */
#define GLX_BLUE_SIZE           10      /* number of blue component bits */
#define GLX_ALPHA_SIZE          11      /* number of alpha component bits */
#define GLX_DEPTH_SIZE          12      /* number of depth bits */
#define GLX_STENCIL_SIZE        13      /* number of stencil bits */
#define GLX_ACCUM_RED_SIZE      14      /* number of red accum bits */
#define GLX_ACCUM_GREEN_SIZE    15      /* number of green accum bits */
#define GLX_ACCUM_BLUE_SIZE     16      /* number of blue accum bits */
#define GLX_ACCUM_ALPHA_SIZE    17      /* number of alpha accum bits */

/*
 * Error return values from glXGetConfig.  Success is indicated by
 * a value of 0.
 */
#define GLX_BAD_SCREEN                  1  /* screen # is bad */
#define GLX_BAD_ATTRIBUTE               2  /* attribute to get is bad */
#define GLX_NO_EXTENSION                3  /* no glx extension on server */
#define GLX_BAD_VISUAL                  4  /* visual # not known by GLX */
#define GLX_BAD_CONTEXT                 5
#define GLX_BAD_VALUE                   6
#define GLX_BAD_ENUM                    7

/*
 * Names for attributes to glXGetClientString.
 */
#define GLX_VENDOR                      0x1
#define GLX_VERSION                     0x2
#define GLX_EXTENSIONS                  0x3

/*
 * GLX 1.2 and higher are defined in glxext.h!
 */

/* return value of glXGetProcAddress() */
typedef void (*__GLXextFuncPtr)(void);

/*
 * GLX resources.
 */
typedef XID GLXContextID;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef XID GLXPbuffer;
typedef XID GLXWindow;
typedef XID GLXFBConfigID;

/*
 * GLXContext is a pointer to opaque data.
 */
typedef struct __GLXcontextRec *GLXContext;

/*
 * GLXFBConfig is a pointer to opaque data.
 */
typedef struct __GLXFBConfigRec *GLXFBConfig;


/**********************************************************************/

/*
 * GLX 1.0 functions.
 */
extern XVisualInfo* glXChooseVisual(Display *dpy, int screen,
                                    int *attrib_list);

extern void glXCopyContext(Display *dpy, GLXContext src,
                           GLXContext dst, unsigned long mask);

extern GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis,
                                   GLXContext share_list, Bool direct);

extern GLXPixmap glXCreateGLXPixmap(Display *dpy, XVisualInfo *vis,
                                    Pixmap pixmap);

extern void glXDestroyContext(Display *dpy, GLXContext ctx);

extern void glXDestroyGLXPixmap(Display *dpy, GLXPixmap pix);

extern int glXGetConfig(Display *dpy, XVisualInfo *vis,
                        int attrib, int *value);

extern GLXContext glXGetCurrentContext(void);

extern GLXDrawable glXGetCurrentDrawable(void);

extern Bool glXIsDirect(Display *dpy, GLXContext ctx);

extern Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable,
                           GLXContext ctx);

extern Bool glXQueryExtension(Display *dpy, int *error_base, int *event_base);

extern Bool glXQueryVersion(Display *dpy, int *major, int *minor);

extern void glXSwapBuffers(Display *dpy, GLXDrawable drawable);

extern void glXUseXFont(Font font, int first, int count, int list_base);

extern void glXWaitGL(void);

extern void glXWaitX(void);


#ifndef GLX_VERSION_1_1
#define GLX_VERSION_1_1         1
/*
 * GLX 1.1 functions.
 */
extern const char *glXGetClientString(Display *dpy, int name);

extern const char *glXQueryServerString(Display *dpy, int screen, int name);

extern const char *glXQueryExtensionsString(Display *dpy, int screen);
#endif

/*
 * GLX 1.2 and higher are defined in glxext.h!
 */

/*
 * Definition of the function prototype, as required by the OpenGL ABI.
 */
#ifndef GLX_VERSION_1_4
extern __GLXextFuncPtr glXGetProcAddress (const GLubyte *procName);
#endif

/**********************************************************************/

/*
 * ARB_get_proc_address
 *
 *  Definition of the function prototype, as required by the OpenGL ABI.
 */
#ifndef GLX_ARB_get_proc_address
extern __GLXextFuncPtr glXGetProcAddressARB(const GLubyte *procName);
#endif

/**********************************************************************/

/*** Should these go here, or in another header? */
/*
 * GLX Events
 */
typedef struct {
    int event_type;             /* GLX_DAMAGED or GLX_SAVED */
    int draw_type;              /* GLX_WINDOW or GLX_PBUFFER */
    unsigned long serial;       /* # of last request processed by server */
    Bool send_event;            /* true if this came for SendEvent request */
    Display *display;           /* display the event was read from */
    GLXDrawable drawable;       /* XID of Drawable */
    unsigned int buffer_mask;   /* mask indicating which buffers are affected */
    unsigned int aux_buffer;    /* which aux buffer was affected */
    int x, y;
    int width, height;
    int count;                  /* if nonzero, at least this many more */
} GLXPbufferClobberEvent;

typedef union __GLXEvent {
    GLXPbufferClobberEvent glxpbufferclobber;
    long pad[24];
} GLXEvent;

#ifndef GLX_GLXEXT_LEGACY
#include "./glxext.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* !__glx_h__ */
