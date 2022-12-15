#include "langtonant/langtonant.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <utility>

#include "langtonant/config.hpp"
#include "utilscpp/matrix.hpp"

using std::map;
using std::pair;

Ant::Ant(int x, int y) : x(x), y(y) {
  age = rand() % Config::ANT_DEATH_AGE;
  dir = rand() % 4;
  auto random_prob = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  for (int i = 0; i < 4; i++) {
    /* std::cerr << random_prob << ' ' << Config::ANTS_DENSITIES[i] <<
     * std::endl; */
    if (random_prob <= Config::ANTS_DENSITIES[i]) {
      type = i;
      /* std::cerr << i << std::endl; */
      break;
    }
  }
}

void Ant::try_move(const map<pair<int, int>, Ant*>& coord_to_ant_map, int xlim,
                   int ylim, bool should_retry = true) {
  /* std::cerr << "Ant::try_move" << std::endl; */
  int nxt_x = x;
  int nxt_y = y;
  switch (dir) {
    case 0:
      nxt_y = y + 1;
      break;
    case 1:
      nxt_x = x + 1;
      break;
    case 2:
      nxt_y = y - 1;
      break;
    case 3:
      nxt_x = x - 1;
      break;
    default:
      break;
  }
  nxt_x = (nxt_x + xlim) % xlim;
  nxt_y = (nxt_y + ylim) % ylim;
  if (coord_to_ant_map.count({nxt_x, nxt_y})) {
    if (should_retry) {
      dir = (dir + rand() % 3 + 1) % 4;
      try_move(coord_to_ant_map, xlim, ylim, false);
    }
    return;
  }
  x = nxt_x;
  y = nxt_y;
}

pair<int, int> Ant::update(Matrix* matrix,
                           map<pair<int, int>, Ant*>* coord_to_ant_map,
                           int xlim, int ylim) {
  /* std::cerr << "Ant::update" << std::endl; */
  age++;
  if (matrix->get(y, x)) {
    dir = (dir + 1) % 4;
    matrix->set(y, x, 0);
  } else {
    dir = (dir - 1 + 4) % 4;
    matrix->set(y, x, 1);
  }
  try_move(*coord_to_ant_map, xlim, ylim);
  return check_collisions(coord_to_ant_map, xlim, ylim);
}

pair<int, int> Ant::check_collisions(
    map<pair<int, int>, Ant*>* coord_to_ant_map, int xlim, int ylim) {
  // queen
  if (type == 0) {
    vector<pair<int, int>> queen_to_queen_collision_diffs = {
        {-1, 0}, {0, -1}, {0, 1}, {1, 0}};
    for (auto diff : queen_to_queen_collision_diffs) {
      pair<int, int> coord = {(x + diff.first + xlim) % xlim,
                              (y + diff.second + ylim) % ylim};
      if (coord_to_ant_map->count(coord) &&
          (*coord_to_ant_map)[coord]->type == 0) {
        manage_queen2queen_collision((*coord_to_ant_map)[coord]);
      }
    }

    // check ant birth
    vector<pair<int, int>> queen_to_reproductor_collision_diffs;
    switch (dir) {
      case 0:
        queen_to_reproductor_collision_diffs = {{1, 0}};
        break;
      case 1:
        queen_to_reproductor_collision_diffs = {{0, -1}};
        break;
      case 2:
        queen_to_reproductor_collision_diffs = {{-1, 0}};
        break;
      case 3:
        queen_to_reproductor_collision_diffs = {{0, 1}};
        break;
      default:
        break;
    }
    for (auto diff : queen_to_reproductor_collision_diffs) {
      pair<int, int> coord = {(x + diff.first + xlim) % xlim,
                              (y + diff.second + ylim) % ylim};
      if (coord_to_ant_map->count(coord) &&
          (*coord_to_ant_map)[coord]->type == 2) {
        return maybe_add_ant(coord_to_ant_map, this, xlim, ylim);
      }
    }
  } else if (type == 2) {
    vector<pair<int, int>> reproductor_to_queen_collisions_diffs;
    switch (dir) {
      case 0:
        reproductor_to_queen_collisions_diffs = {{-1, 0}};
        break;
      case 1:
        reproductor_to_queen_collisions_diffs = {{0, 1}};
        break;
      case 2:
        reproductor_to_queen_collisions_diffs = {{1, 0}};
        break;
      case 3:
        reproductor_to_queen_collisions_diffs = {{0, -1}};
        break;
      default:
        break;
    }
    for (auto diff : reproductor_to_queen_collisions_diffs) {
      pair<int, int> coord = {(x + diff.first + xlim) % xlim,
                              (y + diff.second + ylim) % ylim};
      if (coord_to_ant_map->count(coord) &&
          (*coord_to_ant_map)[coord]->type == 0) {
        return maybe_add_ant(coord_to_ant_map, (*coord_to_ant_map)[coord], xlim,
                             ylim);
      }
    }
  }
  return {-1, -1};
}

pair<int, int> Ant::maybe_add_ant(map<pair<int, int>, Ant*>* coord_to_ant_map,
                                  Ant* ant, int xlim, int ylim) {
  vector<pair<int, int>> neighborhood_diffs = {
      {-1, 0}, {0, -1}, {0, 1}, {1, 0}};
  for (auto diff : neighborhood_diffs) {
    pair<int, int> coord((ant->x + diff.first + xlim) % xlim,
                         (ant->y + diff.second + ylim) % ylim);
    if (!coord_to_ant_map->count(coord)) return coord;
  }
  return {-1, -1};
}

void Ant::manage_queen2queen_collision(Ant* ant) {
  auto prob_this_ant_survives = 0.5;
  if ((age - ant->age) > Config::QUEEN_BATTLE_MAX_AGE_DIFFERENCE) {
    prob_this_ant_survives = Config::QUEEN_BATTLE_DISTADVANTAGE_PROB;
  } else if ((ant->age - age) > Config::QUEEN_BATTLE_MAX_AGE_DIFFERENCE) {
    prob_this_ant_survives = 1 - Config::QUEEN_BATTLE_DISTADVANTAGE_PROB;
  }
  auto random_prob = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  if (random_prob <= prob_this_ant_survives) {
    ant->age = Config::ANT_DEATH_AGE;
  } else {
    age = Config::ANT_DEATH_AGE;
  }
}

LangtonAntGame::LangtonAntGame(int rows, int cols,
                               std::function<bool(int, int)> generator)
    : matrix(rows, cols) {
  int cnt[4] = {0, 0, 0, 0};
  num_ants = 0;
  current_iteration = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      auto random_prob =
          static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      if (random_prob <= Config::ANTS_DISTRIBUTION) {
        ants.push_back(Ant(rand() % cols, rand() % rows));
        cnt[ants[i].type]++;
      }
      /* matrix.set(i, j, generator(i, j)); */
      /* if (matrix.get(i, j)) num_cells_alive++; */
    }
  }
  for (int i = 0; i < 4; i++) {
    /* std::cerr << "cnt: " << cnt[i] << std::endl; */
    num_ants += cnt[i];
  }
  /* std::cerr << "tot_ants: " << num_ants << " tot_cells: " << cols * rows <<
   * std::endl; */
}

LangtonAntGame::LangtonAntGame() : matrix(0, 0) {
  rule = {};
  neighborhood_diffs = {};
}

void LangtonAntGame::update() {
  current_iteration++;
  map<pair<int, int>, Ant*> coord_to_ant_map;
  vector<Ant> ants_copy;
  for (auto& ant : ants) {
    auto coord =
        ant.update(&matrix, &coord_to_ant_map, matrix.cols, matrix.rows);
    if (coord.first != -1 && coord.second != -1) {
      ants_copy.push_back(Ant(coord.first, coord.second));
    }
    coord_to_ant_map[{ant.x, ant.y}] = &ant;
  }
  std::copy_if(ants.begin(), ants.end(), std::back_inserter(ants_copy),
               [](const Ant& ant) { return ant.age < Config::ANT_DEATH_AGE; });
  std::swap(ants, ants_copy);
  num_ants = ants.size();
}

int LangtonAntGame::get_neighborhood_count(int i, int j,
                                           const Matrix& matrix) const {
  int ans = 0;
  for (const auto& diff : neighborhood_diffs) {
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

void LangtonAntGame::on_click(int i, int j) { matrix.toggle(i, j); }
