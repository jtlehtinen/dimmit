#include "colorpicker.h"
#include <stdlib.h>
#include <Commdlg.h>

#define COLOR_ALPHAACCEL  800
#define COLOR_ALPHA       801

static UINT kColorSubmitMessage = 0;

static UINT_PTR CALLBACK color_picker_hook(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  CHOOSECOLOR* cc = (CHOOSECOLOR*)lparam;

  if (message == kColorSubmitMessage) {
    char buf[512] = {0};
    UINT count = GetDlgItemTextA(window, COLOR_ALPHA, buf, ARRAYSIZE(buf));

    if (count < ARRAYSIZE(buf)) {
      COLORREF color = cc->rgbResult;
      DWORD red = GetRValue(color);
      DWORD green = GetGValue(color);
      DWORD blue = GetBValue(color);
      DWORD alpha = ((DWORD)atoi(buf)) & 0xff;

      cc->rgbResult = RGBA(red, green, blue, alpha);
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
