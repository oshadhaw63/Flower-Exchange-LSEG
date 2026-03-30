#include "CSVWriter.h"
#include <fstream>
#include <iomanip>

CSVWriter::CSVWriter(const std::string &file) : filename(file) {}

// Copy constructor
CSVWriter::CSVWriter(const CSVWriter &other) : filename(other.filename) {}

// Copy assignment operator
CSVWriter &CSVWriter::operator=(const CSVWriter &other)
{
    if (this != &other)
    {
        filename = other.filename;
    }
    return *this;
}

// Destructor
CSVWriter::~CSVWriter() {}

void CSVWriter::write_reports(const std::vector<ExecutionReport> &reports)
{
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open())
        return;

    file << "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";

    for (const auto &rep : reports)
    {
        std::string status_str;
        if (rep.status == 0)
            status_str = "New";
        else if (rep.status == 1)
            status_str = "Rejected";
        else if (rep.status == 2)
            status_str = "Fill";
        else if (rep.status == 3)
            status_str = "Pfill";

        file << rep.order_id << ","
             << rep.client_order_id << ","
             << rep.instrument << ","
             << rep.side << ","
             << status_str << ","
             << rep.quantity << ","
             << std::fixed << std::setprecision(2) << rep.price << ","
             << rep.reason << ","
             << rep.transaction_time << "\n";
    }
    file.close();
}