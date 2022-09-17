#include "gameoflife/gamehandler.hpp"

#include <iostream>

#include "gameoflife/gameoflife.hpp"
#include "gameoflife/protocol.hpp"

using std::cerr;
using std::endl;

GameHandler::GameHandler(int rows, int cols, bool is_executed_locally)
    : is_executed_locally(is_executed_locally),
      client(Protocol::HOST, Protocol::PORT, Protocol::TYPE) {
  last_x = 0;
  last_x = 0;
  last_h = 0;
  last_w = 0;
  if (is_executed_locally) {
    gameoflife =
        GameOfLife(rows, cols, [](int i, int j) { return rand() % 2; });
  }
}

void GameHandler::update(int x, int y, int w, int h) {
  last_x = x;
  last_y = y;
  last_w = w;
  last_h = h;
  if (is_executed_locally) {
    gameoflife.update();
  } else {
    cerr << "sending get request" << endl;
    cerr << "requesting submatrix:" << endl;
    cerr << y << ' ' << y + h - 1 << ' ' << x << ' ' << x + w - 1 << endl;
    auto buffer =
        client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_GET,
                            vector<int>{y, y + h - 1, x, x + w - 1});
    auto matrix = client.deserialize_matrix(buffer);
    pending_updates.push({x, y, matrix});
  }
}

void GameHandler::update() { update(last_x, last_y, last_w, last_h); }

void GameHandler::on_click(int i, int j) {
  if (is_executed_locally) {
    gameoflife.on_click(i, j);
  } else {
    client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_MOUSE_CLICK,
                        vector<int>{i, j});
  }
}

void GameHandler::toggle_pause() {
  if (is_executed_locally) return;
  cerr << "Sending pause request" << endl;
  client.send_message(Protocol::MESSAGE_EVENT, Protocol::EVENT_PAUSE,
                      vector<char>{});
}

void GameHandler::increase_update_rate() {
  if (is_executed_locally) return;
  cerr << "Sending increase_update_rate request" << endl;
  client.send_message(Protocol::MESSAGE_EVENT,
                      Protocol::EVENT_UPDATE_RATE_INCREASE, vector<char>{});
}

void GameHandler::decrease_update_rate() {
  if (is_executed_locally) return;
  cerr << "Sending decrease_update_rate request" << endl;
  client.send_message(Protocol::MESSAGE_EVENT,
                      Protocol::EVENT_UPDATE_RATE_DECREASE, vector<char>{});
}

void GameHandler::close_server() {
  if (is_executed_locally) return;
  client.send_message(Protocol::MESSAGE_CLOSE, 0, vector<char>{});
}
