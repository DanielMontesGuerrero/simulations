#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "utilscpp/matrix.hpp"

using std::pair;
using std::vector;

struct Rule {
  int survival_min;
  int survival_max;
  int birth_min;
  int birth_max;
};

class GameOfLife {
 public:
  Matrix matrix;

  GameOfLife(int rows, int cols, std::function<bool(int, int)> generator);
  void update();

 private:
  Rule rule;
  vector<pair<int, int>> neighborhood_diffs;

  int get_neighborhood_count(int i, int j, const Matrix &matrix) const;
};
