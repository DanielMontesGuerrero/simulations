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
  int64_t num_cells_alive;
  int current_iteration;

  GameOfLife();
  GameOfLife(int rows, int cols, std::function<bool(int, int)> generator);
  pair<vector<int>, vector<int>> update();
  void on_click(int i, int j);

 private:
  Rule rule;
  vector<pair<int, int>> neighborhood_diffs;

  int get_neighborhood_count(int i, int j, const Matrix &matrix) const;
  vector<int> get_states_for_every_submatrix(int rows, int cols);
};
