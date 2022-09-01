#pragma once

#include "utilscpp/matrix.hpp"

class GameOfLife {
 public:
  GameOfLife(int rows, int cols);
  Matrix matrix;
};
