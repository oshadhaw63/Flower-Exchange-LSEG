#ifndef EXECUTION_REPORT_H
#define EXECUTION_REPORT_H

#include <string>
#include "Order.h"

class ExecutionReport {
public:
    std::string order_id;
    std::string client_order_id;
    std::string instrument;
    int side;
    int status; 
    int quantity;
    double price;
    std::string reason;
    std::string transaction_time;

    ExecutionReport(const Order& order, int exec_status, std::string time, std::string rej_reason = "");

    std::string serialize() const;
};

#endif 