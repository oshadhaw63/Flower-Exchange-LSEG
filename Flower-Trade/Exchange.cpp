#include "Exchange.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Exchange::Exchange() {
    order_id_counter = 1;
    order_books["Rose"] = std::make_unique<OrderBook>("Rose");
    order_books["Lavender"] = std::make_unique<OrderBook>("Lavender");
    order_books["Lotus"] = std::make_unique<OrderBook>("Lotus");
    order_books["Tulip"] = std::make_unique<OrderBook>("Tulip");
    order_books["Orchid"] = std::make_unique<OrderBook>("Orchid");
}

std::string Exchange::generate_order_id() {
    return "ord" + std::to_string(order_id_counter++);
}

std::string Exchange::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d-%H%M%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

#include "OrderValidator.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Exchange::Exchange() {
// ... existing code ...
std::vector<ExecutionReport> Exchange::process_order(const RawOrder& raw) {
    std::vector<ExecutionReport> reports;
    std::string reject_reason = "";
    
    if (!OrderValidator::isValid(raw, reject_reason)) {
        int side = 0;
        try { side = std::stoi(raw.side); } catch (...) {}
        int qty = 0;
        try { qty = std::stoi(raw.qty); } catch (...) {}
        double price = 0.0;
        try { price = std::stod(raw.price); } catch (...) {}

        Order rejected_order(raw.client_id, raw.instrument, side, qty, price);
        rejected_order.order_id = generate_order_id();
        reports.push_back(ExecutionReport(rejected_order, 1, get_timestamp(), reject_reason));
    } else {
        Order new_order(raw.client_id, raw.instrument, std::stoi(raw.side), std::stoi(raw.qty), std::stod(raw.price));
        new_order.order_id = generate_order_id();
        reports = order_books.at(raw.instrument)->process_incoming_order(new_order, get_timestamp());
    }

    return reports;
}