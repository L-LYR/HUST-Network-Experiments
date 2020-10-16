#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <cstring>
#include <string>

#include "http_protocol.hpp"
#include "http_request.hpp"

struct httpResponse {
    httpResponse(const std::string& path, const std::string& name)
        : _path(path), _serverName(name) { _entityLen = _respLen = 0; };
    ~httpResponse(){};

    void respondTo(const httpRequest& req);

    bool respondGetOpt(const std::string& target);

    void setHeader(const std::string& key, const std::string& value);
    int setEntity(const std::string& docRePath);

    const char* fullResponse(void);
    int getLength(void);
    void cleanup(void);

    enum {
        BUFSIZE = 81920,
    };

    std::string _path;
    std::string _serverName;

    std::string _statusLine;
    std::string _headerLines;
    char _entityBody[BUFSIZE];
    int _entityLen;

    char _resp[BUFSIZE];
    int _respLen;
};

#endif