#include "gameoflife/gameoflife.hpp"

GameOfLife::GameOfLife(int rows, int cols) : matrix(rows, cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix.set(i, j, (i + j) % 2);
    }
  }
}
