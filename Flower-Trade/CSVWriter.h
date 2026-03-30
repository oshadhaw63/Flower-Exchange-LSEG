#pragma once
#include "InterfaceReportWriter.h"
#include <string>
#include <vector>

class CSVWriter : public IReportWriter {
private:
    std::string filename;
public:
    explicit CSVWriter(const std::string& file);
    ~CSVWriter() override;

    void write_reports(const std::vector<ExecutionReport>& reports) override;
};