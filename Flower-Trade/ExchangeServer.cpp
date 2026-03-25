#include "ExchangeServer.h"
#include <sstream>
#include <chrono>
#include <iomanip>

ExchangeServer::ExchangeServer(int port) {
    listen_port = port;
    main_socket = INVALID_SOCKET;
    order_id_counter = 1;

    order_books["Rose"] = std::make_unique<OrderBook>("Rose");
    order_books["Lavender"] = std::make_unique<OrderBook>("Lavender");
    order_books["Lotus"] = std::make_unique<OrderBook>("Lotus");
    order_books["Tulip"] = std::make_unique<OrderBook>("Tulip");
    order_books["Orchid"] = std::make_unique<OrderBook>("Orchid");

    output_file.open("output.csv", std::ios::out);
    if (output_file.is_open()) {
        output_file << "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";
    }
}

bool ExchangeServer::boot_winsock() {
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

bool ExchangeServer::boot_up() {
    if (!boot_winsock()) return false;

    main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (main_socket == INVALID_SOCKET) return false;

    sockaddr_in server_rules;
    server_rules.sin_family = AF_INET;
    server_rules.sin_port = htons(listen_port);
    server_rules.sin_addr.s_addr = INADDR_ANY;

    if (bind(main_socket, (sockaddr*)&server_rules, sizeof(server_rules)) == SOCKET_ERROR) return false;
    if (listen(main_socket, SOMAXCONN) == SOCKET_ERROR) return false;

    std::cout << "[EXCHANGE] Online. Listening on port " << listen_port << "..." << std::endl;
    return true;
}

std::string ExchangeServer::generate_order_id() {
    return "ord" + std::to_string(order_id_counter.fetch_add(1)); 
}

std::string ExchangeServer::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d-%H%M%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void ExchangeServer::handle_trader(SOCKET trader_socket) {
    char buffer[4096];
    std::string incoming_data = "";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(trader_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            std::cout << "Trader disconnected." << std::endl;
            break;
        }

        incoming_data += std::string(buffer, bytes_read);

        size_t pos;
        while ((pos = incoming_data.find('\n')) != std::string::npos) {
            std::string single_line = incoming_data.substr(0, pos);
            incoming_data.erase(0, pos + 1);

            if (single_line.empty() || single_line == "\r") continue;

            if (single_line == "RESET") {
                order_books["Rose"] = std::make_unique<OrderBook>("Rose");
                order_books["Lavender"] = std::make_unique<OrderBook>("Lavender");
                order_books["Lotus"] = std::make_unique<OrderBook>("Lotus");
                order_books["Tulip"] = std::make_unique<OrderBook>("Tulip");
                order_books["Orchid"] = std::make_unique<OrderBook>("Orchid");
                order_id_counter = 1;

                std::lock_guard<std::mutex> lock(file_lock);
                if (output_file.is_open()) output_file.close();
                output_file.open("output.csv", std::ios::out | std::ios::trunc);
                if (output_file.is_open()) {
                    output_file << "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";
                }
                std::cout << "Engine memory and OrderBooks wiped for new scenario." << std::endl;
                continue;
            }

            std::stringstream ss(single_line);
            std::string c_id, inst, side_str, qty_str, price_str;
            
            std::getline(ss, c_id, '|');
            std::getline(ss, inst, '|');
            std::getline(ss, side_str, '|');
            std::getline(ss, qty_str, '|');
            std::getline(ss, price_str, '|');

            try {
                int side = std::stoi(side_str);
                int qty = std::stoi(qty_str);
                double price = std::stod(price_str);

                Order new_order(c_id, inst, side, qty, price);
                new_order.order_id = generate_order_id();
                std::string current_time = get_timestamp();

                std::vector<ExecutionReport> reports = order_books.at(inst)->process_incoming_order(new_order, current_time);

                for (const ExecutionReport& rep : reports) {
                    std::string receipt_packet = rep.serialize();
                    send(trader_socket, receipt_packet.c_str(), receipt_packet.size(), 0);

                    std::lock_guard<std::mutex> lock(file_lock);
                    if (output_file.is_open()) {
                        std::string status_str = (rep.status == 0) ? "New" : (rep.status == 1) ? "Rejected" : (rep.status == 2) ? "Fill" : "Pfill";
                        output_file << rep.order_id << "," << rep.client_order_id << "," << rep.instrument << "," 
                                    << rep.side << "," << status_str << "," << rep.quantity << "," 
                                    << std::fixed << std::setprecision(2) << rep.price << "," 
                                    << rep.reason << "," << rep.transaction_time << "\n";
                        output_file.flush();
                    }
                }
            } catch (...) {
                std::string err = "Error: Invalid packet format.\n";
                send(trader_socket, err.c_str(), err.size(), 0);
            }
        }
    }
    closesocket(trader_socket);
}

void ExchangeServer::wait_for_traders() {
    std::cout << "Waiting for traders..." << std::endl;
    while (true) {
        sockaddr_in trader_info;
        int trader_size = sizeof(trader_info);
        SOCKET trader_socket = accept(main_socket, (sockaddr*)&trader_info, &trader_size);
        
        if (trader_socket != INVALID_SOCKET) {
            std::cout << "New Trader connected! Spawning thread..." << std::endl;
            std::thread(&ExchangeServer::handle_trader, this, trader_socket).detach();
        }
    }
}

ExchangeServer::~ExchangeServer() {
    if (output_file.is_open()) {
        output_file.close();
    }
    closesocket(main_socket);
    WSACleanup();
}