#include "colorpicker.h"
#include "colors.h"
#include <stdlib.h>
#include <stdio.h>
#include <Commdlg.h>

#define COLOR_ALPHAACCEL  800
#define COLOR_ALPHA       801

static UINT kColorSubmitMessage = 0;

static int clamp(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

static UINT_PTR CALLBACK color_picker_hook(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  CHOOSECOLOR* cc = (CHOOSECOLOR*)lparam;

  if (message == WM_INITDIALOG) {

    char buf[512] = {0};
    sprintf_s(buf, sizeof(buf), "%d", GetAValue(cc->rgbResult));
    SetDlgItemTextA(window, COLOR_ALPHA, buf);

  } else if (message == kColorSubmitMessage) {

    char buf[512] = {0};
    UINT count = GetDlgItemTextA(window, COLOR_ALPHA, buf, sizeof(buf));

    if (count < sizeof(buf)) {
      COLORREF color = cc->rgbResult;

      DWORD r = GetRValue(color);
      DWORD g = GetGValue(color);
      DWORD b = GetBValue(color);
      DWORD a = (DWORD)clamp(atoi(buf), 0, 230);

      cc->rgbResult = RGBA(r, g, b, a);
    }
  }

  return 0;
}

COLORREF choose_color(COLORREF initial_color, HWND parent_window) {
  kColorSubmitMessage = RegisterWindowMessageW(COLOROKSTRINGW);

  static DWORD custom_colors[16] = {
    COLOR_RGB_BLACK,
    COLOR_RGB_RED,
    COLOR_RGB_GREEN,
    COLOR_RGB_BLUE,

    COLOR_RGB_ORANGE,
    COLOR_RGB_BEIGE,
    COLOR_RGB_BISQUE,
    COLOR_RGB_FIRE_BRICK,

    COLOR_RGB_CRIMSON,
    COLOR_RGB_CORAL,
    COLOR_RGB_BROWN,
    COLOR_RGB_BLANCHED_ALMOND,

    COLOR_RGB_BLUE_VIOLET,
    COLOR_RGB_AZURE,
    COLOR_RGB_DARK_KHAKI,
    COLOR_RGB_DARK_OLIVE_GREEN
  };

  CHOOSECOLORW cc = {
    .lStructSize = sizeof(cc),
    .hwndOwner = parent_window,
    .Flags = CC_RGBINIT | CC_FULLOPEN | CC_ENABLETEMPLATE | CC_ENABLEHOOK,
    .lpCustColors = custom_colors,
    .rgbResult = initial_color,
    .lpTemplateName = L"DimmitChooseColor",
    .lpfnHook = color_picker_hook,
  };

  return ChooseColorW(&cc) == FALSE ? initial_color : cc.rgbResult;
}
