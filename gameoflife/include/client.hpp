#pragma once

#include <arpa/inet.h>

#include <string>
#include <vector>

#include "utilscpp/matrix.hpp"

using std::string;
using std::vector;

class Client {
 public:
  string host;
  int port;
  int type;
  struct sockaddr_in serv_addr;

  Client(string host, int port, int type);
  vector<char> send_message(char message_type, char event, vector<char> data);
  vector<char> send_message(char message_type, char event, vector<int> data);
  Matrix deserialize_matrix(vector<char> packet);

 private:
  void send_raw(int sockd, vector<char> data);
  vector<char> create_packet(char message_type, char event, vector<char> data);
  vector<char> read_response_packet(int sockd);
  void read_raw(int sockd, char* buffer, int len);
  int bytes_to_int(char* buffer);
  int bytes_to_int(vector<char>::iterator ini, vector<char>::iterator fin);
  vector<char> ints_to_bytes(vector<int> data);
  vector<char> int_to_bytes(int x);
};
