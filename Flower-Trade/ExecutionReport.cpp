#include "ExecutionReport.h"
#include <sstream>
#include <iomanip>

ExecutionReport::ExecutionReport(const Order& order, int exec_status, std::string time, std::string rej_reason) {
    order_id = order.order_id;
    client_order_id = order.client_order_id;
    instrument = order.instrument;
    side = order.side;
    price = order.price;
    quantity = order.quantity; 
    status = exec_status;
    transaction_time = time;
    reason = rej_reason;
}

std::string ExecutionReport::serialize() const {
    std::string status_str;
    if (status == 0) status_str = "New";
    else if (status == 1) status_str = "Rejected";
    else if (status == 2) status_str = "Fill";
    else if (status == 3) status_str = "Pfill";

    std::stringstream ss;
    ss << order_id << "|" 
       << client_order_id << "|" 
       << instrument << "|" 
       << side << "|" 
       << status_str << "|" 
       << quantity << "|" 
       << std::fixed << std::setprecision(2) << price << "|" 
       << reason << "|" 
       << transaction_time << "\n";

    return ss.str();
}