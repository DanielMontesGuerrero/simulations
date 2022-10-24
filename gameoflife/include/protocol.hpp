#pragma once

#include <sys/socket.h>

#include <vector>

#include "utilscpp/matrix.hpp"

using std::vector;

namespace Protocol {
const int TYPE = SOCK_STREAM;

const char MESSAGE_PING = 1;
const char MESSAGE_ERROR = 2;
const char MESSAGE_EVENT = 3;
const char MESSAGE_CLOSE = 9;

const char EVENT_PAUSE = 1;
const char EVENT_MOUSE_CLICK = 2;
const char EVENT_UPDATE_RATE_INCREASE = 3;
const char EVENT_UPDATE_RATE_DECREASE = 4;
const char EVENT_GET = 5;
const char EVENT_SET_BORDERS = 6;
const char EVENT_UPDATE = 7;
const char EVENT_LOG = 8;

Matrix deserialize_matrix(vector<char>* packet);
vector<int> serialize_matrix(const Matrix& matrix);
vector<char> create_packet(char message_type, char event,
                           const vector<char>& data);

int bytes_to_int(char* buffer);
int bytes_to_int(vector<char>::iterator ini, vector<char>::iterator fin);
vector<char> ints_to_bytes(const vector<int>& data);
vector<char> int_to_bytes(int x);
};  // namespace Protocol
