#pragma once
#include <string>
#include <vector>
#include "ExecutionReport.h"
#include "InterfaceReportWriter.h"

class CSVWriter : public IReportWriter
{
public:
    // Constructor
    explicit CSVWriter(const std::string &file);

    // Copy constructor and assignment (Rule of Three)
    CSVWriter(const CSVWriter &other);
    CSVWriter &operator=(const CSVWriter &other);

    // Destructor
    ~CSVWriter();

private:
    std::string filename;

public:
    void write_reports(const std::vector<ExecutionReport> &reports) override;
};