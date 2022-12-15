#include "langtonant/protocol.hpp"

#include <iostream>
#include <vector>

#include "langtonant/langtonant.hpp"
#include "utilscpp/matrix.hpp"

using std::vector;

vector<char> Protocol::create_packet(char message_type, char event,
                                     const vector<char>& data) {
  vector<char> packet;
  packet.push_back(message_type);
  packet.push_back(event);
  auto aux = int_to_bytes(data.size());
  packet.insert(packet.end(), aux.begin(), aux.end());
  packet.insert(packet.end(), data.begin(), data.end());
  return packet;
}

vector<char> Protocol::ints_to_bytes(const vector<int>& data) {
  vector<char> ans;
  for (auto i : data) {
    auto aux = int_to_bytes(i);
    ans.insert(ans.end(), aux.begin(), aux.end());
  }
  return ans;
}

vector<char> Protocol::int_to_bytes(int x) {
  return {static_cast<char>(x & 0x000000ff),
          static_cast<char>((x & 0x0000ff00) >> 8),
          static_cast<char>((x & 0x00ff0000) >> 16),
          static_cast<char>((x & 0xff000000) >> 24)};
}

int Protocol::bytes_to_int(char* buffer) {
  int ans = 0;
  for (int i = 0; i < 4; i++) {
    ans |= ((buffer[i] & 0x000000ff) << (8 * i));
  }
  return ans;
}

int Protocol::bytes_to_int(vector<char>::iterator ini,
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

Matrix Protocol::deserialize_matrix(vector<char>* packet) {
  if (packet->size() < 8) {
    return Matrix(0, 0);
  }
  auto rows = bytes_to_int(packet->begin(), packet->begin() + 4);
  auto cols = bytes_to_int(packet->begin() + 4, packet->begin() + 8);
  Matrix matrix(rows, cols);
  int num_of_ints = cols / 32 + (cols % 32 ? 1 : 0);
  vector<char> data(packet->begin() + 8, packet->end());
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j += 32) {
      int val = 0;
      if (data.size() >= (i * num_of_ints * 4 + j / 32 * 4 + 4)) {
        val = bytes_to_int(data.begin() + i * num_of_ints * 4 + j / 32 * 4,
                           data.begin() + i * num_of_ints * 4 + j / 32 * 4 + 4);
      }
      for (int k = 0; k < 32; k++) {
        matrix.set(i, j + k, (val >> k) & 1);
      }
    }
  }
  return matrix;
}

vector<int> Protocol::serialize_matrix(const Matrix& matrix) {
  vector<int> packet;
  packet.push_back(matrix.rows);
  packet.push_back(matrix.cols);
  for (int i = 0; i < matrix.rows; i++) {
    for (int j = 0; j < matrix.cols; j += 32) {
      int val = 0;
      for (int k = 0; k < 32; k++) {
        if (matrix.get(i, j + k)) val |= (1 << k);
      }
      packet.push_back(val);
    }
  }
  return packet;
}

vector<int> Protocol::serialize_ants(const vector<Ant>& ants) {
  vector<int> packet;
  packet.push_back(ants.size());
  for (auto ant : ants) {
    packet.push_back(ant.x);
    packet.push_back(ant.y);
    packet.push_back(ant.dir);
    packet.push_back(ant.type);
    packet.push_back(ant.age);
  }
  return packet;
}

vector<Ant> Protocol::deserialize_ants(vector<char>* packet) {
  auto size = bytes_to_int(packet->begin(), packet->begin() + 4);
  std::cerr << "expected ants: " << size << std::endl;
  vector<Ant> ants;
  for (int i = 0; i < size; i++) {
    vector<int> ant_data;
    for (int j = 0; j < 5; j++) {
      auto num = bytes_to_int(packet->begin() + 4 + i * 5 * 4 + j * 4,
                              packet->begin() + 8 + i * 5 * 4 + j * 4);
      ant_data.push_back(num);
    }
    Ant ant(0, 0);
    ant.x = ant_data[0];
    ant.y = ant_data[1];
    ant.dir = ant_data[2];
    ant.type = ant_data[3];
    ant.age = ant_data[4];
    ants.push_back(ant);
  }
  return ants;
}
