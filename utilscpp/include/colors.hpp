#pragma once

#include <SDL2/SDL.h>

namespace Color {
const SDL_Color BARELY_BLACK = {22, 22, 22, 255};
const SDL_Color DARK_GREY = {44, 44, 44, 255};
const SDL_Color WHITE = {255, 255, 255, 255};
const SDL_Color RED = {255, 0, 0, 255};
const SDL_Color YELLOW = {255, 255, 0, 255};
const SDL_Color GREEN = {0, 255, 0, 255};
const SDL_Color BLUE = {0, 0, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color GRID_BACKGROUND = BARELY_BLACK;
const SDL_Color GRID_LINE = DARK_GREY;
const SDL_Color GRID_CELL_ALIVE = WHITE;
const SDL_Color ANT_QUEEN = WHITE;
const SDL_Color ANT_WORKER = BLUE;
const SDL_Color ANT_REPRODUCTOR = RED;
const SDL_Color ANT_SOLDIER = GREEN;
};  // namespace Color
