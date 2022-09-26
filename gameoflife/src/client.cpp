#include "gameoflife/client.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "gameoflife/config.hpp"
#include "gameoflife/protocol.hpp"
#include "utilscpp/httprequest.hpp"

using std::cerr;
using std::endl;
using std::string;
using std::tuple;
using std::vector;

Client::Client(string host, int port, int type)
    : host(host), port(port), type(type) {
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
    cerr << "Failed to create client" << endl;
    return;
  }
}

vector<char> Client::send_message(char message_type, char event,
                                  vector<char> data) {
  int sockd = 0;
  int cliend_fd = 0;
  if ((sockd = socket(AF_INET, type, 0)) < 0) {
    return {};
  }
  if ((cliend_fd = connect(sockd, (struct sockaddr*)&serv_addr,
                           sizeof(serv_addr))) < 0) {
    return {};
  }
  auto packet = create_packet(message_type, event, data);
  send_raw(sockd, packet);
  return read_response_packet(sockd);
}

vector<char> Client::create_packet(char message_type, char event,
                                   vector<char> data) {
  vector<char> packet;
  packet.push_back(message_type);
  packet.push_back(event);
  auto aux = int_to_bytes(data.size());
  packet.insert(packet.end(), aux.begin(), aux.end());
  packet.insert(packet.end(), data.begin(), data.end());
  return packet;
}

void Client::send_raw(int sockd, vector<char> data) {
  int sent_len =
      send(sockd, reinterpret_cast<char*>(data.data()), data.size(), 0);
  if (sent_len == -1) {
    cerr << "Error sending data" << endl;
  }
}

vector<char> Client::send_message(char message_type, char event,
                                  vector<int> data) {
  return send_message(message_type, event, ints_to_bytes(data));
}

vector<char> Client::ints_to_bytes(vector<int> data) {
  vector<char> ans;
  for (auto i : data) {
    auto aux = int_to_bytes(i);
    ans.insert(ans.end(), aux.begin(), aux.end());
  }
  return ans;
}

vector<char> Client::int_to_bytes(int x) {
  return {static_cast<char>(x & 0x000000ff), static_cast<char>(x & 0x0000ff00),
          static_cast<char>(x & 0x00ff0000), static_cast<char>(x & 0xff000000)};
}

vector<char> Client::read_response_packet(int sockd) {
  char* header = reinterpret_cast<char*>(std::malloc(4 * sizeof(char)));
  read_raw(sockd, header, 4);
  int len = bytes_to_int(header);
  char* buffer = reinterpret_cast<char*>(std::malloc(len * sizeof(char)));
  read_raw(sockd, buffer, len);
  vector<char> response(buffer, buffer + len);
  free(header);
  free(buffer);
  return response;
}

void Client::read_raw(int sockd, char* buffer, int len) {
  int bytes_read = 0;
  int result;
  while (bytes_read < len) {
    result = read(sockd, buffer + bytes_read, len - bytes_read);
    if (result < 1) {
      cerr << "failed to read data" << endl;
      return;
    }
    bytes_read += result;
  }
}

int Client::bytes_to_int(char* buffer) {
  int ans = 0;
  for (int i = 0; i < 4; i++) {
    ans |= ((buffer[i] & 0x000000ff) << (8 * i));
  }
  return ans;
}

int Client::bytes_to_int(vector<char>::iterator ini,
                         vector<char>::iterator fin) {
  int len = fin - ini;
  char* buffer = reinterpret_cast<char*>(std::malloc(len * sizeof(char)));
  for (auto it = ini; it != fin; it++) {
    buffer[it - ini] = *it;
  }
  int val = bytes_to_int(buffer);
  free(buffer);
  return val;
}

Matrix Client::deserialize_matrix(vector<char> packet) {
  if (packet.size() < 8) {
    return Matrix(0, 0);
  }
  auto rows = bytes_to_int(packet.begin(), packet.begin() + 4);
  auto cols = bytes_to_int(packet.begin() + 4, packet.begin() + 8);
  Matrix matrix(rows, cols);
  int num_of_ints = cols / 32 + (cols % 32 ? 1 : 0);
  vector<char> data(packet.begin() + 8, packet.end());
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j += 32) {
      int val = 0;
      if (data.size() >= (i * num_of_ints * 4 + j * 4 + 4)) {
        val = bytes_to_int(data.begin() + i * num_of_ints * 4 + j * 4,
                           data.begin() + i * num_of_ints * 4 + j * 4 + 4);
      }
      for (int k = 0; k < 32; k++) {
        matrix.set(i, j + k, (val >> k) & 1);
      }
    }
  }
  return matrix;
}

tuple<string, int> get_orchestrator_host() {
  std::map<string, string> params{{"rows", std::to_string(Config::GRID_HEIGHT)},
                                  {"cols", std::to_string(Config::GRID_WIDTH)}};
  auto response = make_htpp_request(Protocol::AZ_CREATE_ORCH_FUNC, params);
  if (response["result"] == nullptr) {
    cerr << "Error reading response of request_orchestrator_host" << endl;
    return std::make_tuple("", 0);
  } else if (!response["result"]) {
    cerr << "Error getting response of request_orchestrator_host: "
         << response["description"] << endl;
    return std::make_tuple("", 0);
  }
  return std::make_tuple(response["data"]["orchestrator"]["public_ip"],
                         response["data"]["orchestrator"]["port"]);
}
