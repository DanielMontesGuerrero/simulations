#include "gameoflife/drawutils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "gameoflife/config.hpp"
#include "utilscpp/colors.hpp"
#include "utilscpp/drawers.hpp"

void draw(SDL_Renderer* renderer, const GameOfLife& gameoflife) {
  // draw cells
  SDL_SetRenderDrawColor(renderer, grid_cell_alive_color().r,
                         grid_cell_alive_color().g, grid_cell_alive_color().b,
                         grid_cell_alive_color().a);
  for (int i = 0; i < gameoflife.matrix.rows; i++) {
    for (int j = 0; j < gameoflife.matrix.cols; j++) {
      if (!gameoflife.matrix.get(i, j)) continue;
      SDL_Rect aux = {
          .x = j * CELL_SIZE,
          .y = i * CELL_SIZE,
          .w = CELL_SIZE,
          .h = CELL_SIZE,
      };
      SDL_RenderFillRect(renderer, &aux);
    }
  }

  // draw grid lines.
  if (SHOULD_DRAW_GRID_LINES) {
    SDL_SetRenderDrawColor(renderer, grid_line_color().r, grid_line_color().g,
                           grid_line_color().b, grid_line_color().a);

    for (int x = 0; x < 1 + GRID_WIDTH * CELL_SIZE; x += CELL_SIZE) {
      SDL_RenderDrawLine(renderer, x, 0, x, HEIGHT);
    }

    for (int y = 0; y < 1 + GRID_HEIGHT * CELL_SIZE; y += CELL_SIZE) {
      SDL_RenderDrawLine(renderer, 0, y, WIDTH, y);
    }
  }
}

void draw(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect source,
          SDL_Rect dest, const GameOfLife& gameoflife,
          const MousePointer& mpointer) {
  // clear texture
  SDL_SetRenderTarget(renderer, texture);
  SDL_SetRenderDrawColor(renderer, grid_background_color().r,
                         grid_background_color().g, grid_background_color().b,
                         grid_background_color().a);
  SDL_RenderClear(renderer);

  // draw grid
  draw(renderer, gameoflife);

  if (DEBUG) {
    // draw mouse pointer
    draw(renderer, mpointer);
  }

  // copy selection from texture to the screen
  SDL_SetRenderTarget(renderer, nullptr);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, &source, &dest);
  SDL_RenderPresent(renderer);
}
