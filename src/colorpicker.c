#include "colorpicker.h"
#include <Commdlg.h>

Color choose_color(Color initial_color, HWND parent_window) {
  // @TODO: need a color picker with alpha...
  static DWORD custom_colors[16] = {0};

  CHOOSECOLORW cc = {
    .lStructSize = sizeof(cc),
    .hwndOwner = parent_window,
    .Flags = CC_RGBINIT | CC_FULLOPEN,
    .lpCustColors = custom_colors,
    .rgbResult = color_to_colorref(initial_color),
  };

  if (ChooseColorW(&cc) == FALSE) {
    return initial_color;
  } else {
    Color color = colorref_to_color(cc.rgbResult);
    color.a = initial_color.a;
    return color;
  }
}
