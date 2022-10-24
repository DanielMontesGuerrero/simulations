#pragma once

#include <deque>
#include <queue>
#include <utility>

#include "gameoflife/client.hpp"
#include "gameoflife/gameoflife.hpp"
#include "utilscpp/matrix.hpp"

using std::deque;
using std::pair;
using std::queue;

struct Submatrix {
  int x, y;
  Matrix matrix;
};

class GameHandler {
 public:
  int rows;
  int cols;
  GameOfLife gameoflife;
  bool is_executed_locally;
  Client client;
  queue<Submatrix, deque<Submatrix>> pending_updates;

  GameHandler(int rows, int cols, bool is_executed_locally = true);
  void update();
  void send_get_message(int x, int y, int w, int h);
  void send_get_message();
  void on_click(int i, int j);
  void increase_update_rate();
  void decrease_update_rate();
  void toggle_pause();
  void close_server();
  void send_update_message();
  long long int get_num_cells_alive();
  int get_current_iteration();

 private:
  int last_x;
  int last_y;
  int last_w;
  int last_h;
  pair<int, int> sanitize_coords(int i, int j);
};
