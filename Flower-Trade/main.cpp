#include "CSVReader.h"
#include "CSVWriter.h"
#include "Exchange.h"
#include <iostream>
#include <chrono>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    CSVReader reader("orders.csv");
    std::vector<RawOrder> raw_orders = reader.read_orders();

    Exchange engine;
    std::vector<ExecutionReport> all_reports;
    all_reports.reserve(raw_orders.size()); 

    for (const auto& raw : raw_orders) {
        std::vector<ExecutionReport> reports = engine.process_order(raw);
        all_reports.insert(all_reports.end(), reports.begin(), reports.end());
    }

    CSVWriter writer("execution_rep.csv");
    writer.write_reports(all_reports);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end_time - start_time;
    
    std::cout << "Successfully processed " << raw_orders.size() << " orders.\n";
    std::cout << "Execution Time: " << ms.count() << " ms\n";
    std::cout << "Results saved to execution_rep.csv\n";

    return 0;
}