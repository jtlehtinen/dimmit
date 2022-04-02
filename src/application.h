#pragma once

#include "dimwindow.h"

typedef struct {
  DimWindow* dim_windows; // stretchy buffer
} Application;

void application_initialize(Application* app);
void application_deinitialize(Application* app);

void application_create_dim_windows(Application* app);
void application_destroy_dim_windows(Application* app);

void application_test_alpha(Application* app);