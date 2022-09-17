#pragma once

#include <SDL2/SDL.h>

#include <utility>

#include "gameoflife/gamehandler.hpp"
#include "utilscpp/mousepointer.hpp"
#include "utilscpp/matrix.hpp"

using std::pair;

void draw(SDL_Renderer *renderer, GameHandler &gamehandler);

void draw(SDL_Renderer *renderer, const Matrix &matrix, int offset_x, int offset_y);

void draw(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect source,
          SDL_Rect dest, GameHandler &gamehandler,
          const MousePointer &mpointer);
