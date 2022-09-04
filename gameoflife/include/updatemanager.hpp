#pragma once

#include <SDL2/SDL.h>

#include <ctime>

#include "gameoflife/gameoflife.hpp"
#include "utilscpp/mousepointer.hpp"

struct UpdateManager {
  double update_rate_ms = 500;
  clock_t last_update_timestamp = 0;
  bool is_paused = true;
  bool should_render = true;
};

void handle_events(UpdateManager* manager, SDL_bool* quit,
                   GameOfLife* gameoflife, MousePointer* mpointer,
                   SDL_Rect* source);

