#include "OrderBook.h"
#include <algorithm>

OrderBook::OrderBook(std::string inst) : instrument_name(std::move(inst)) {}

OrderBook::OrderBook() {
    instrument_name = "Unknown";
}

std::vector<ExecutionReport> OrderBook::process_incoming_order(Order new_order, std::string current_time) {

    std::lock_guard<std::mutex> lock(book_lock);

    std::vector<ExecutionReport> generated_receipts;
    bool trade_occurred = false; 

    if (new_order.side == 1) { 
        while (new_order.quantity > 0 && !sell_side.empty()) {
            auto cheapest_sell = sell_side.begin(); 
            double sell_price = cheapest_sell->first;

            if (sell_price <= new_order.price) { 
                trade_occurred = true;
                Order& resting_order = cheapest_sell->second.front(); 
                
                double execution_price = resting_order.price; 
                int trade_qty = std::min(new_order.quantity, resting_order.quantity);
      
                int agg_status = (new_order.quantity == trade_qty) ? 2 : 3; //2=Filled, 3=Partially Filled
                Order agg_rep_order = new_order;
                agg_rep_order.quantity = trade_qty;
                agg_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(agg_rep_order, agg_status, current_time));

                int rest_status = (resting_order.quantity == trade_qty) ? 2 : 3; 
                Order rest_rep_order = resting_order;
                rest_rep_order.quantity = trade_qty;
                rest_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(rest_rep_order, rest_status, current_time));

                new_order.quantity -= trade_qty;
                resting_order.quantity -= trade_qty;

                if (resting_order.quantity == 0) {
                    cheapest_sell->second.pop(); 
                    if (cheapest_sell->second.empty()) {
                        sell_side.erase(cheapest_sell); 
                    }
                }
            } else {
                break; 
            }
        }
        
        if (new_order.quantity > 0) {
            buy_side[new_order.price].push(new_order);
            if (!trade_occurred) {
                generated_receipts.push_back(ExecutionReport(new_order, 0, current_time)); // 0 = New
            }
        }
    } 
    // --- SELL ORDER LOGIC ---
    else if (new_order.side == 2) { 
        while (new_order.quantity > 0 && !buy_side.empty()) {
            auto highest_buy = buy_side.begin();
            double buy_price = highest_buy->first;

            if (buy_price >= new_order.price) { 
                trade_occurred = true;
                Order& resting_order = highest_buy->second.front();
                
                double execution_price = resting_order.price;
                int trade_qty = std::min(new_order.quantity, resting_order.quantity);

                int agg_status = (new_order.quantity == trade_qty) ? 2 : 3;
                Order agg_rep_order = new_order;
                agg_rep_order.quantity = trade_qty;
                agg_rep_order.price = execution_price;
                generated_receipts.push_back(ExecutionReport(agg_rep_order, agg_status, current_time));

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

    return generated_receipts; 
}