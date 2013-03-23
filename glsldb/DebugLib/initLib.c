/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "initLib.h"

#include <stdio.h>

#include "../GL/gl.h"
#include "../GL/glext.h"
#include "../utils/dbgprint.h"
#ifdef _WIN32
#include "../GL/wglext.h"
#include "trampolines.h"
#endif /* _WIN32 */


#ifdef _WIN32

/*
 * ::openEvents
 */
BOOL openEvents(HANDLE *outEvtDebugee, HANDLE *outEvtDebugger) {
#define EVENT_NAME_LEN (32)
#define SHMEM_NAME_LEN (64)

    wchar_t eventName[EVENT_NAME_LEN];
    DWORD processId = 0;

    /* Sanity checks. */
    if ((outEvtDebugee == NULL) || (outEvtDebugger == NULL)) {
        return TRUE;
    }

    /* TODO: Possible hazard when using multiple instances simultanously. */
    processId = GetCurrentProcessId();

    _snwprintf(eventName, EVENT_NAME_LEN, L"%udbgee", processId);
    if ((*outEvtDebugee = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName)) 
            == NULL) {
        dbgPrint(DBGLVL_ERROR, "OpenEvent(\"%s\") failed: %u.\n",
		          eventName, GetLastError());
        return FALSE;
    }

    _snwprintf(eventName, EVENT_NAME_LEN, L"%udbgr", processId);
    if ((*outEvtDebugger = OpenEventW(EVENT_ALL_ACCESS, FALSE, eventName)) 
            == NULL) {
        dbgPrint(DBGLVL_ERROR, "OpenEvent(\"%s\") failed: %u.\n",
		         eventName, GetLastError());
        return FALSE;
    }

    return TRUE;
#undef EVENT_NAME_LEN
}


/*
 * ::openSharedMemory
 */
BOOL openSharedMemory(HANDLE *outShMem, void **outBaseAddr, const int size) {
#define SHMEM_NAME_LEN (64)

    char shMemName[SHMEM_NAME_LEN];

    if (!GetEnvironmentVariableA("GLSL_DEBUGGER_SHMID", shMemName, 
            SHMEM_NAME_LEN)) {
        dbgPrint(DBGLVL_ERROR, "Oh Shit! No Shmid! Set GLSL_DEBUGGER_SHMID.\n");
        return FALSE;
    }

    /* This creates a non-inheritable shared memory mapping! */
    *outShMem = OpenFileMappingA(FILE_MAP_WRITE, FALSE, shMemName);
    if ((*outShMem == NULL) || (*outShMem == INVALID_HANDLE_VALUE)) {
        dbgPrint(DBGLVL_ERROR, "Opening of shared mem segment \"%s\" failed: %u.\n", 
            shMemName, GetLastError());
        return FALSE;
    }
    
    /* FILE_MAP_WRITE implies read */
    *outBaseAddr = MapViewOfFile(*outShMem, FILE_MAP_WRITE, 0, 0, size);
    if (*outBaseAddr == NULL) {
        dbgPrint(DBGLVL_ERROR, "View mapping of shared mem segment \"%s\" failed: %u.\n",
            shMemName, GetLastError());
        CloseHandle(*outShMem);
        return FALSE;
    }

    return TRUE;
#undef SHMEM_NAME_LEN
}


/*
 * ::closeEvents
 */
BOOL closeEvents(HANDLE hEvtDebugee, HANDLE hEvtDebugger) {
    BOOL retval = TRUE;
    
    if (hEvtDebugee != NULL) {
        if (!CloseHandle(hEvtDebugee)) {
            dbgPrint(DBGLVL_ERROR, "CloseEvent(%u) failed: %u.\n", hEvtDebugee,
                GetLastError());
            retval = FALSE;
        }
        hEvtDebugee = NULL;
    }
    
    if (hEvtDebugger != NULL) {
        if (!CloseHandle(hEvtDebugger)) {
            dbgPrint(DBGLVL_ERROR, "CloseEvent(%u) failed: %u.\n", hEvtDebugger,
                GetLastError());
            retval = FALSE;
        }
        hEvtDebugger = NULL;
    }

    return retval;
}


/*
 * ::closeSharedMemory
 */
BOOL closeSharedMemory(HANDLE hShMem, void *baseAddr) {
    BOOL retval = TRUE;

    if (baseAddr != NULL) {
        if (!UnmapViewOfFile(baseAddr)) {
            dbgPrint(DBGLVL_ERROR, "View unmapping of shared mem segment failed: %u\n", 
                GetLastError());
            retval = FALSE;
        }
        baseAddr = NULL;
    }
    
    if ((hShMem != NULL) && (hShMem != INVALID_HANDLE_VALUE)) {
        if (!CloseHandle(hShMem)) {
            dbgPrint(DBGLVL_ERROR, "Closing handle of shared mem segment failed: %u\n", 
                GetLastError());
            retval = FALSE;
        }
        hShMem = INVALID_HANDLE_VALUE;
    }

    return retval;
}


/** Window class for GL context window. */
static const char *INITCTX_WNDCLASS_NAME = "GLSLDEVIL DEBUGLIB WINDOW";


/*
 * ::createGlInitContext
 */
BOOL createGlInitContext(GlInitContext *outCtx) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    GLuint pixelFormat = 0;
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0 };
    WNDCLASSEXA wndClass;
    
    if (outCtx == NULL) {
        return FALSE;
    }

    outCtx->hWnd = NULL;
    outCtx->hDC = NULL;
    outCtx->hRC = NULL;

    /* Register window class, if not yet available. */
    if (!GetClassInfoExA(hInst, INITCTX_WNDCLASS_NAME, &wndClass)) {
        ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_CLASSDC;
        wndClass.lpfnWndProc = DefWindowProc;
        wndClass.hInstance = hInst;
        wndClass.lpszClassName = INITCTX_WNDCLASS_NAME;

        if (!RegisterClassExA(&wndClass)) {
            dbgPrint(DBGLVL_ERROR, "Registering window class for GL detours initialisation "
                "failed: %u\n", GetLastError());
            return FALSE;
        }
    }

    /* Create window. */
    if ((outCtx->hWnd = CreateWindowExA(WS_EX_APPWINDOW, 
            INITCTX_WNDCLASS_NAME, "", WS_POPUP, 0, 0, 1, 1, NULL, NULL, hInst,
            NULL)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "Creating window for GL detours initialisation failed:\n",
            GetLastError());
        releaseGlInitContext(outCtx);
        return FALSE;
    }

    /* Create OpenGL context. */
    outCtx->hDC = GetDC(outCtx->hWnd);

    if ((pixelFormat = ChoosePixelFormat(outCtx->hDC, &pfd)) == 0) {
        dbgPrint(DBGLVL_ERROR, "ChoosePixelFormat failed: %u\n", GetLastError());
        releaseGlInitContext(outCtx);
        return FALSE;
    }
    if (!SetPixelFormat(outCtx->hDC, pixelFormat, &pfd)) {
        dbgPrint(DBGLVL_ERROR, "SetPixelFormat failed: %u\n", GetLastError());
        releaseGlInitContext(outCtx);
        return FALSE;
    }

    if ((outCtx->hRC = wglCreateContext(outCtx->hDC)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "wglCreateContext failed: %u\n", GetLastError());
        releaseGlInitContext(outCtx);
        return FALSE;
    }
    if (!wglMakeCurrent(outCtx->hDC, outCtx->hRC)) {
        dbgPrint(DBGLVL_ERROR, "wglMakeCurrent failed: %u\n", GetLastError());
        releaseGlInitContext(outCtx);
        return FALSE;
    }

    return TRUE;
}


/*
 * ::releaseGlInitContext
 */
BOOL releaseGlInitContext(GlInitContext *ctx) {
    BOOL retval = TRUE;
    BOOL (APIENTRYP myMakeCurrent)(HDC, HGLRC) = (OrigwglMakeCurrent != NULL) 
        ? OrigwglMakeCurrent : wglMakeCurrent;
    BOOL (APIENTRYP myDeleteContext)(HGLRC) = (OrigwglDeleteContext != NULL)
        ? OrigwglDeleteContext : wglDeleteContext;

    if (ctx == NULL) {
        dbgPrint(DBGLVL_WARNING, "'ctx' must not be NULL when calling releaseGlInitContext\n");
        return FALSE;
    }

    myMakeCurrent(NULL, NULL);

    if (ctx->hRC != NULL) {
        if (!myDeleteContext(ctx->hRC)) {
            dbgPrint(DBGLVL_ERROR, "wglDeleteContext failed: %u\n", GetLastError());
            retval = FALSE;
        }
    }

    if ((ctx->hWnd != NULL) && (ctx->hDC != NULL)) {
        if (!ReleaseDC(ctx->hWnd, ctx->hDC)) {
            dbgPrint(DBGLVL_ERROR, "ReleaseDC failed: %u\n", GetLastError());
            retval = FALSE;
        }
    }

    if (ctx->hWnd != NULL) {
        if (!DestroyWindow(ctx->hWnd)) {
            dbgPrint(DBGLVL_ERROR, "DestroyWindow failed: %u\n", GetLastError());
            retval = FALSE;
        }
    }

    ctx->hRC = NULL;
    ctx->hDC = NULL;
    ctx->hWnd = NULL;

    return retval;
}

#endif /* _WIN32 */
