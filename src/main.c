#include "application.h"

#ifdef DIMMIT_DEBUG
  #include <crtdbg.h>
#endif
#include <Windows.h>
#include <shellapi.h>

const int EXIT_OK = 0;
const int EXIT_FAIL = 1;

#define WM_DIMMIT_NOTIFY_COMMAND (WM_USER + 1)

BOOL add_notification_area_icon(HWND window) {
  NOTIFYICONDATAW data = {
    .cbSize = sizeof(data),
    .hWnd = window,
    .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
    .uCallbackMessage = WM_DIMMIT_NOTIFY_COMMAND,
    .hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1)),
  };

  wcscpy_s(data.szTip, ARRAYSIZE(data.szTip), L"dimmit");
  return Shell_NotifyIconW(NIM_ADD, &data);
}

void remove_notification_area_icon(HWND window) {
  NOTIFYICONDATAW data = {
    .cbSize = sizeof(NOTIFYICONDATAW),
    .hWnd = window
  };
  Shell_NotifyIconW(NIM_DELETE, &data);
}

LRESULT CALLBACK notification_area_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  if (message == WM_CREATE) {
    add_notification_area_icon(window);
    return 0;
  }

  Application* app = (Application*)GetWindowLongPtrW(window, GWLP_USERDATA);

  switch (message) {
    case WM_DESTROY: {
      remove_notification_area_icon(window);
      PostQuitMessage(0);
      return 0;
    }

    case WM_TIMER: {
      if (app) {
        application_test_color(app);
      }
      return 0;
    }

    case WM_DIMMIT_NOTIFY_COMMAND: {
      if (lparam == WM_RBUTTONUP) {
        static BOOL enabled = FALSE;

        const UINT kCmdEnabled = 1;
        const UINT kCmdExit = 255;

        HMENU menu = CreatePopupMenu();

        #define CHECKED(condition) ((condition) ? (UINT)MF_CHECKED : (UINT)MF_UNCHECKED)
        AppendMenuW(menu, CHECKED(enabled), kCmdEnabled, L"Enabled");
        AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(menu, MF_STRING, kCmdExit, L"Exit");
        #undef CHECKED

        SetForegroundWindow(window);

        POINT mouse;
        GetCursorPos(&mouse);
        UINT cmd = (UINT)TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY, mouse.x, mouse.y, 0, window, NULL);

        DestroyMenu(menu);

        if (cmd == kCmdExit) {
          DestroyWindow(window);
        } else if (cmd == kCmdEnabled) {
          enabled = !enabled;
        }
      }
    }

    default:
      return DefWindowProcW(window, message, wparam, lparam);
  }
}

int CALLBACK wWinMain(HINSTANCE instance, HINSTANCE previous_instance, LPWSTR command_line, int show_code) {
#ifdef DIMMIT_DEBUG
  int current_flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  _CrtSetDbgFlag(current_flags | _CRTDBG_LEAK_CHECK_DF);
#endif

  const wchar_t* guid = L"3eddb977-f739-48b3-a4b0-3cae2d885251";
  HANDLE mutex = CreateMutexW(NULL, TRUE, guid);
  if (GetLastError() != ERROR_SUCCESS) {
    return EXIT_FAIL;
  }

  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  WNDCLASSW wc = {
    .lpfnWndProc = notification_area_window_proc,
    .hInstance = instance,
    .lpszClassName = L"notification-area-wc"
  };
  RegisterClassW(&wc);

  HWND notification_area_window = CreateWindowExW(
    WS_EX_TOOLWINDOW,
    wc.lpszClassName,
    L"",
    0,
    0, 0,
    1, 1,
    NULL,
    NULL,
    instance,
    NULL
  );

  if (notification_area_window) {
    Application app = {0};
    application_initialize(&app);

    SetWindowLongPtrW(notification_area_window, GWLP_USERDATA, (LONG_PTR)(&app));

    const UINT_PTR timer = SetTimer(notification_area_window, 0, 100, NULL);

    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

    KillTimer(notification_area_window, timer);
    application_deinitialize(&app);
  }

  ReleaseMutex(mutex);

  return EXIT_OK;
}
