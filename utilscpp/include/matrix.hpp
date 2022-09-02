#pragma once

#include <cstdint>
#include <utility>
#include <vector>

using std::vector;

class Matrix {
public:
  int rows;
  int cols;

  Matrix(int rows, int cols);

  bool get(int i, int j) const;
  void turn_on(int i, int j);
  void turn_off(int i, int j);
  void set(int i, int j, bool val);
  bool are_valid_coords(int i, int j) const;

private:
  vector<vector<std::int64_t>> grid;
};
