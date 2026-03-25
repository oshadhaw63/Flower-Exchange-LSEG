#pragma once
#include <string>
#include <vector>
#include "ExecutionReport.h"

class CSVWriter {
private:
    std::string filename;
public:
    CSVWriter(std::string file);
    void write_reports(const std::vector<ExecutionReport>& reports);
};