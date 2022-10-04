#pragma once

#include <map>
#include <string>
#include <nlohmann/json.hpp>

using std::map;
using std::string;
using json = nlohmann::json;

json make_htpp_request(string url, map<string, string> query_params);

string get_params_string(map<string, string> params);
