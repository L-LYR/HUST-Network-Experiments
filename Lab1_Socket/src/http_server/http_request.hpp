#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <unordered_map>

#include "http_protocol.hpp"

struct httpRequest {
    httpRequest(){};
    httpRequest(const std::string &request);
    ~httpRequest(){};

    bool parseRequest(const std::string &line);
    bool parseOption(const std::string &line);

    std::string _requestLine;
    std::string _headerLines;
    std::string _entityBody;

    std::unordered_map<std::string, std::string> headers;
};

#endif