#pragma once
#include "OrderBook.h"
#include "ExecutionReport.h"
#include "CSVReader.h"
#include "OrderValidator.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class Exchange {
private:
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> order_books;
    int order_id_counter;

    std::string generate_order_id();
    std::string get_timestamp();

public:
    Exchange();
    std::vector<ExecutionReport> process_order(const RawOrder& raw);
};