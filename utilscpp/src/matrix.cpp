#include "utilscpp/matrix.hpp"

#include <cstdint>
#include <ostream>
#include <vector>

using std::vector;

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
  int num_of_i32 = cols / 32 + (cols % 32 > 0);
  grid = vector<vector<int>>(rows, vector<int>(num_of_i32, 0));
}

bool Matrix::get(int i, int j) const {
  return (grid[i][j / 32] >> (j % 32)) & 1;
}

void Matrix::turn_on(int i, int j) { grid[i][j / 32] |= (1 << (j % 32)); }

void Matrix::turn_off(int i, int j) { grid[i][j / 32] &= ~(1 << (j % 32)); }

void Matrix::set(int i, int j, bool val) {
  if (val)
    turn_on(i, j);
  else
    turn_off(i, j);
}

void Matrix::toggle(int i, int j) { grid[i][j / 32] ^= (1 << (j % 32)); }

bool Matrix::are_valid_coords(int i, int j) const {
  return !(i < 0 || i >= rows || j < 0 || j >= cols);
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
  for (int i = 0; i < matrix.rows; i++) {
    for (int j = 0; j < matrix.cols; j++) os << matrix.get(i, j);
    os << std::endl;
  }
  return os;
}
