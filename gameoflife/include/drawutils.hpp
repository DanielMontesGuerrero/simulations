#pragma once

#include "gameoflife/gameoflife.hpp"
#include <SDL2/SDL.h>
#include <utility>

using std::pair;

void draw_grid(SDL_Renderer *renderer, GameOfLife *gameoflife);

void draw(SDL_Renderer *renderer, GameOfLife *gameoflife);
