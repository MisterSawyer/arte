#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0xa00
#endif
#ifdef UNICODE
#undef UNICODE
#endif
#include <winsock2.h>
#include <windows.h>

namespace arte::native
{
    using device_context = HDC;
    using console_handle = HANDLE;
    using window_handle = HWND;
    using process_handle = HANDLE;
}