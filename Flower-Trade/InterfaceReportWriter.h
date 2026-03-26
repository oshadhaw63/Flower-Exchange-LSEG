#pragma once
#include <vector>
#include "ExecutionReport.h"

class IReportWriter {
public:
    // A virtual destructor is required for C++ interfaces to prevent memory leaks
    virtual ~IReportWriter() = default;

    // The contract: Any class that implements this MUST have this exact function
    virtual void write_reports(const std::vector<ExecutionReport>& reports) = 0;
};