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

std::vector<ExecutionReport> Exchange::process_order(const RawOrder& raw) {
    std::vector<ExecutionReport> reports;
    std::string reject_reason = "";
    int side = 0;
    int qty = 0;
    double price = 0.0;
    bool is_valid = true;

    if (raw.client_id.empty() || raw.instrument.empty() || raw.side.empty() || raw.qty.empty() || raw.price.empty()) {
        is_valid = false;
        reject_reason = "Missing required field";
    }

    if (is_valid) {
        try {
            side = std::stoi(raw.side);
            qty = std::stoi(raw.qty);
            price = std::stod(raw.price);
        } catch (...) {
            is_valid = false;
            reject_reason = "Invalid format";
        }
    }

    if (is_valid) {
        if (raw.instrument != "Rose" && raw.instrument != "Lavender" && raw.instrument != "Lotus" && raw.instrument != "Tulip" && raw.instrument != "Orchid") {
            is_valid = false;
            reject_reason = "Invalid instrument";
        } else if (side != 1 && side != 2) {
            is_valid = false;
            reject_reason = "Invalid side";
        } else if (price <= 0) {
            is_valid = false;
            reject_reason = "Price must be greater than 0";
        } else if (qty < 10 || qty > 1000) {
            is_valid = false;
            reject_reason = "Quantity outside range (10-1000)";
        } else if (qty % 10 != 0) {
            is_valid = false;
            reject_reason = "Quantity not a multiple of 10";
        }
    }

    std::string current_time = get_timestamp();

    if (!is_valid) {
        Order rejected_order(raw.client_id, raw.instrument, side, qty, price);
        rejected_order.order_id = generate_order_id();
        reports.push_back(ExecutionReport(rejected_order, 1, current_time, reject_reason));
    } else {
        Order new_order(raw.client_id, raw.instrument, side, qty, price);
        new_order.order_id = generate_order_id();
        reports = order_books.at(raw.instrument)->process_incoming_order(new_order, current_time);
    }

    return reports;
}