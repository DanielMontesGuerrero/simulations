#pragma once

#include <string>
#include "utilscpp/utils.hpp"

using std::string;

namespace Config {
extern bool SHOULD_EXECUTE_LOCALLY;
extern int CELL_SIZE;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;
extern int HEIGHT;
extern int WIDTH;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int ZOOM_FACTOR;
extern int ZOOM_DEFAULT;
extern Offset MARGIN;
extern int SCROLL_AMOUNT;
extern bool SHOULD_DRAW_GRID_LINES;
extern bool DEBUG;
extern bool SHOULD_MODULE_INDEXES;
extern double SPEED_FACTOR;
extern double MIN_UPDATE_RATE_MS;
extern double MAX_UPDATE_RATE_MS;
extern bool IS_ORCHESTRATOR_STANDALONE;
extern char* HOST;
extern int PORT;
extern char* AZ_CREATE_ORCH_FUNC;
extern char* AZ_ORCH_FUNC_CODE;
extern float DENSITY;
extern char* SAVE_TO_FOLDER;

string init(int argc, char** argv);
};  // namespace Config
