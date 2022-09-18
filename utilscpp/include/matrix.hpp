#pragma once

#include <ostream>
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
  void toggle(int i, int j);
  bool are_valid_coords(int i, int j) const;

 private:
  vector<vector<int>> grid;
};

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
