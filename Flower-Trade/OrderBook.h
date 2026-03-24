#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <string>
#include <map>
#include <queue>
#include <vector>
#include <mutex> // NEW: The thread-safety locking mechanism
#include "Order.h"
#include "ExecutionReport.h"

class OrderBook {
private:
    std::string instrument_name;

    // The Red-Black Trees for lightning-fast price sorting
    std::map<double, std::queue<Order>, std::greater<double>> buy_side;
    std::map<double, std::queue<Order>> sell_side;

    // The Lock
    std::mutex book_lock;

public:
    OrderBook(std::string inst);
    
    // Default constructor
    OrderBook();

    // The matching engine
    std::vector<ExecutionReport> process_incoming_order(Order new_order, std::string current_time);
};

#endif // ORDERBOOK_H