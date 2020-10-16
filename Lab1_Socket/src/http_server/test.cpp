#include <iostream>

#include "http_request.hpp"
#include "http_response.hpp"

std::string request =
    "GET /index.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: Mozilla/5.0(Windows NT 5.1; rv: 10.0.2)Gecko/20100101 Firefox/10.0.2\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,/;q=0.8\r\n"
    "Accept-Language: zh-cn,zh;q=0.5\r\n"
    "Accept-Encoding: gzip,deflate\r\n"
    "Connection: keep-alive\r\n"
    "Referer: http://localhost/\r\n"
    "Content-Length: 25\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n";

int main(void) {
    httpRequest r(request);
    std::cout << "Request Line: "
              << r._requestLine << std::endl
              << std::endl
              << "Header Lines:\n"
              << r._headerLines << std::endl
              << std::endl
              << "Entity Body:\n"
              << r._entityBody << std::endl
              << std::endl;

    for (auto &i : r.headers) {
        std::cout << i.first << ": " << i.second << std::endl;
    }

    httpResponse resp(".", "myServer");
    resp.respondTo(request);
    std::cout << std::endl
              << "Status Line:\n"
              << resp._statusLine << std::endl
              << std::endl
              << "Headers Lines:\n"
              << resp._headerLines << std::endl
              << std::endl
              << "Entity Body:\n"
              << resp._entityBody << std::endl
              << std::endl;

    std::cout << std::endl
              << "Full Response:\n"
              << resp.fullResponse() << std::endl;

    return 0;
}