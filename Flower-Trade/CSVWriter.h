#pragma once
#include <string>
#include <vector>
#include "ExecutionReport.h"

class CSVWriter {
// Constructor
    explicit CSVWriter(const std::string& file);

    // Copy constructor and assignment (Rule of Three)
    CSVWriter(const CSVWriter& other);
    CSVWriter& operator=(const CSVWriter& other);

    // Destructor
    ~CSVWriter();

private:
    std::string filename;
public:
    CSVWriter(std::string file);
    void write_reports(const std::vector<ExecutionReport>& reports);
};