#include "dimwindow.h"
#include <assert.h>

// @TODO: Remove this when you figure out nice way to pass
// the color to the window procedure. SetWindowLongPtr sucks
// since you need to pass pointer... And I don't want to alloc
// DimWindow from the heap or introduce dependency that would
// provide a stable pointer.
static Color global_color = {.r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.5f};

static float clamp(float value, float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

static BYTE tobyte(float normalized_value) {
  float value = clamp(normalized_value, 0.0f, 1.0f);
  return (BYTE)(value * 255.0f + 0.5f);
}

static LRESULT CALLBACK dim_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  switch (message) {
    case WM_PAINT: {
      HBRUSH brush = CreateSolidBrush(color_to_colorref(global_color));

      PAINTSTRUCT ps;
      HDC dc = BeginPaint(window, &ps);

      RECT cr;
      GetClientRect(window, &cr);
      FillRect(dc, &cr, brush);

      EndPaint(window, &ps);

      DeleteObject(brush);
      return 0;
    }

    default:
      return DefWindowProcW(window, message, wparam, lparam);
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

COLORREF color_to_colorref(Color color) {
  return RGB(tobyte(color.r), tobyte(color.g), tobyte(color.b));
}

Color colorref_to_color(COLORREF colorref) {
  return (Color){
    .r = (float)GetRValue(colorref) / 255.0f,
    .g = (float)GetGValue(colorref) / 255.0f,
    .b = (float)GetBValue(colorref) / 255.0f,
    .a = 1.0f,
  };
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
  assert(handle);

  SetLayeredWindowAttributes(handle, RGB(0, 0, 0), 128, LWA_ALPHA);

  return (DimWindow){
    .handle = handle,
  };
}

void dim_window_destroy(DimWindow* window) {
  if (window->handle) {
    DestroyWindow(window->handle);
  }
}

void dim_window_set_color(DimWindow* window, Color color) {
  global_color = color;
  SetLayeredWindowAttributes(window->handle, RGB(0,0,0), tobyte(color.a), LWA_ALPHA);
  InvalidateRect(window->handle, NULL, FALSE);
}
