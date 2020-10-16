#include "http_request.hpp"

#include <iostream>

#include "http_protocol.hpp"

httpRequest::httpRequest(const std::string &request) {
    size_t idx = 0, optBegin = 0;
    std::string line = request.substr(idx, request.find(CRLF, idx) - idx);
    idx += line.length() + CRLF.length();
    optBegin = idx;

    _requestLine = line;

    if (!parseRequest(line)) {
        std::cout << "Error in parsing HTTP request line: "
                  << line << std::endl;
    }

    line = request.substr(idx, request.find(CRLF, idx) - idx);
    idx += line.length() + CRLF.length();

    while (line != "") {
        if (!parseOption(line)) {
            std::cout << "Error in parsing HTTP header line: "
                      << line << std::endl;
        }
        line = request.substr(idx, request.find(CRLF, idx) - idx);
        idx += line.length() + CRLF.length();
    }
    _headerLines = request.substr(optBegin, idx - optBegin);
    _entityBody = request.substr(idx);
}

// TODO: add error detections
bool httpRequest::parseRequest(const std::string &line) {
    size_t idx = 0;
    std::string opt;

    opt = line.substr(idx, line.find(" ", idx) - idx);
    headers["Method"] = opt;
    idx += opt.length() + 1;

    opt = line.substr(idx, line.find(" ", idx) - idx);
    headers["Document"] = opt;
    idx += opt.length() + 1;

    opt = line.substr(idx, line.find(CRLF, idx) - idx);
    headers["Version"] = opt;

    return true;
}

bool httpRequest::parseOption(const std::string &line) {
    size_t idx = 0;
    std::string key, value;
    key = line.substr(idx, line.find(": ", idx) - idx);
    idx += key.length() + 2;
    value = line.substr(idx, line.find(CRLF, idx) - idx);
    headers[key] = value;
    return true;
}