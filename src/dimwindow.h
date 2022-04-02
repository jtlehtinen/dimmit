#pragma once

#include <Windows.h>

typedef struct {
  int x, y;
} Int2;

typedef struct {
  float r, g, b, a;
} Color;

COLORREF color_to_colorref(Color color);
Color colorref_to_color(COLORREF colorref);

typedef struct {
  HWND handle;
} DimWindow;

DimWindow dim_window_create(Int2 position, Int2 size);
void dim_window_destroy(DimWindow* window);

void dim_window_set_color(DimWindow* window, Color color);
