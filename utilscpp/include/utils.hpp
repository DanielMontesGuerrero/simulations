#pragma once

#include <tuple>
#include <utility>
#include <vector>
#include <string>

using std::pair;
using std::tuple;
using std::vector;
using std::string;

struct Offset {
  int left, right, top, bottom;
  explicit Offset(int margin)
      : left(margin), right(margin), top(margin), bottom(margin) {}
  Offset(int x, int y) : left(x), right(x), top(y), bottom(y) {}
  Offset(int left, int right, int top, int bottom)
      : left(left), right(right), top(top), bottom(bottom) {}
};

pair<int, int> translate_coords_from_window_to_rect(int window_w, int window_h,
                                                    Offset offset, int rect_w,
                                                    int rect_h, int x, int y);

pair<int, int> translate_coords_from_rect_to_texture(int rect_w, int rect_h,
                                                     int offset_x, int offset_y,
                                                     int texture_w,
                                                     int texture_h, int x,
                                                     int y);

pair<int, int> translate_coords_from_rect_to_matrix(int rect_w, int rect_h,
                                                    int offset_x, int offset_y,
                                                    int texture_w,
                                                    int texture_h, int x, int y,
                                                    int cell_size);

void save_to_file(string path, const vector<char>& data);

vector<char> read_from_file(string path);
