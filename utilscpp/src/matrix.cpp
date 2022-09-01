#include "utilscpp/matrix.hpp"
#include <cstdint>
#include <vector>

using std::vector;

typedef std::int64_t lli;

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
  int num_of_i64 = cols / 64 + (cols % 64 > 0);
  grid = vector<vector<lli>>(rows, vector<lli>(num_of_i64, 0));
}

bool Matrix::get(int i, int j) { return (grid[i][j / 64] >> (j % 64)) & 1; }

void Matrix::set(int i, int j, bool x) { grid[i][j / 64] |= (x << (j % 64)); }
