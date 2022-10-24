#include "utilscpp/utils.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using std::cerr;
using std::endl;
using std::max;
using std::min;
using std::pair;
using std::string;
using std::tuple;
using std::vector;

pair<int, int> translate_coords_from_window_to_rect(int window_w, int window_h,
                                                    Offset offset, int rect_w,
                                                    int rect_h, int x, int y) {
  window_w -= (offset.left + offset.right);
  window_h -= (offset.bottom + offset.top);
  x = max(0, x - offset.left);
  x = min(window_w, x);
  y = max(0, y - offset.top);
  y = min(window_h, y);
  double relative_x = static_cast<double>(x) / window_w;
  double relative_y = static_cast<double>(y) / window_h;
  return {rect_w * relative_x, rect_h * relative_y};
}

pair<int, int> translate_coords_from_rect_to_texture(int rect_w, int rect_h,
                                                     int offset_x, int offset_y,
                                                     int texture_w,
                                                     int texture_h, int x,
                                                     int y) {
  double relative_x = 1;
  double relative_y = 1;
  if (rect_w > texture_w) relative_x = static_cast<double>(texture_w) / rect_w;
  if (rect_h > texture_h) relative_y = static_cast<double>(texture_h) / rect_h;
  return {offset_x + x * relative_x, offset_y + y * relative_y};
}

pair<int, int> translate_coords_from_rect_to_matrix(int rect_w, int rect_h,
                                                    int offset_x, int offset_y,
                                                    int texture_w,
                                                    int texture_h, int x, int y,
                                                    int cell_size) {
  std::tie(x, y) = translate_coords_from_rect_to_texture(
      rect_w, rect_h, offset_x, offset_y, texture_w, texture_h, x, y);
  x /= cell_size;
  y /= cell_size;
  return {x, y};
}

void save_to_file(string path, const vector<char>& data) {
  std::ofstream file;
  file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
  if (file.is_open()) {
    file.write(data.data(), data.size());
    file.close();
  }
}

vector<char> read_from_file(string path) {
  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary | std::ios::ate);
  vector<char> data;
  if (file.is_open()) {
    int size = file.tellg();
    char* buffer = reinterpret_cast<char*>(std::malloc(size * sizeof(char)));
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    data = vector<char>(buffer, buffer + size);
    file.close();
    delete[] buffer;
  }
  return data;
}
