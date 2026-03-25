#include "Exchange.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

// The global core engine
Exchange global_engine; 

void handle_client(SOCKET client_socket) {
    char buffer[4096];
    std::string incoming_data = "";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) break;

        incoming_data += std::string(buffer, bytes_read);
        size_t pos;
        while ((pos = incoming_data.find('\n')) != std::string::npos) {
            std::string line = incoming_data.substr(0, pos);
            incoming_data.erase(0, pos + 1);

            if (line.empty() || line == "\r") continue;

            // Handle the UI Reset button instantly
            if (line == "RESET") {
                global_engine = Exchange(); 
                std::cout << "Engine reset triggered from UI." << std::endl;
                continue;
            }

            // Parse incoming UI data into a RawOrder
            std::stringstream ss(line);
            RawOrder raw;
            std::getline(ss, raw.client_id, '|');
            std::getline(ss, raw.instrument, '|');
            std::getline(ss, raw.side, '|');
            std::getline(ss, raw.qty, '|');
            std::getline(ss, raw.price, '|');

            // Feed it to the core engine
            std::vector<ExecutionReport> reports = global_engine.process_order(raw);

            // Send reports back to React
            for (const auto& rep : reports) {
                std::string status_str = (rep.status == 0) ? "New" : (rep.status == 1) ? "Rejected" : (rep.status == 2) ? "Fill" : "Pfill";
                std::stringstream out;
                out << rep.order_id << "|" << rep.client_order_id << "|" << rep.instrument << "|"
                    << rep.side << "|" << status_str << "|" << rep.quantity << "|"
                    << rep.price << "|" << rep.reason << "|" << rep.transaction_time << "\n";
                
                std::string packet = out.str();
                send(client_socket, packet.c_str(), packet.size(), 0);
            }
        }
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, SOMAXCONN);

    std::cout << "[GUI SERVER] Online and listening on port 8080..." << std::endl;

    while (true) {
        SOCKET client = accept(server_fd, nullptr, nullptr);
        if (client != INVALID_SOCKET) {
            std::thread(handle_client, client).detach();
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}