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

#include "utilscpp/matrix.hpp"
#include "gameoflife/config.hpp"
#include "gameoflife/protocol.hpp"
#include "utilscpp/httprequest.hpp"

using std::cerr;
using std::cout;
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
                                  const vector<char>& data) {
  int sockd = 0;
  int cliend_fd = 0;
  if ((sockd = socket(AF_INET, type, 0)) < 0) {
    return {};
  }
  if ((cliend_fd = connect(sockd, (struct sockaddr*)&serv_addr,
                           sizeof(serv_addr))) < 0) {
    return {};
  }
  auto packet = Protocol::create_packet(message_type, event, data);
  send_raw(sockd, packet);
  return read_response_packet(sockd);
}

void Client::send_raw(int sockd, vector<char> data) {
  int sent_len =
      send(sockd, reinterpret_cast<char*>(data.data()), data.size(), 0);
  if (sent_len == -1) {
    cerr << "Error sending data" << endl;
  }
}

vector<char> Client::send_message(char message_type, char event,
                                  const vector<int>& data) {
  return send_message(message_type, event, Protocol::ints_to_bytes(data));
}

vector<char> Client::read_response_packet(int sockd) {
  char* header = reinterpret_cast<char*>(std::malloc(4 * sizeof(char)));
  read_raw(sockd, header, 4);
  int len = Protocol::bytes_to_int(header);
  char* buffer = reinterpret_cast<char*>(std::malloc(len * sizeof(char)));
  read_raw(sockd, buffer, len);
  vector<char> response(buffer, buffer + len);
  free(header);
  free(buffer);
  return response;
}

void Client::read_raw(int sockd, char* buffer, int len) {
  int max_size = 4095;
  int bytes_read = 0;
  int result;
  while (bytes_read < len) {
    int len_to_read = std::min(max_size, len - bytes_read);
    result = read(sockd, buffer + bytes_read, len_to_read);
    if (result < 1) {
      cerr << "failed to read data" << endl;
      return;
    }
    bytes_read += result;
  }
}

tuple<string, int> get_orchestrator_host() {
  if (strlen(Config::AZ_CREATE_ORCH_FUNC) == 0) return std::make_tuple("", 0);
  std::map<string, string> params{{"code", Config::AZ_ORCH_FUNC_CODE},
                                  {"rows", std::to_string(Config::GRID_HEIGHT)},
                                  {"cols", std::to_string(Config::GRID_WIDTH)}};
  auto response = make_htpp_request(Config::AZ_CREATE_ORCH_FUNC, params);
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
