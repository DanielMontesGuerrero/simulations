#include "langtonant/gamehandler.hpp"

#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "langtonant/client.hpp"
#include "langtonant/config.hpp"
#include "langtonant/langtonant.hpp"
#include "langtonant/protocol.hpp"
#include "utilscpp/utils.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::pair;
using std::string;
using std::tie;

GameHandler::GameHandler(int rows, int cols, bool is_executed_locally,
                         string state_config)
    : rows(rows),
      cols(cols),
      is_executed_locally(is_executed_locally),
      client(Config::HOST, Config::PORT, Protocol::TYPE) {
  last_x = 0;
  last_x = 0;
  last_h = 0;
  last_w = 0;
  if (is_executed_locally) {
    langtonantGame = LangtonAntGame(rows, cols, [](int i, int j) {
      auto prob = static_cast<float>(rand()) / RAND_MAX;
      return prob <= Config::DENSITY;
    });
    if (!state_config.empty()) {
      set_state_from_file(state_config);
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
    langtonantGame.update();
  } else if (!Config::IS_ORCHESTRATOR_STANDALONE) {
    send_update_message();
  }
  try {
    update_plot_proxy("density", get_current_iteration(), get_density());
    update_plot_proxy("entropy", get_current_iteration(), get_entropy());
  } catch (...) {
    // proxy not connected
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
    if (Config::DEBUG) {
      cout << "request submatrix: [" << ui << ',' << bi << ',' << lj << ','
           << rj << "]" << endl;
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
    langtonantGame.on_click(i, j);
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

int64_t GameHandler::get_num_ants() {
  if (is_executed_locally) {
    return langtonantGame.num_ants;
  } else {
    // Unimplemented
    return 0;
  }
}

int64_t GameHandler::get_total_num_cells() {
  if (is_executed_locally) {
    return static_cast<int64_t>(langtonantGame.matrix.rows) *
           langtonantGame.matrix.cols;
  } else {
    // Unimplemented
    return 0;
  }
}

int GameHandler::get_current_iteration() {
  if (is_executed_locally) {
    return langtonantGame.current_iteration;
  } else {
    // Unimplemented
    return 0;
  }
}

long double GameHandler::get_density() {
  return static_cast<long double>(this->get_num_ants()) /
         static_cast<long double>(this->get_total_num_cells());
}

long double GameHandler::get_entropy() {
  long double density = std::max(1e-6L, std::min(1.0L - 1e-6L, get_density()));
  return -(density * std::log2(density) +
           (1 - density) * std::log2(1 - density));
}

void GameHandler::save_current_state(string path) {
  auto matrix = Protocol::serialize_matrix(langtonantGame.matrix);
  auto ants = Protocol::serialize_ants(langtonantGame.ants);
  auto data = matrix;
  data.insert(data.end(), ants.begin(), ants.end());
  save_to_file(Config::SAVE_TO_FOLDER + path, Protocol::ints_to_bytes(data));
}

void GameHandler::set_state_from_file(string path) {
  auto data = read_from_file(path);
  /* for(auto i:data) std::cerr << i << ' '; std::cerr << std::endl; */
  auto matrix = Protocol::deserialize_matrix(&data);
  int num_of_ints = matrix.cols / 32 + (matrix.cols % 32 ? 1 : 0);
  int matrix_bytes_size = 8 + matrix.rows * num_of_ints * 4;
  auto ants_data =
      std::vector<char>(data.begin() + matrix_bytes_size, data.end());
  auto ants = Protocol::deserialize_ants(&ants_data);
  langtonantGame.matrix = matrix;
  langtonantGame.ants = ants;
  cout << "matrix: " << matrix.cols << ' ' << matrix.cols << endl;
  cout << "ants: " << ants.size() << endl;
}
