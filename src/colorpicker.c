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
      DWORD a = (DWORD)clamp(atoi(buf), 0, 255);

      cc->rgbResult = RGBA(r, g, b, a);
    }
  }

  return 0;
}

COLORREF choose_color(COLORREF initial_color, HWND parent_window) {
  kColorSubmitMessage = RegisterWindowMessageW(COLOROKSTRINGW);

  static DWORD custom_colors[16] = {0};

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
