#pragma once

#include <SDL2/SDL.h>

#include "utilscpp/colors.hpp"
#include "utilscpp/mousepointer.hpp"

void draw_circle(SDL_Renderer *renderer, int centre_x, int centre_y, int radius,
                 SDL_Color color);
void draw(SDL_Renderer *renderer, const MousePointer &mpointer);
