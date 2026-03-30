#include "Exchange.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <thread> // Disabled due to MinGW compatibility issues
#include <fstream>
// #include <mutex> // Disabled due to MinGW compatibility issues

#pragma comment(lib, "ws2_32.lib")

Exchange global_engine;
// std::mutex file_lock; // Removed - Disabled due to MinGW compatibility issues

void handle_client(SOCKET client_socket)
{
    char buffer[4096];
    std::string incoming_data = "";

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0)
            break;

        incoming_data += std::string(buffer, bytes_read);
        size_t pos;
        while ((pos = incoming_data.find('\n')) != std::string::npos)
        {
            std::string line = incoming_data.substr(0, pos);
            incoming_data.erase(0, pos + 1);

            if (line.empty() || line == "\r")
                continue;

            // 1. Handle UI Reset
            if (line == "RESET")
            {
                global_engine = Exchange();
                std::cout << "\n[SYSTEM] UI Bulk Upload Detected. Engine memory wiped." << std::endl;

                // Wipe the physical output file for the new test
                // Removed std::lock_guard<std::mutex> lock(file_lock); due to MinGW
                std::ofstream file("execution_rep.csv", std::ios::out | std::ios::trunc);
                file << "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";
                continue;
            }

            // 2. Parse the UI packet
            std::stringstream ss(line);
            RawOrder raw;
            std::getline(ss, raw.client_id, '|');
            std::getline(ss, raw.instrument, '|');
            std::getline(ss, raw.side, '|');
            std::getline(ss, raw.qty, '|');
            std::getline(ss, raw.price, '|');

            // --- LIVE TERMINAL LOGGING ADDED HERE ---
            std::cout << "[UI ORDER RECV] Client: " << raw.client_id
                      << " | Asset: " << raw.instrument
                      << " | Qty: " << raw.qty
                      << " @ $" << raw.price << std::endl;

            // 3. Process matching logic
            std::vector<ExecutionReport> reports = global_engine.process_order(raw);

            // 4. Send back to UI and write to file
            for (const auto &rep : reports)
            {
                std::string status_str = (rep.status == 0) ? "New" : (rep.status == 1) ? "Rejected"
                                                                 : (rep.status == 2)   ? "Fill"
                                                                                       : "Pfill";

                // Send to React
                std::stringstream out;
                out << rep.order_id << "|" << rep.client_order_id << "|" << rep.instrument << "|"
                    << rep.side << "|" << status_str << "|" << rep.quantity << "|"
                    << rep.price << "|" << rep.reason << "|" << rep.transaction_time << "\n";
                std::string packet = out.str();
                send(client_socket, packet.c_str(), packet.size(), 0);

                // --- LIVE FILE WRITING ADDED HERE ---
                // Removed std::lock_guard<std::mutex> lock(file_lock);
                std::ofstream file("execution_rep.csv", std::ios::app);
                if (file.is_open())
                {
                    file << rep.order_id << "," << rep.client_order_id << "," << rep.instrument << ","
                         << rep.side << "," << status_str << "," << rep.quantity << ","
                         << rep.price << "," << rep.reason << "," << rep.transaction_time << "\n";
                }
            }
        }
    }
    closesocket(client_socket);
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);

    // Safety check to ensure port isn't stuck
    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cout << "[ERROR] Port 8080 is blocked! Close all other terminals and try again." << std::endl;
        return 1;
    }

    listen(server_fd, SOMAXCONN);
    std::cout << "[GUI SERVER] Online and listening on port 8888..." << std::endl;

    // Create the physical file on boot
    std::ofstream file("execution_rep.csv", std::ios::out | std::ios::trunc);
    file << "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";
    file.close();

    while (true)
    {
        SOCKET client = accept(server_fd, nullptr, nullptr);
        if (client != INVALID_SOCKET)
        {
            // Removed std::thread due to MinGW compatibility issues
            // Single-threaded fallback
            handle_client(client);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}