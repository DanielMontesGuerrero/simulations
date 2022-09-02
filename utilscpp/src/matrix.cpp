#include "utilscpp/matrix.hpp"

#include <cstdint>
#include <vector>

using std::vector;

typedef std::int64_t lli;

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
  int num_of_i64 = cols / 64 + (cols % 64 > 0);
  grid = vector<vector<lli>>(rows, vector<lli>(num_of_i64, 0));
}

bool Matrix::get(int i, int j) const {
  return (grid[i][j / 64] >> (j % 64)) & 1;
}

void Matrix::turn_on(int i, int j) { grid[i][j / 64] |= (1 << (j % 64)); }

void Matrix::turn_off(int i, int j) { grid[i][j / 64] &= ~(1 << (j % 64)); }

void Matrix::set(int i, int j, bool val) {
  if (val)
    turn_on(i, j);
  else
    turn_off(i, j);
}

bool Matrix::are_valid_coords(int i, int j) const {
  return !(i < 0 || i >= rows || j < 0 || j >= cols);
}
