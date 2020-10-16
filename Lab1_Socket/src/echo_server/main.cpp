#include <iostream>

#include "echo_server.hpp"

int main(void) {
    echoServer test(27015, "localhost");

    test.run();

    return 0;
}