#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <string>
#include <map>
#include <queue>
#include <vector>
#include <mutex> 
#include "Order.h"
#include "ExecutionReport.h"

class OrderBook {
private:
    std::string instrument_name;

    // Red-Black Trees for sorting
    std::map<double, std::queue<Order>, std::greater<double>> buy_side;
    std::map<double, std::queue<Order>> sell_side;

    std::mutex book_lock;

public:
    explicit OrderBook(std::string inst);
    
    // Default constructor
    OrderBook();

    // The matching engine
    std::vector<ExecutionReport> process_incoming_order(Order new_order, std::string current_time);
};

#endif 