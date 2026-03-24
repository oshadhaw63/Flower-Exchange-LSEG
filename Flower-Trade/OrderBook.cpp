#include "OrderBook.h"
#include <algorithm>

OrderBook::OrderBook(std::string inst) {
    instrument_name = inst;
}

OrderBook::OrderBook() {
    instrument_name = "Unknown";
}

std::vector<ExecutionReport> OrderBook::process_incoming_order(Order new_order, std::string current_time) {
    // 1. LOCK THE BOOK! No other thread can process an order for this flower until this function finishes.
    std::lock_guard<std::mutex> lock(book_lock);

    std::vector<ExecutionReport> generated_receipts;
    bool trade_occurred = false; 

    // --- BUY ORDER LOGIC ---
    if (new_order.side == 1) { 
        // While the buyer wants flowers AND there are sellers available
        while (new_order.quantity > 0 && !sell_side.empty()) {
            auto cheapest_sell = sell_side.begin(); 
            double sell_price = cheapest_sell->first;

            if (sell_price <= new_order.price) { // Prices overlap! Match!
                trade_occurred = true;
                Order& resting_order = cheapest_sell->second.front(); 
                
                double execution_price = resting_order.price; // Price decided by resting order
                int trade_qty = std::min(new_order.quantity, resting_order.quantity);
                
                // Aggressive Order Receipt
                int agg_status = (new_order.quantity == trade_qty) ? 2 : 3; // 2=Fill, 3=Pfill
                Order agg_rep_order = new_order;
                agg_rep_order.quantity = trade_qty;
                agg_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(agg_rep_order, agg_status, current_time));

                // Resting Order Receipt
                int rest_status = (resting_order.quantity == trade_qty) ? 2 : 3; 
                Order rest_rep_order = resting_order;
                rest_rep_order.quantity = trade_qty;
                rest_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(rest_rep_order, rest_status, current_time));

                // Deduct quantities
                new_order.quantity -= trade_qty;
                resting_order.quantity -= trade_qty;

                // Remove seller from book if they are completely out of flowers
                if (resting_order.quantity == 0) {
                    cheapest_sell->second.pop(); 
                    if (cheapest_sell->second.empty()) {
                        sell_side.erase(cheapest_sell); 
                    }
                }
            } else {
                break; // Sell price is too high, stop looking
            }
        }
        
        // Put the leftover buy order into the book
        if (new_order.quantity > 0) {
            buy_side[new_order.price].push(new_order);
            if (!trade_occurred) {
                generated_receipts.push_back(ExecutionReport(new_order, 0, current_time)); // 0 = New
            }
        }
    } 
    // --- SELL ORDER LOGIC ---
    else if (new_order.side == 2) { 
        // While the seller has flowers AND there are buyers available
        while (new_order.quantity > 0 && !buy_side.empty()) {
            auto highest_buy = buy_side.begin();
            double buy_price = highest_buy->first;

            if (buy_price >= new_order.price) { // Prices overlap!
                trade_occurred = true;
                Order& resting_order = highest_buy->second.front();
                
                double execution_price = resting_order.price;
                int trade_qty = std::min(new_order.quantity, resting_order.quantity);
                
                // Aggressive Order Receipt
                int agg_status = (new_order.quantity == trade_qty) ? 2 : 3;
                Order agg_rep_order = new_order;
                agg_rep_order.quantity = trade_qty;
                agg_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(agg_rep_order, agg_status, current_time));

                // Resting Order Receipt
                int rest_status = (resting_order.quantity == trade_qty) ? 2 : 3;
                Order rest_rep_order = resting_order;
                rest_rep_order.quantity = trade_qty;
                rest_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(rest_rep_order, rest_status, current_time));

                new_order.quantity -= trade_qty;
                resting_order.quantity -= trade_qty;

                if (resting_order.quantity == 0) {
                    highest_buy->second.pop();
                    if (highest_buy->second.empty()) {
                        buy_side.erase(highest_buy);
                    }
                }
            } else {
                break; 
            }
        }

        if (new_order.quantity > 0) {
            sell_side[new_order.price].push(new_order);
            if (!trade_occurred) {
                generated_receipts.push_back(ExecutionReport(new_order, 0, current_time)); // 0 = New
            }
        }
    }

    // The lock is automatically released here when the function ends
    return generated_receipts; 
}