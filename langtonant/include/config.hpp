#pragma once

#include <string>
#include <vector>

#include "utilscpp/utils.hpp"

using std::string;
using std::vector;

namespace Config {
extern float ANTS_DISTRIBUTION;
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
extern char* PLOT_PROXY;
extern vector<float> ANTS_DENSITIES;
extern int ANT_DEATH_AGE;
extern int QUEEN_BATTLE_MAX_AGE_DIFFERENCE;
extern float QUEEN_BATTLE_DISTADVANTAGE_PROB;

string init(int argc, char** argv);
string get_plot_proxy_port();
};  // namespace Config