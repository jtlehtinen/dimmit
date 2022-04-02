#pragma once

#include <Windows.h>

typedef struct {
  int x, y;
} Int2;

#define RGBA(r, g, b, a)  ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))
#define GetAValue(rgba)   (LOBYTE((rgba)>>24))

typedef struct {
  HWND handle;
} DimWindow;

DimWindow dim_window_create(Int2 position, Int2 size);
void dim_window_destroy(DimWindow* window);

void dim_window_set_color(DimWindow* window, COLORREF color);
