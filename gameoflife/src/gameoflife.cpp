#include "gameoflife/gameoflife.hpp"

#include <functional>

#include "gameoflife/config.hpp"
#include "utilscpp/matrix.hpp"

GameOfLife::GameOfLife(int rows, int cols,
                       std::function<bool(int, int)> generator)
    : matrix(rows, cols) {
  rule = {2, 3, 3, 3};
  neighborhood_diffs = {
      {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
  };
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix.set(i, j, generator(i, j));
    }
  }
}

GameOfLife::GameOfLife(): matrix(0, 0) {
  rule = {};
  neighborhood_diffs = {};
}

void GameOfLife::update() {
  const Matrix prev_matrix = matrix;
  for (int i = 0; i < matrix.rows; i++) {
    for (int j = 0; j < matrix.cols; j++) {
      int cnt = get_neighborhood_count(i, j, prev_matrix);
      bool is_alive = prev_matrix.get(i, j);
      bool new_state = false;
      if (is_alive && rule.survival_min <= cnt && cnt <= rule.survival_max) {
        new_state = true;
      } else if (!is_alive && rule.birth_min <= cnt && cnt <= rule.birth_max) {
        new_state = true;
      }
      matrix.set(i, j, new_state);
    }
  }
}

int GameOfLife::get_neighborhood_count(int i, int j,
                                       const Matrix &matrix) const {
  int ans = 0;
  for (const auto &diff : neighborhood_diffs) {
    int y = i + diff.first;
    int x = j + diff.second;
    if (Config::SHOULD_MODULE_INDEXES) {
      x = (x + matrix.cols) % matrix.cols;
      y = (y + matrix.rows) % matrix.rows;
    }
    if (!matrix.are_valid_coords(y, x)) continue;
    ans += matrix.get(y, x);
  }
  return ans;
}

void GameOfLife::on_click(int i, int j) { matrix.toggle(i, j); }
