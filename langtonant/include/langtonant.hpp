#pragma once

#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "utilscpp/matrix.hpp"

using std::map;
using std::pair;
using std::vector;

struct Rule {
  int survival_min;
  int survival_max;
  int birth_min;
  int birth_max;
};

class Ant {
 public:
  int x, y;
  int dir;
  int type;
  int age;

  Ant(int x, int y);
  void try_move(const map<pair<int, int>, Ant*>& coord_to_ant_map, int xlim,
                int ylim, bool should_retry);
  pair<int, int> update(Matrix* matrix,
                        map<pair<int, int>, Ant*>* coord_to_ant_map, int xlim,
                        int ylim);
  pair<int, int> check_collisions(map<pair<int, int>, Ant*>* coord_to_ant_map,
                                  int xlim, int ylim);
  void manage_queen2queen_collision(Ant* ant);
  pair<int, int> maybe_add_ant(
      map<pair<int, int>, Ant*>* coord_to_ant_map, Ant* ant, int xlim,
      int ylim);
};

class LangtonAntGame {
 public:
  vector<Ant> ants;
  Matrix matrix;
  int64_t num_ants;
  int current_iteration;

  LangtonAntGame();
  LangtonAntGame(int rows, int cols, std::function<bool(int, int)> generator);
  void update();
  void on_click(int i, int j);

 private:
  Rule rule;
  vector<pair<int, int>> neighborhood_diffs;

  int get_neighborhood_count(int i, int j, const Matrix& matrix) const;
};
