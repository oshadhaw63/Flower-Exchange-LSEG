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
#include <fstream>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

class ExchangeServer {
private:
    int listen_port;
    SOCKET main_socket; 
    
    std::atomic<int> order_id_counter;

    std::unordered_map<std::string, std::unique_ptr<OrderBook>> order_books;

    bool boot_winsock(); 

    void handle_trader(SOCKET trader_socket);

    std::string generate_order_id();
    std::string get_timestamp();
    std::ofstream output_file;
    std::mutex file_lock;

public:
    ExchangeServer(int port);
    ~ExchangeServer();

    bool boot_up();          
    void wait_for_traders(); 
};