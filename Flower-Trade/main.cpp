#include "CSVReader.h"
#include "InterfaceReportWriter.h"
#include "CSVWriter.h"
#include "Exchange.h"
#include <iostream>
#include <chrono>
#include <memory>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_orders.csv>" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    std::string input_filepath = argv[1];
    CSVReader reader(input_filepath);
    std::vector<RawOrder> raw_orders = reader.read_orders();

    Exchange engine;
    std::vector<ExecutionReport> all_reports;
    all_reports.reserve(raw_orders.size()); 

    for (const auto& raw : raw_orders) {
        std::vector<ExecutionReport> reports = engine.process_order(raw);
        all_reports.insert(all_reports.end(), reports.begin(), reports.end());
    }

    std::filesystem::path p(input_filepath);
    std::string output_filename = p.stem().string() + "_execution_rep.csv";

    std::unique_ptr<IReportWriter> writer = std::make_unique<CSVWriter>(output_filename);
    writer->write_reports(all_reports);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end_time - start_time;
    
    std::cout << "Successfully processed " << raw_orders.size() << " orders.\n";
    std::cout << "Execution Time: " << ms.count() << " ms\n";
    std::cout << "Results saved to " << output_filename << std::endl;

    return 0;
}