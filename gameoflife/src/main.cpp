#include <SDL2/SDL.h>

#include <ctime>
#include <iostream>

#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"
#include "utilscpp/mousepointer.hpp"
#include "utilscpp/utils.hpp"

int main() {
  srand(time(NULL));
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Rect source{0, 0, Config::WINDOW_WIDTH / Config::ZOOM_DEFAULT,
                  Config::WINDOW_HEIGHT / Config::ZOOM_DEFAULT};
  SDL_Rect dest{Config::MARGIN, Config::MARGIN,
                Config::WINDOW_WIDTH - 2 * Config::MARGIN,
                Config::WINDOW_HEIGHT - 2 * Config::MARGIN};
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  if (SDL_CreateWindowAndRenderer(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT,
                                  0, &window, &renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, Config::WIDTH,
                              Config::HEIGHT);

  SDL_SetWindowTitle(window, "Game of life");

  GameHandler gamehandler(Config::GRID_HEIGHT, Config::GRID_WIDTH, Config::SHOUD_EXECUTE_LOCALLY);
  UpdateManager manager;
  manager.last_update_timestamp = clock();
  MousePointer mpointer(0, 0, 5);

  SDL_bool quit = SDL_FALSE;
  while (!quit) {
    handle_events(&manager, &quit, &gamehandler, &mpointer, &source);

    auto dt = clock() - manager.last_update_timestamp;
    auto time_since_last_update = 1000 * dt / CLOCKS_PER_SEC;
    if (time_since_last_update > manager.update_rate_ms && !manager.is_paused) {
      manager.last_update_timestamp = clock();
      int x, y, w, h;
      std::tie(x, y) = translate_coords_from_rect_to_matrix(
            source.w, source.h, source.x, source.y, Config::WIDTH,
            Config::HEIGHT, 0, 0, Config::CELL_SIZE);
      std::tie(w, h) = translate_coords_from_rect_to_matrix(
            source.w, source.h, source.x, source.y, Config::WIDTH,
            Config::HEIGHT, source.w, source.h, Config::CELL_SIZE);
      gamehandler.update(x, y, w - x, h - y);
      manager.should_render = true;
    }

    if (manager.should_render) {
      draw(renderer, texture, source, dest, gamehandler, mpointer);
      manager.should_render = false;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
