#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"
#include "utilscpp/mousepointer.hpp"
#include "utilscpp/utils.hpp"

using std::max;
using std::min;

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
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = SDL_TRUE;
          break;
        case SDL_MOUSEBUTTONDOWN:
          int x, y;
          std::tie(x, y) = translate_coords_from_window_to_rect(
              WINDOW_WIDTH, WINDOW_HEIGHT, Offset(MARGIN), source.w, source.h,
              event.motion.x, event.motion.y);
          std::tie(x, y) = translate_coords_from_rect_to_texture(
              source.w, source.h, source.x, source.y, WIDTH, HEIGHT, x, y);
          gameoflife.on_click(y / CELL_SIZE, x / CELL_SIZE);
          mpointer.x = x;
          mpointer.y = y;
          manager.should_render = true;
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_ENTER) {
            manager.should_render = true;
          }
        case SDL_KEYDOWN:
          manager.should_render = true;
          switch (event.key.keysym.sym) {
            case SDLK_UP:
              source.y = max(0, source.y - SCROLL_OFFSET);
              break;
            case SDLK_DOWN:
              source.y = min(HEIGHT - source.h, source.y + SCROLL_OFFSET);
              break;
            case SDLK_LEFT:
              source.x = max(0, source.x - SCROLL_OFFSET);
              break;
            case SDLK_RIGHT:
              source.x = min(WIDTH - source.w, source.x + SCROLL_OFFSET);
              break;
            case SDLK_o:
              source.w *= ZOOM_FACTOR;
              source.h *= ZOOM_FACTOR;
              source.w = min(source.w, WIDTH);
              source.h = min(source.h, HEIGHT);
              source.x = max(0, source.x - SCROLL_OFFSET);
              source.x = min(WIDTH - source.w, source.x + SCROLL_OFFSET);
              source.y = max(0, source.y - SCROLL_OFFSET);
              source.y = min(HEIGHT - source.h, source.y + SCROLL_OFFSET);
              break;
            case SDLK_i:
              source.w /= ZOOM_FACTOR;
              source.h /= ZOOM_FACTOR;
              source.w = max(source.w, 1);
              source.h = max(source.h, 1);
              break;
            case SDLK_p:
              manager.is_paused ^= true;
              break;
            default:
              break;
          }
        default:
          break;
      }
    }

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
