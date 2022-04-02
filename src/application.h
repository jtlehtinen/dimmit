#pragma once

#include "dimwindow.h"

typedef struct {
  DimWindow* dim_windows; // stretchy buffer
  Color color;
  BOOL enabled;
} Application;

void application_initialize(Application* app);
void application_deinitialize(Application* app);

void application_create_dim_windows(Application* app);
void application_destroy_dim_windows(Application* app);

void application_set_enabled(Application* app, BOOL enabled);
void application_set_color(Application* app, Color color);
