#pragma once

#include <deque>
#include <queue>
#include <string>
#include <utility>

#include "langtonant/client.hpp"
#include "langtonant/langtonant.hpp"
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
  LangtonAntGame langtonantGame;
  bool is_executed_locally;
  Client client;
  queue<Submatrix, deque<Submatrix>> pending_updates;

  GameHandler(int rows, int cols, bool is_executed_locally = true,
              std::string matrix_config = "");
  void update();
  void send_get_message(int x, int y, int w, int h);
  void send_get_message();
  void on_click(int i, int j);
  void increase_update_rate();
  void decrease_update_rate();
  void toggle_pause();
  void close_server();
  void send_update_message();
  int64_t get_num_ants();
  int64_t get_total_num_cells();
  int get_current_iteration();
  long double get_density();
  long double get_entropy();
  void save_current_state(std::string path);
  void set_state_from_file(std::string path);

 private:
  int last_x;
  int last_y;
  int last_w;
  int last_h;
  pair<int, int> sanitize_coords(int i, int j);
};
