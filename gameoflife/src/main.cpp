#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"
#include <ratio>
#include <chrono>
#include <ctime>
#include <SDL2/SDL.h>
#include <iostream>

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Window *window;
  SDL_Renderer *renderer;
  if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_SetWindowTitle(window, "Game of life");

  GameOfLife gameoflife(GRID_HEIGHT, GRID_WIDTH);
  UpdateManager manager;
  manager.last_update_timestamp = clock();

  SDL_bool quit = SDL_FALSE;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = SDL_TRUE;
        break;
      }
    }

    auto dt = clock() - manager.last_update_timestamp;
    if (1000 * dt / CLOCKS_PER_SEC > manager.update_rate_ms) {
      manager.last_update_timestamp = clock();
      gameoflife.update();
    }

    draw(renderer, gameoflife);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
