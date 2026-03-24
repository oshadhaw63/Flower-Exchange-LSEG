#ifndef ORDER_H
#define ORDER_H

#include <string>

class Order {
public:
    std::string client_order_id;
    std::string instrument;
    int side;       // 1 = Buy, 2 = Sell
    int quantity;
    double price;

    std::string order_id; // System generated
    std::string reason;
    std::string transaction_time;

    // Constructor
    Order(std::string c_id, std::string inst, int s, int qty, double pr);
    
    // Default constructor needed for standard library containers
    Order();
};

#endif // ORDER_H