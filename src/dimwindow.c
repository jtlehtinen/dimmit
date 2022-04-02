#include "dimwindow.h"

static Int2 window_size(HWND window) {
  RECT r;
  GetClientRect(window, &r);
  return (Int2){
    .x = r.right - r.left,
    .y = r.bottom - r.top
  };
}

static LRESULT CALLBACK dim_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  if (message == WM_PAINT) {
    Int2 size = window_size(window);

    PAINTSTRUCT ps;
    HDC dc = BeginPaint(window, &ps);
    PatBlt(dc, 0, 0, size.x, size.y, BLACKNESS);
    EndPaint(window, &ps);
    return 0;
  }

  return DefWindowProcW(window, message, wparam, lparam);
}

static register_window_class() {
  static BOOL registered = FALSE;

  if (!registered) {
    WNDCLASSW wc = {
      .lpfnWndProc = dim_window_proc,
      .hInstance = GetModuleHandleW(NULL),
      .lpszClassName = L"dim-wc"
    };
    RegisterClassW(&wc);

    registered = TRUE;
  }
}

DimWindow dim_window_create(Int2 position, Int2 size) {
  register_window_class();

  DWORD ws = WS_POPUP | WS_VISIBLE;
  DWORD wsx =  WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT;

  HWND handle = CreateWindowExW(
    wsx,
    L"dim-wc",
    L"",
    ws,
    position.x, position.y,
    size.x, size.y,
    NULL,
    NULL,
    GetModuleHandleW(NULL),
    NULL
  );

  SetLayeredWindowAttributes(handle, RGB(0, 0, 0), 128, LWA_ALPHA);

  DimWindow result = {
    .handle = handle,
  };

  return result;
}

void dim_window_destroy(DimWindow window) {
  if (window.handle) {
    DestroyWindow(window.handle);
  }
}
