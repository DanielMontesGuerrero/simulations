#pragma once

#include <SDL2/SDL.h>

#include <utility>

#include "gameoflife/gameoflife.hpp"
#include "utilscpp/mousepointer.hpp"

using std::pair;

void draw(SDL_Renderer *renderer, const GameOfLife &gameoflife);

void draw(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect source,
          SDL_Rect dest, const GameOfLife &gameoflife,
          const MousePointer &mpointer);
