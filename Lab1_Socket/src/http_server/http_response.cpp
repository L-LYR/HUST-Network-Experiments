#include "http_response.hpp"

#include <fstream>
#include <iostream>

#include "http_protocol.hpp"

void httpResponse::respondTo(const httpRequest& req) {
    cleanup();

    if (req.headers.count("Method") == 0 || req.headers.count("Version") == 0) {
        _statusLine = getResponseStatus(BAD_REQUEST) + CRLF;
        _respLen = _statusLine.length();
        memcpy(_resp, _statusLine.c_str(), _respLen);
        return;
    }
    std::string method = req.headers.at("Method");
    if (method == "GET" && req.headers.count("Document") == 1) {
        if (respondGetOpt(req.headers.at("Document"))) {
            _statusLine = getResponseStatus(OK);
        } else {
            _statusLine = getResponseStatus(NOT_FOUND);
            _respLen = _statusLine.length() + _headerLines.length() + CRLF.length();
            memcpy(_resp, (_statusLine + _headerLines + CRLF).c_str(), _respLen);
            return;
        }
    } else {
        _statusLine = getResponseStatus(BAD_REQUEST) + CRLF;
        _respLen = _statusLine.length();
        memcpy(_resp, _statusLine.c_str(), _respLen);
        return;
    }

    _respLen = _statusLine.length() + _headerLines.length() + CRLF.length();
    memcpy(_resp, (_statusLine + _headerLines + CRLF).c_str(), _respLen);
    memcpy(_resp + _respLen, _entityBody, _entityLen);
    _respLen += _entityLen;
}

static bool endWith(const std::string& s, const std::string& suf) {
    if (s.length() < suf.length()) return false;
    for (int i = suf.length() - 1, j = s.length() - 1; i >= 0; i--, j--) {
        if (s[j] != suf[i]) return false;
    }
    return true;
}

static std::string getDocType(const std::string& doc) {
    if (endWith(doc, ".html") || endWith(doc, ".htm")) {
        return "text/html";
    } else if (endWith(doc, ".txt")) {
        return "text/plain";
    } else if (endWith(doc, ".jpg") || endWith(doc, ".jpeg") || endWith(doc, ".jpe")) {
        return "image/jpeg";
    } else if (endWith(doc, ".gif")) {
        return "image/gif";
    } else if (endWith(doc, ".bmp")) {
        return "image/bmp";
    } else if (endWith(doc, ".ico")) {
        return "image/x-icon";
    } else if (endWith(doc, ".mp4")) {
        return "video/mp4";
    } else {
        return "application/octet-stream";
    }
}

bool httpResponse::respondGetOpt(const std::string& target) {
    _entityLen = setEntity(target);
    if (_entityLen > 0) {
        setHeader("Server", _serverName);
        setHeader("Content-Type", getDocType(target));
        setHeader("Content-Length", std::to_string(_entityLen));
        return true;
    } else if (_entityLen == -2) {
        // std::ifstream fin(_path + "404.html", std::ios_base::binary | std::ios_base::in);
        // memset(_entityBody, 0, BUFSIZE);
        // fin.read(_entityBody, BUFSIZE);
        // _entityLen = fin.gcount();

        setHeader("Server", _serverName);
        setHeader("Content-Type", "text/html");
        setHeader("Content-Length", std::to_string(0));
        return false;
    } else {
        return false;
    }
}

void httpResponse::setHeader(const std::string& key, const std::string& value) {
    _headerLines += key + ": " + value + CRLF;
}

int httpResponse::setEntity(const std::string& docRePath) {
    std::ifstream fin(_path + docRePath, std::ios_base::binary | std::ios_base::in);
    memset(_entityBody, 0, BUFSIZE);
    if (!fin) {
        return -2;
    }
    fin.read(_entityBody, BUFSIZE);
    if (!fin.bad()) {
        return fin.gcount();
    } else {
        return -1;
    }
}

const char* httpResponse::fullResponse(void) {
    return _resp;
}

int httpResponse::getLength(void) {
    return _respLen;
}

void httpResponse::cleanup(void) {
    _statusLine.clear();
    _headerLines.clear();
    memset(_resp, 0, BUFSIZE);
    memset(_entityBody, 0, BUFSIZE);
}