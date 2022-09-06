#pragma once

#include <SDL2/SDL.h>

namespace Color {
const SDL_Color BARELY_BLACK = {22, 22, 22, 255};
const SDL_Color DARK_GREY = {44, 44, 44, 255};
const SDL_Color WHITE = {255, 255, 255, 255};
const SDL_Color RED = {255, 0, 0, 255};
const SDL_Color GRID_BACKGROUND = BARELY_BLACK;
const SDL_Color GRID_LINE = DARK_GREY;
const SDL_Color GRID_CELL_ALIVE = WHITE;
};  // namespace Color
