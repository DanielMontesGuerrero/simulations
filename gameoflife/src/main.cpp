#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gameoflife/config.hpp"
#include "gameoflife/drawutils.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/updatemanager.hpp"

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

  GameOfLife gameoflife(GRID_HEIGHT, GRID_WIDTH, [](int i, int j){
      return rand() % 2;
      });
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
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_UP:
              source.y = std::max(0, source.y - SCROLL_OFFSET);
              break;
            case SDLK_DOWN:
              source.y = std::min(HEIGHT - source.h, source.y + SCROLL_OFFSET);
              break;
            case SDLK_LEFT:
              source.x = std::max(0, source.x - SCROLL_OFFSET);
              break;
            case SDLK_RIGHT:
              source.x = std::min(WIDTH - source.w, source.x + SCROLL_OFFSET);
              break;
            case SDLK_o:
              source.w *= ZOOM_FACTOR;
              source.h *= ZOOM_FACTOR;
              break;
            case SDLK_i:
              source.w /= ZOOM_FACTOR;
              source.h /= ZOOM_FACTOR;
              break;
            default:
              break;
          }
        default:
          break;
      }
    }

    auto dt = clock() - manager.last_update_timestamp;
    if (1000 * dt / CLOCKS_PER_SEC > manager.update_rate_ms) {
      manager.last_update_timestamp = clock();
      gameoflife.update();
    }

    draw(renderer, texture, source, dest, gameoflife);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
