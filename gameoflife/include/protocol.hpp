#pragma once

#include <sys/socket.h>

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
};  // namespace Protocol
