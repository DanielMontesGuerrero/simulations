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
                   GameHandler* gamehandler, MousePointer* mpointer,
                   SDL_Rect* source) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        gamehandler->close_server();
        *quit = SDL_TRUE;
        break;
      case SDL_MOUSEBUTTONDOWN:
        int x, y;
        tie(x, y) = translate_coords_from_window_to_rect(
            Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Offset(Config::MARGIN),
            source->w, source->h, event.motion.x, event.motion.y);
        tie(x, y) = translate_coords_from_rect_to_texture(
            source->w, source->h, source->x, source->y, Config::WIDTH,
            Config::HEIGHT, x, y);
        gamehandler->on_click(y / Config::CELL_SIZE, x / Config::CELL_SIZE);
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
            source->y = max(0, source->y - Config::SCROLL_AMOUNT);
            break;
          case SDLK_DOWN:
            source->y = min(Config::HEIGHT - source->h,
                            source->y + Config::SCROLL_AMOUNT);
            break;
          case SDLK_LEFT:
            source->x = max(0, source->x - Config::SCROLL_AMOUNT);
            break;
          case SDLK_RIGHT:
            source->x = min(Config::WIDTH - source->w,
                            source->x + Config::SCROLL_AMOUNT);
            break;
          case SDLK_o:
            source->w *= Config::ZOOM_FACTOR;
            source->h *= Config::ZOOM_FACTOR;
            source->w = min(source->w, Config::WIDTH);
            source->h = min(source->h, Config::HEIGHT);
            source->x = max(0, source->x - Config::SCROLL_AMOUNT);
            source->x = min(Config::WIDTH - source->w,
                            source->x + Config::SCROLL_AMOUNT);
            source->y = max(0, source->y - Config::SCROLL_AMOUNT);
            source->y = min(Config::HEIGHT - source->h,
                            source->y + Config::SCROLL_AMOUNT);
            break;
          case SDLK_i:
            source->w /= Config::ZOOM_FACTOR;
            source->h /= Config::ZOOM_FACTOR;
            source->w = max(source->w, 1);
            source->h = max(source->h, 1);
            break;
          case SDLK_p:
            manager->is_paused ^= true;
            gamehandler->toggle_pause();
            break;
          case SDLK_f:
            manager->update_rate_ms =
                max(manager->update_rate_ms - Config::SPEED_FACTOR,
                    Config::MIN_UPDATE_RATE_MS);
            gamehandler->increase_update_rate();
            break;
          case SDLK_s:
            manager->update_rate_ms =
                min(manager->update_rate_ms + Config::SPEED_FACTOR,
                    Config::MAX_UPDATE_RATE_MS);
            gamehandler->decrease_update_rate();
            break;
          case SDLK_r:
            gamehandler->send_get_message();
            manager->should_render = true;
            break;
          case SDLK_u:
            gamehandler->send_update_message();
            manager->should_render = true;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
}
