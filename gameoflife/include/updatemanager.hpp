#pragma once

#include <ctime>

struct UpdateManager {
  double update_rate_ms = 500;
  clock_t last_update_timestamp = 0;
};
