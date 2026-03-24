#pragma once
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <atomic>
#include "OrderBook.h"

#pragma comment(lib, "ws2_32.lib")

class ExchangeServer {
private:
    int listen_port;
    SOCKET main_socket; 
    
    // Thread-safe order ID generator
    std::atomic<int> order_id_counter;

    // The 5 Order Books
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> order_books;

    bool boot_winsock(); 
    
    // The background worker function for each connected trader
    void handle_trader(SOCKET trader_socket);

    // Helper to generate IDs
    std::string generate_order_id();
    std::string get_timestamp();

public:
    ExchangeServer(int port);
    ~ExchangeServer();

    bool boot_up();          
    void wait_for_traders(); 
};