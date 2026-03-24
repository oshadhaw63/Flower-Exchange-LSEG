#include "Order.h"

// Main Constructor
Order::Order(std::string c_id, std::string inst, int s, int qty, double pr) {
    client_order_id = c_id;
    instrument = inst;
    side = s;
    quantity = qty;
    price = pr;
    
    order_id = "";
    reason = "";
    transaction_time = "";
}

// Default Constructor
Order::Order() {
    client_order_id = "";
    instrument = "";
    side = 0;
    quantity = 0;
    price = 0.0;
}