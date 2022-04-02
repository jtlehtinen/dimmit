#include "dimwindow.h"
#include "colors.h"
#include <assert.h>
#include <stdlib.h>

static LRESULT CALLBACK dim_window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  DimWindow* window = (DimWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
  if (!window) {
    return DefWindowProcW(hwnd, message, wparam, lparam);
  }

  switch (message) {
    case WM_PAINT: {
      SetLayeredWindowAttributes(hwnd, RGB(0,0,0), GetAValue(window->color), LWA_ALPHA);

      HBRUSH brush = CreateSolidBrush(window->color);

      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hwnd, &ps);

      RECT cr;
      GetClientRect(hwnd, &cr);
      FillRect(dc, &cr, brush);

      EndPaint(hwnd, &ps);

      DeleteObject(brush);

      return 0;
    }

    default:
      return DefWindowProcW(hwnd, message, wparam, lparam);
  }
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

DimWindow* dim_window_create(Int2 position, Int2 size, COLORREF color) {
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

  if (!handle) {
    return NULL;
  }

  DimWindow* result = (DimWindow*)malloc(sizeof(DimWindow));
  if (!result) {
    DestroyWindow(handle);
    return NULL;
  }

  result->handle = handle;
  result->color = color;

  SetWindowLongPtrW(result->handle, GWLP_USERDATA, (LONG_PTR)result);
  SetLayeredWindowAttributes(result->handle, RGB(0, 0, 0), GetAValue(color), LWA_ALPHA);

  return result;
}

void dim_window_destroy(DimWindow* window) {
  if (window) {
    if (window->handle) {
      DestroyWindow(window->handle);
    }
    free(window);
  }
}

void dim_window_set_color(DimWindow* window, COLORREF color) {
  window->color = color;
  InvalidateRect(window->handle, NULL, FALSE);
}
