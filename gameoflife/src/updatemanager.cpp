#include "gameoflife/updatemanager.hpp"

#include <SDL2/SDL.h>

#include <algorithm>
#include <tuple>

#include "gameoflife/config.hpp"
#include "gameoflife/gameoflife.hpp"
#include "utilscpp/mousepointer.hpp"
#include "utilscpp/utils.hpp"

using std::max;
using std::min;
using std::tie;

void handle_events(UpdateManager* manager, SDL_bool* quit,
                   GameOfLife* gameoflife, MousePointer* mpointer,
                   SDL_Rect* source) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        *quit = SDL_TRUE;
        break;
      case SDL_MOUSEBUTTONDOWN:
        int x, y;
        tie(x, y) = translate_coords_from_window_to_rect(
            WINDOW_WIDTH, WINDOW_HEIGHT, Offset(MARGIN), source->w, source->h,
            event.motion.x, event.motion.y);
        tie(x, y) = translate_coords_from_rect_to_texture(
            source->w, source->h, source->x, source->y, WIDTH, HEIGHT, x, y);
        gameoflife->on_click(y / CELL_SIZE, x / CELL_SIZE);
        mpointer->x = x;
        mpointer->y = y;
        manager->should_render = true;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
          manager->should_render = true;
        }
      case SDL_KEYDOWN:
        manager->should_render = true;
        switch (event.key.keysym.sym) {
          case SDLK_UP:
            source->y = max(0, source->y - SCROLL_AMOUNT);
            break;
          case SDLK_DOWN:
            source->y = min(HEIGHT - source->h, source->y + SCROLL_AMOUNT);
            break;
          case SDLK_LEFT:
            source->x = max(0, source->x - SCROLL_AMOUNT);
            break;
          case SDLK_RIGHT:
            source->x = min(WIDTH - source->w, source->x + SCROLL_AMOUNT);
            break;
          case SDLK_o:
            source->w *= ZOOM_FACTOR;
            source->h *= ZOOM_FACTOR;
            source->w = min(source->w, WIDTH);
            source->h = min(source->h, HEIGHT);
            source->x = max(0, source->x - SCROLL_AMOUNT);
            source->x = min(WIDTH - source->w, source->x + SCROLL_AMOUNT);
            source->y = max(0, source->y - SCROLL_AMOUNT);
            source->y = min(HEIGHT - source->h, source->y + SCROLL_AMOUNT);
            break;
          case SDLK_i:
            source->w /= ZOOM_FACTOR;
            source->h /= ZOOM_FACTOR;
            source->w = max(source->w, 1);
            source->h = max(source->h, 1);
            break;
          case SDLK_p:
            manager->is_paused ^= true;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
}
