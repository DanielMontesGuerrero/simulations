#include "utilscpp/httprequest.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <nlohmann/json.hpp>

using std::list;
using std::map;
using std::string;
using std::stringstream;
using json = nlohmann::json;
using curlpp::Easy;
using curlpp::options::Url;
using curlpp::options::WriteStream;

json make_htpp_request(string url, map<string, string> query_params) {
  try {
    Easy request;
    stringstream response;
    request.setOpt(new Url(url + get_params_string(query_params)));
    request.setOpt(new WriteStream(&response));
    request.perform();
    auto raw_response = response.str();
    auto response_json = json::parse(raw_response);
    return response_json;
  } catch (curlpp::LibcurlLogicError& e) {
    std::cerr << e.what() << std::endl;
    return {"result", false};
  } catch (curlpp::RuntimeError& e) {
    std::cerr << e.what() << std::endl;
    return {"result", false};
  }
}

string get_params_string(map<string, string> params) {
  string url_prams = "";
  if (!params.empty()) url_prams = "?";
  for (const auto& it : params) {
    url_prams += "&" + it.first + "=" + it.second;
  }
  return url_prams;
}
