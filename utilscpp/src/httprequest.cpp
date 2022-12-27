#include "utilscpp/httprequest.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

using std::list;
using std::map;
using std::string;
using std::stringstream;
using json = nlohmann::json;
using curlpp::Easy;
using curlpp::options::Url;
using curlpp::options::WriteStream;

json make_htpp_request(string url, map<string, string> query_params,
                       bool is_post = false) {
  string dummy;
  return make_htpp_request(url, query_params, is_post, dummy);
}

json make_htpp_request(string url, map<string, string> query_params,
                       bool is_post, const string& body) {
  try {
    Easy request;
    stringstream response;
    std::list<std::string> header;
    header.push_back("Content-Type: application/json");
    request.setOpt(new curlpp::options::HttpHeader(header));

    request.setOpt(new Url(url + get_params_string(query_params)));
    request.setOpt(new WriteStream(&response));
    if (is_post) {
      request.setOpt(new curlpp::options::PostFields(body));
      request.setOpt(new curlpp::options::PostFieldSize(body.size()));
    }
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
