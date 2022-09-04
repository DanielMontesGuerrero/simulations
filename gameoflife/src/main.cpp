#include <SDL2/SDL.h>

#include <ctime>
#include <iostream>

#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"
#include "utilscpp/mousepointer.hpp"

int main() {
  srand(time(NULL));
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Rect source{0, 0, WINDOW_WIDTH / ZOOM_DEFAULT,
                  WINDOW_HEIGHT / ZOOM_DEFAULT};
  SDL_Rect dest{MARGIN, MARGIN, WINDOW_WIDTH - 2 * MARGIN,
                WINDOW_HEIGHT - 2 * MARGIN};
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window,
                                  &renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

  SDL_SetWindowTitle(window, "Game of life");

  GameOfLife gameoflife(GRID_HEIGHT, GRID_WIDTH,
                        [](int i, int j) { return rand() % 2; });
  UpdateManager manager;
  manager.last_update_timestamp = clock();
  MousePointer mpointer(0, 0, 5);

  SDL_bool quit = SDL_FALSE;
  while (!quit) {
    handle_events(&manager, &quit, &gameoflife, &mpointer, &source);

    auto dt = clock() - manager.last_update_timestamp;
    auto time_since_last_update = 1000 * dt / CLOCKS_PER_SEC;
    if (time_since_last_update > manager.update_rate_ms && !manager.is_paused) {
      manager.last_update_timestamp = clock();
      gameoflife.update();
      manager.should_render = true;
    }

    if (manager.should_render) {
      draw(renderer, texture, source, dest, gameoflife, mpointer);
      manager.should_render = false;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
