#include "gameoflife/drawutils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

#include "gameoflife/config.hpp"
#include "utilscpp/colors.hpp"
#include "utilscpp/drawers.hpp"
#include "utilscpp/matrix.hpp"

void draw(SDL_Renderer* renderer, GameHandler* gamehandler) {
  if (gamehandler->is_executed_locally) {
    draw(renderer, gamehandler->gameoflife.matrix, 0, 0);
  } else {
    while (!gamehandler->pending_updates.empty()) {
      auto submatrix = gamehandler->pending_updates.front();
      gamehandler->pending_updates.pop();
      if (gamehandler->pending_updates.empty()) {
        draw(renderer, submatrix.matrix, submatrix.x, submatrix.y);
        gamehandler->pending_updates.push(submatrix);
        break;
      }
    }
  }
}

void draw(SDL_Renderer* renderer, const Matrix& matrix, int offset_x,
          int offset_y) {
  // draw cells
  SDL_SetRenderDrawColor(renderer, Color::GRID_CELL_ALIVE.r,
                         Color::GRID_CELL_ALIVE.g, Color::GRID_CELL_ALIVE.b,
                         Color::GRID_CELL_ALIVE.a);
  for (int i = 0; i < matrix.rows; i++) {
    for (int j = 0; j < matrix.cols; j++) {
      if (!matrix.get(i, j)) continue;
      SDL_Rect aux = {
          .x = (j + offset_x) * Config::CELL_SIZE,
          .y = (i + offset_y) * Config::CELL_SIZE,
          .w = Config::CELL_SIZE,
          .h = Config::CELL_SIZE,
      };
      SDL_RenderFillRect(renderer, &aux);
    }
  }

  // draw grid lines.
  if (Config::SHOULD_DRAW_GRID_LINES) {
    SDL_SetRenderDrawColor(renderer, Color::GRID_LINE.r, Color::GRID_LINE.g,
                           Color::GRID_LINE.b, Color::GRID_LINE.a);

    for (int x = 0; x < 1 + Config::GRID_WIDTH * Config::CELL_SIZE;
         x += Config::CELL_SIZE) {
      SDL_RenderDrawLine(renderer, x, 0, x, Config::HEIGHT);
    }

    for (int y = 0; y < 1 + Config::GRID_HEIGHT * Config::CELL_SIZE;
         y += Config::CELL_SIZE) {
      SDL_RenderDrawLine(renderer, 0, y, Config::WIDTH, y);
    }
  }
}

void draw(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect source,
          SDL_Rect dest, GameHandler* gamehandler,
          const MousePointer& mpointer) {
  // clear texture
  SDL_SetRenderTarget(renderer, texture);
  SDL_SetRenderDrawColor(renderer, Color::GRID_BACKGROUND.r,
                         Color::GRID_BACKGROUND.g, Color::GRID_BACKGROUND.b,
                         Color::GRID_BACKGROUND.a);
  SDL_RenderClear(renderer);

  // draw grid
  draw(renderer, gamehandler);

  if (Config::DEBUG) {
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
