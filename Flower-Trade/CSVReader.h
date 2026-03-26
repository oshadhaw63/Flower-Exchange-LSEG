#pragma once
#include <string>
#include <vector>

struct RawOrder {
    std::string client_id;
    std::string instrument;
    std::string side;
    std::string qty;
    std::string price;
};

class CSVReader {
// Constructor
    explicit CSVReader(const std::string& file);

    // Copy constructor and assignment (Rule of Three)
    CSVReader(const CSVReader& other);
    CSVReader& operator=(const CSVReader& other);

    // Destructor
    ~CSVReader();

private:
    std::string filename;
public:
    CSVReader(std::string file);
    std::vector<RawOrder> read_orders();
};