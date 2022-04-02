#include "colorpicker.h"
#include <Commdlg.h>

static Color win32_choose_color(Color initial_color, HWND parent_window) {
  static DWORD custom_colors[16] = {0};

  CHOOSECOLORW cc = {
    .lStructSize = sizeof(cc),
    .hwndOwner = parent_window,
    .Flags = CC_RGBINIT | CC_FULLOPEN | CC_ENABLETEMPLATE,
    .lpCustColors = custom_colors,
    .rgbResult = color_to_colorref(initial_color),
    .lpTemplateName = L"DimmitChooseColor",
  };

  if (ChooseColorW(&cc) == FALSE) {
    return initial_color;
  } else {
    Color color = colorref_to_color(cc.rgbResult);
    color.a = initial_color.a;
    return color;
  }
}

Color choose_color(Color initial_color, HWND parent_window) {
  return win32_choose_color(initial_color, parent_window);
}
