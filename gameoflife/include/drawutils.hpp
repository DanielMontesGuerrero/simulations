#pragma once

#include <SDL2/SDL.h>

#include <utility>

#include "gameoflife/gameoflife.hpp"

using std::pair;

void draw_grid(SDL_Renderer *renderer, const GameOfLife &gameoflife);

void draw(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect source,
          SDL_Rect dest, const GameOfLife &gameoflife);
