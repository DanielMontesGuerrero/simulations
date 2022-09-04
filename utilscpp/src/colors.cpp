#include "utilscpp/colors.hpp"

SDL_Color grid_background_color() {
  return {22, 22, 22, 255};  // Barely Black
}
SDL_Color grid_line_color() {
  return {44, 44, 44, 255};  // Dark grey
}
SDL_Color grid_cell_alive_color() {
  return {255, 255, 255, 255};  // White
}

SDL_Color red() {
  return {255, 0, 0, 255};
}
