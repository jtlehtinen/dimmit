#include "application.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdint.h>

typedef struct {
  HMONITOR handle;
  Int2 position;
  Int2 size;
} Monitor;

static BOOL enumerate_monitors_callback(HMONITOR hmonitor, HDC dc, LPRECT rect, LPARAM lparam) {
  Monitor** monitors = (Monitor**)(lparam);

  Monitor monitor = {
    .handle = hmonitor,
    .position = {
      .x = rect->left,
      .y = rect->top
    },
    .size = {
      .x = rect->right - rect->left,
      .y = rect->bottom - rect->top
    }
  };
  arrput(*monitors, monitor);

  return TRUE;
}

// Result alloced, you need to arrfree().
static Monitor* all_monitors() {
  Monitor* monitors = NULL;
  EnumDisplayMonitors(NULL, NULL, enumerate_monitors_callback, (LPARAM)(&monitors));
  return monitors;
}

void application_initialize(Application* app) {
  application_create_dim_windows(app);
}

void application_deinitialize(Application* app) {
  application_destroy_dim_windows(app);
}

void application_create_dim_windows(Application* app) {
  if (app->dim_windows) {
    application_destroy_dim_windows(app);
  }

  Monitor* monitors = all_monitors();

  int64_t count = arrlen(monitors);
  for (int64_t i = 0; i < count; ++i) {
    Monitor* monitor = monitors + i;
    arrput(app->dim_windows, dim_window_create(monitor->position, monitor->size));
  }

  arrfree(monitors);
}

void application_destroy_dim_windows(Application* app) {
  int64_t count = arrlen(app->dim_windows);
  for (int64_t i = 0; i < count; ++i) {
    dim_window_destroy(app->dim_windows[i]);
  }
  arrfree(app->dim_windows);
  app->dim_windows = NULL;
}
