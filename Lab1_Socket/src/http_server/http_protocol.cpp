#include "http_protocol.hpp"

static void initStatusCodeDic(void) {
    statusCodeDic[OK] = "OK";

    statusCodeDic[BAD_REQUEST] = "Bad Request";
    statusCodeDic[NOT_FOUND] = "Not Found";

    statusCodeDic[NOT_IMPLEMENTED] = "Not Implemented";
}

std::string getResponseStatus(httpStatusCode sc) {
    if (statusCodeDic.empty()) initStatusCodeDic();
    return defaultVer + " " + std::to_string(sc) + " " + statusCodeDic[sc] + CRLF;
}