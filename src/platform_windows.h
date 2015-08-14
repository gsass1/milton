//    Milton Paint
//    Copyright (C) 2015  Sergio Gonzalez
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


// The returns value mean different things, but other than that, we're ok
#ifdef _MSC_VER
#define snprintf sprintf_s
#endif

#define HEAP_BEGIN_ADDRESS NULL
#ifndef NDEBUG
#undef HEAP_BEGIN_ADDRESS
#define HEAP_BEGIN_ADDRESS (LPVOID)(1024LL * 1024 * 1024 * 1024)
#endif
#define platform_allocate(total_memory_size) VirtualAlloc(HEAP_BEGIN_ADDRESS, \
                                                          (total_memory_size),\
                                                          MEM_COMMIT | MEM_RESERVE, \
                                                          PAGE_READWRITE) \
#define platform_deallocate(pointer) VirtualFree((pointer), 0, MEM_RELEASE)

func void milton_fatal(char* message);
void win32_log(char *format, ...);
#define milton_log win32_log

void win32_log(char *format, ...)
{
    char message[ 128 ];

    int num_bytes_written = 0;

    va_list args;

    assert ( format );

    va_start( args, format );

    num_bytes_written = _vsnprintf(message, sizeof( message ) - 1, format, args);

    if ( num_bytes_written > 0 )
    {
	OutputDebugStringA( message );
    }

    va_end( args );
}

void platform_load_gl_func_pointers()
{
    GLenum glew_err = glewInit();

    if (glew_err != GLEW_OK)
    {
        milton_log("glewInit failed with error: %s\nExiting.\n",
                   glewGetErrorString(glew_err));
        exit(EXIT_FAILURE);
    }
}

#include "win32_wacom_defines.h"
struct TabletState_s {
    HWND window;
    // Window dimensions:
    int32_t width;
    int32_t height;
    BITMAPINFO bitmap_info;

    HINSTANCE wintab_handle;
    WacomAPI wacom;
    POINT wacom_pt_old;
    POINT wacom_pt_new;
    UINT wacom_prs_old;
    UINT wacom_prs_new;
    UINT wacom_prs_max;
    HCTX wacom_ctx;
    int wacom_num_attached_devices;
};

#include "win32_wacom.h"

void platform_wacom_init(TabletState* tablet_state, SDL_Window* window)
{
    SDL_SysWMinfo wminfo;
    SDL_bool wminfo_ok = SDL_GetWindowWMInfo(window, &wminfo);
    if (!wminfo_ok)
    {
        milton_fatal("Could not get WM info from SDL");
    }
    tablet_state->window = wminfo.info.win.window;

    if (!win32_wacom_load_wintab(tablet_state))
    {
        OutputDebugStringA("No wintab.\n");
    }

    win32_wacom_get_context(tablet_state);
}

int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow
        )
{
    milton_main();
}
