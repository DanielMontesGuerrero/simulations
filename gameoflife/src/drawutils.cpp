#include "gameoflife/drawutils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "gameoflife/config.hpp"
#include "gameoflife/updatemanager.hpp"
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

void draw(SDL_Renderer* renderer, SDL_Texture** texture, const UpdateManager& manager) {
  auto dt = clock() - manager.gamte_started_timestamp;
  auto current_runtime = dt / CLOCKS_PER_SEC;
  TTF_Font* sans_font = TTF_OpenFont("./ubuntu.ttf", 24);
  if(!sans_font){
    std::cerr << "Error opening font" << std::endl;
  }
  auto message_txt = "Runtime: "
    + std::to_string(current_runtime / 60)
    + ":"
    + std::to_string(current_runtime % 60);
  SDL_Color font_color = Color::WHITE;
  SDL_Surface* surface_message =
    TTF_RenderText_Solid(sans_font, message_txt.c_str(), font_color); 
  *texture = SDL_CreateTextureFromSurface(renderer, surface_message);
  SDL_FreeSurface(surface_message);
}

void draw(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect source,
          SDL_Rect dest, GameHandler* gamehandler,
          const MousePointer& mpointer, const UpdateManager& manager) {

  // draw status
  SDL_SetRenderTarget(renderer, nullptr);
  SDL_RenderClear(renderer);
  SDL_Rect ui_dest{Config::MARGIN.left, 2, 120, Config::MARGIN.top - 4};
  SDL_Texture *runtime_texture;
  draw(renderer, &runtime_texture, manager);


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
  SDL_RenderCopy(renderer, runtime_texture, nullptr, &ui_dest);
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(runtime_texture);
}
