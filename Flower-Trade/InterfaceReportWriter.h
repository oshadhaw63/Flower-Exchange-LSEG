#pragma once
#include <vector>
#include "ExecutionReport.h"

class IReportWriter {
public:
    virtual ~IReportWriter() = default;

    virtual void write_reports(const std::vector<ExecutionReport>& reports) = 0;
};