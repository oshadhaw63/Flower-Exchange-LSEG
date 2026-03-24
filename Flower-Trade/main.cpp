#include <iostream>
#include "ExchangeServer.h"

int main() {
    std::cout << "--- Starting High-Level Flower Exchange Server ---" << std::endl;

    // 1. Define the port (8080 is the standard port for local development)
    int port = 8080;
    ExchangeServer exchange(port);

    // 2. Boot up the network and bind the port
    if (exchange.boot_up()) {
        // 3. Pause and wait for a trader to connect over TCP
        exchange.wait_for_traders();
    } else {
        std::cerr << "CRITICAL FAILURE: Could not start the exchange server." << std::endl;
        return 1;
    }

    std::cout << "Server shutting down." << std::endl;
    return 0;
}