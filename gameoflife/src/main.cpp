#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <ctime>
#include <iostream>

#include "gameoflife/client.hpp"
#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"
#include "utilscpp/mousepointer.hpp"
#include "utilscpp/utils.hpp"

int main(int argc, char **argv) {
  string matrix_config = Config::init(argc, argv);
  srand(time(NULL));

  if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Rect source{0, 0, Config::WINDOW_WIDTH / Config::ZOOM_DEFAULT,
                  Config::WINDOW_HEIGHT / Config::ZOOM_DEFAULT};
  SDL_Rect dest{
      Config::MARGIN.left, Config::MARGIN.top,
      Config::WINDOW_WIDTH - (Config::MARGIN.left + Config::MARGIN.right),
      Config::WINDOW_HEIGHT - (Config::MARGIN.top + Config::MARGIN.bottom)};
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

  GameHandler gamehandler(Config::GRID_HEIGHT, Config::GRID_WIDTH,
                          Config::SHOULD_EXECUTE_LOCALLY, matrix_config);
  UpdateManager manager;
  manager.gamte_started_timestamp = clock();
  manager.last_update_timestamp = clock();
  MousePointer mpointer(0, 0, 5);
  SDL_StopTextInput();

  SDL_bool quit = SDL_FALSE;
  while (!quit) {
    handle_events(&manager, &quit, &gamehandler, &mpointer, &source);

    auto dt = clock() - manager.last_update_timestamp;
    auto time_since_last_update = 1000 * dt / CLOCKS_PER_SEC;
    if (time_since_last_update > manager.update_rate_ms && !manager.is_paused) {
      manager.last_update_timestamp = clock();
      gamehandler.update();
      manager.should_render = true;
    }

    if (manager.should_render) {
      int x, y, w, h;
      std::tie(x, y) = translate_coords_from_rect_to_matrix(
          source.w, source.h, source.x, source.y, Config::WIDTH, Config::HEIGHT,
          0, 0, Config::CELL_SIZE);
      std::tie(w, h) = translate_coords_from_rect_to_matrix(
          source.w, source.h, source.x, source.y, Config::WIDTH, Config::HEIGHT,
          source.w - 1, source.h - 1, Config::CELL_SIZE);
      gamehandler.send_get_message(x, y, w - x, h - y);
      draw(renderer, texture, source, dest, &gamehandler, mpointer, manager);
      manager.should_render = false;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
