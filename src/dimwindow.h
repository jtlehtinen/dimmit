#pragma once

#include <Windows.h>

typedef struct {
  int x, y;
} Int2;

typedef struct {
  HWND handle;
} DimWindow;

DimWindow dim_window_create(Int2 position, Int2 size);
void dim_window_destroy(DimWindow window);

void dim_window_set_alpha(DimWindow window, float alpha);
