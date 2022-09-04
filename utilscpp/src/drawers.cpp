#include "utilscpp/drawers.hpp"

void draw(SDL_Renderer* renderer, const MousePointer& mpointer) {
  draw_circle(renderer, mpointer.x, mpointer.y, mpointer.r, red());
}

void draw_circle(SDL_Renderer* renderer, int centre_x, int centre_y, int radius,
                 SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  const int diameter = radius * 2;

  int x = (radius - 1);
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = (tx - diameter);

  while (x >= y) {
    //  Each of the following renders an octant of the circle
    SDL_RenderDrawPoint(renderer, centre_x + x, centre_y - y);
    SDL_RenderDrawPoint(renderer, centre_x + x, centre_y + y);
    SDL_RenderDrawPoint(renderer, centre_x - x, centre_y - y);
    SDL_RenderDrawPoint(renderer, centre_x - x, centre_y + y);
    SDL_RenderDrawPoint(renderer, centre_x + y, centre_y - x);
    SDL_RenderDrawPoint(renderer, centre_x + y, centre_y + x);
    SDL_RenderDrawPoint(renderer, centre_x - y, centre_y - x);
    SDL_RenderDrawPoint(renderer, centre_x - y, centre_y + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}
