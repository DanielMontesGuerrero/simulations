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
  bool get(int i, int j);
  void set(int i, int j, bool x);

 private:
  vector<vector<std::int64_t>> grid;
};
