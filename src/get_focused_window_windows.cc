// Copyright 2014 Toggl Desktop developers.

#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <time.h>
#include <string>

#include "Poco/UnicodeConverter.h"

#include "./get_focused_window.h"

static const int kFilenameBufferSize = 255;
static const int kTitleBufSize = 500;

int getFocusedWindowInfo(
    std::string *title,
    std::string *filename,
    bool *idle) {
    *title = "";
    *filename = "";
    *idle = false;

    // get window handle
    HWND window_handle = GetForegroundWindow();
    if (!window_handle) {
        *idle = true;
        return 0;
    }

    // get window title
    int length = GetWindowTextLengthW(window_handle);
    if (length) {
        wchar_t buf[kTitleBufSize];
        GetWindowTextW(window_handle, buf, kTitleBufSize);

        std::string utf8("");

#ifndef __CYGWIN__
        // FIXME: does not work with Cygwin
        Poco::UnicodeConverter::toUTF8(buf, utf8);
#endif

        *title = utf8;
    }

    // get process by window handle
    DWORD process_id;
    GetWindowThreadProcessId(window_handle, &process_id);

    // get the filename of another process
    HANDLE ps = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
                            process_id);

    CHAR filename_buffer[kFilenameBufferSize];
    if (GetModuleFileNameExA(ps, 0, filename_buffer, kFilenameBufferSize) > 0) {
        *filename = std::string(filename_buffer);
    }

    CloseHandle(ps);

    return 0;
}
