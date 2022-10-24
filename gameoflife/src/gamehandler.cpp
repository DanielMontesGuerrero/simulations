#include "gameoflife/gamehandler.hpp"

#include <iostream>
#include <tuple>
#include <utility>
#include <string>

#include "gameoflife/client.hpp"
#include "gameoflife/config.hpp"
#include "gameoflife/gameoflife.hpp"
#include "gameoflife/protocol.hpp"
#include "utilscpp/utils.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::pair;
using std::tie;
using std::string;

GameHandler::GameHandler(int rows, int cols, bool is_executed_locally, string matrix_config)
    : rows(rows),
      cols(cols),
      is_executed_locally(is_executed_locally),
      client(Config::HOST, Config::PORT, Protocol::TYPE) {
  last_x = 0;
  last_x = 0;
  last_h = 0;
  last_w = 0;
  if (is_executed_locally) {
    gameoflife =
        GameOfLife(rows, cols, [](int i, int j) {
            return (float(rand()) / RAND_MAX) <= Config::DENSITY;
          });
    if(!matrix_config.empty()){
      set_matrix_from_file(matrix_config);
    }
  } else {
    string host;
    int port;
    std::tie(host, port) = get_orchestrator_host();
    if (host.empty()) {
      cerr << "Unable to get client host and port" << endl;
    } else {
      client = Client(host, port, Protocol::TYPE);
    }
  }
}

void GameHandler::update() {
  if (is_executed_locally) {
    gameoflife.update();
  } else if (!Config::IS_ORCHESTRATOR_STANDALONE) {
    send_update_message();
  }
}

void GameHandler::send_get_message(int x, int y, int w, int h) {
  last_x = x;
  last_y = y;
  last_w = w;
  last_h = h;
  if (!is_executed_locally) {
    int ui, bi, lj, rj;
    tie(ui, lj) = sanitize_coords(y, x);
    tie(bi, rj) = sanitize_coords(y + h - 1, x + w - 1);
    if(Config::DEBUG){
      cout << "request submatrix: [" << ui << ',' << bi << ',' << lj << ',' << rj
         << "]" << endl;
    }
    auto buffer =
        client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_GET,
                            vector<int>{ui, bi, lj, rj});
    auto matrix = Protocol::deserialize_matrix(&buffer);
    pending_updates.push({x, y, matrix});
  }
}

void GameHandler::send_get_message() {
  send_get_message(last_x, last_y, last_w, last_h);
}

void GameHandler::on_click(int i, int j) {
  if (is_executed_locally) {
    gameoflife.on_click(i, j);
  } else {
    tie(i, j) = sanitize_coords(i, j);
    cout << "send click: [" << i << ',' << j << "]" << endl;
    client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_MOUSE_CLICK,
                        vector<int>{i, j});
  }
}

void GameHandler::toggle_pause() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_PAUSE,
                      vector<char>{});
}

void GameHandler::increase_update_rate() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_EVENT,
                      Protocol::EVENT_UPDATE_RATE_INCREASE, vector<char>{});
}

void GameHandler::decrease_update_rate() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_EVENT,
                      Protocol::EVENT_UPDATE_RATE_DECREASE, vector<char>{});
}

void GameHandler::close_server() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_CLOSE, 0, vector<char>{});
}

void GameHandler::send_update_message() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_UPDATE,
                      vector<char>{});
}

pair<int, int> GameHandler::sanitize_coords(int i, int j) {
  i = min(i, rows - 1);
  i = max(i, 0);
  j = min(j, cols - 1);
  j = max(j, 0);
  return {i, j};
}

long long int GameHandler::get_num_cells_alive() {
  if(is_executed_locally){
    return gameoflife.num_cells_alive;
  }
  else{
    // Unimplemented
    return 0;
  }
}

int GameHandler::get_current_iteration() {
  if(is_executed_locally){
    return gameoflife.current_iteration;
  }
  else{
    // Unimplemented
    return 0;
  }
}

void GameHandler::save_current_config(string path){
  auto data = Protocol::serialize_matrix(gameoflife.matrix);
  save_to_file(Config::SAVE_TO_FOLDER + path, Protocol::ints_to_bytes(data));
}

void GameHandler::set_matrix_from_file(string path){
  auto data = read_from_file(path);
  /* for(auto i:data) std::cerr << i << ' '; std::cerr << std::endl; */
  auto matrix = Protocol::deserialize_matrix(&data);
  gameoflife.matrix = matrix;
}
