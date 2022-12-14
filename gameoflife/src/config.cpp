#include "gameoflife/config.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>

using boost::program_options::command_line_parser;
using boost::program_options::notify;
using boost::program_options::options_description;
using boost::program_options::parse_config_file;
using boost::program_options::parse_environment;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::ifstream;
using std::string;

namespace Config {
bool SHOULD_EXECUTE_LOCALLY = true;
int CELL_SIZE = 10;
int GRID_HEIGHT = 10;
int GRID_WIDTH = 10;
int HEIGHT = 1;
int WIDTH = 1;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;
int ZOOM_FACTOR = 2;
int ZOOM_DEFAULT = 1;
Offset MARGIN(10, 10, 30, 30);
int SCROLL_AMOUNT = CELL_SIZE;
bool SHOULD_DRAW_GRID_LINES = true;
bool DEBUG = false;
bool SHOULD_MODULE_INDEXES = true;
double SPEED_FACTOR = 10;
double MIN_UPDATE_RATE_MS = 10;
double MAX_UPDATE_RATE_MS = 5000;
bool IS_ORCHESTRATOR_STANDALONE = false;
char* HOST;
int PORT = 3000;
char* AZ_CREATE_ORCH_FUNC;
char* AZ_ORCH_FUNC_CODE;
char* SAVE_TO_FOLDER;
float DENSITY = 0.5;

string init(int argc, char** argv) {
  string config_file;
  string host = "127.0.0.1";
  string az_create_orch_func;
  string az_orch_func_code;
  string save_to_folder;
  string load_matrix;
  int margin;
  options_description game("Game options");
  options_description window("Window options");
  options_description development("Dev options");
  options_description cloud("Cloud options");
  game.add_options()("help", "Shows available options")(
      "config", value<string>(&config_file)->default_value("GAMEOFLIFE.cfg"),
      "configuration file")("rows", value<int>(&GRID_HEIGHT), "Number of rows")(
      "cols", value<int>(&GRID_WIDTH), "Number of cols")(
      "module-indexes", value<bool>(&SHOULD_MODULE_INDEXES),
      "Module indexes of grid when updating")("speed-factor",
                                              value<double>(&SPEED_FACTOR),
                                              "Single update speed increment")(
      "max-update-rate", value<double>(&MAX_UPDATE_RATE_MS),
      "Maximum update rate in milliseconds")(
      "min-update-rate", value<double>(&MIN_UPDATE_RATE_MS),
      "Minimum update rate in milliseconds")("density", value<float>(&DENSITY),
                                             "Initial alive cells density")(
      "folder", value<string>(&save_to_folder)->default_value(""),
      "Folder where configurations will be stored")(
      "load-matrix", value<string>(&load_matrix),
      "Matrix configuration to load");
  window.add_options()("cell-size", value<int>(&CELL_SIZE),
                       "Size of each cell in pixels")(
      "window-w", value<int>(&WINDOW_WIDTH), "Width of screen in pixels")(
      "window-h", value<int>(&WINDOW_HEIGHT), "Height of screen in pixels")(
      "zoom-factor", value<int>(&ZOOM_FACTOR), "Zoom factor")(
      "zoom-default", value<int>(&ZOOM_DEFAULT), "Zoom default")(
      "margin", value<int>(&margin)->default_value(10),
      "Margin of screen in pixels")("scroll-amount", value<int>(&SCROLL_AMOUNT),
                                    "Number of pixels to scroll")(
      "draw-grid-lines", value<bool>(&SHOULD_DRAW_GRID_LINES),
      "Draw lines between cells in the grid");
  development.add_options()("debug", value<bool>(&DEBUG),
                            "Activate debug mode");
  cloud.add_options()("local", value<bool>(&SHOULD_EXECUTE_LOCALLY),
                      "Execute game in local computer or in the cloud")(
      "host", value<string>(&host), "Orchestrator host")(
      "port", value<int>(&PORT), "Orchestrator port")(
      "az-orchestrator", value<string>(&az_create_orch_func),
      "Url of azure orchestrator function")(
      "az-orchestrator-code", value<string>(&az_orch_func_code),
      "Code for azure orchestrator function")(
      "orchestrator-standalone", value<bool>(&IS_ORCHESTRATOR_STANDALONE),
      "Determines if orchestrator is standalone");
  game.add(window).add(development).add(cloud);

  variables_map vm;
  store(command_line_parser(argc, argv).options(game).run(), vm);
  notify(vm);
  ifstream ifs(config_file.c_str());
  if (ifs) {
    store(parse_config_file(ifs, game), vm);
    notify(vm);
  } else {
    std::cerr << "Can not open file: " << config_file << std::endl;
  }

  if (vm.count("help")) std::cerr << game << std::endl;
  /* if(save_to_folder.back() != '/') save_to_folder.push_back('/'); */

  MARGIN = Offset(margin, margin, 30, 30);
  HEIGHT = (GRID_HEIGHT * CELL_SIZE) + 1;
  WIDTH = (GRID_WIDTH * CELL_SIZE) + 1;
  HOST = reinterpret_cast<char*>(malloc(sizeof(char) * (host.size() + 1)));
  AZ_CREATE_ORCH_FUNC = reinterpret_cast<char*>(
      malloc(sizeof(char) * (az_create_orch_func.size() + 1)));
  AZ_ORCH_FUNC_CODE = reinterpret_cast<char*>(
      malloc(sizeof(char) * (az_orch_func_code.size() + 1)));
  SAVE_TO_FOLDER = reinterpret_cast<char*>(
      malloc(sizeof(char) * (save_to_folder.size() + 1)));
  snprintf(HOST, host.size() + 1, host.c_str());
  snprintf(AZ_CREATE_ORCH_FUNC, az_create_orch_func.size() + 1,
           az_create_orch_func.c_str());
  snprintf(AZ_ORCH_FUNC_CODE, az_orch_func_code.size() + 1,
           az_orch_func_code.c_str());
  snprintf(SAVE_TO_FOLDER, save_to_folder.size() + 1, save_to_folder.c_str());
  return load_matrix;
}
};  // namespace Config
