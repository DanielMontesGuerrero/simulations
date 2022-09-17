#pragma once

namespace Config {
const bool SHOUD_EXECUTE_LOCALLY = false;
const int CELL_SIZE = 10;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;
const int HEIGHT = (GRID_HEIGHT * CELL_SIZE) + 1;
const int WIDTH = (GRID_WIDTH * CELL_SIZE) + 1;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int ZOOM_FACTOR = 2;
const int ZOOM_DEFAULT = 1;
const int MARGIN = 10;
const int SCROLL_AMOUNT = CELL_SIZE;
const bool SHOULD_DRAW_GRID_LINES = true;
const bool DEBUG = false;
const bool SHOULD_MODULE_INDEXES = true;
const double SPEED_FACTOR = 10;
const double MIN_UPDATE_RATE_MS = 10;
const double MAX_UPDATE_RATE_MS = 5000;
};  // namespace Config
