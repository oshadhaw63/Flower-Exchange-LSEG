#include "CSVReader.h"
#include <fstream>
#include <sstream>

CSVReader::CSVReader(const std::string& file) : filename(file) {}

// Copy constructor
CSVReader::CSVReader(const CSVReader& other) : filename(other.filename) {}

// Copy assignment operator
CSVReader& CSVReader::operator=(const CSVReader& other) {
    if (this != &other) {
        filename = other.filename;
    }
    return *this;
}

// Destructor
CSVReader::~CSVReader() {}

std::vector<RawOrder> CSVReader::read_orders() {
    std::vector<RawOrder> orders;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) return orders;

    std::getline(file, line); 

    while (std::getline(file, line)) {
        if (line.empty() || line == "\r") continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(ss, token, ',')) {
            fields.push_back(token);
        }

        while (fields.size() < 5) {
            fields.push_back("");
        }

        RawOrder raw;
        raw.client_id = fields[0];
        raw.instrument = fields[1];
        raw.side = fields[2];
        raw.qty = fields[3];
        raw.price = fields[4];
        orders.push_back(raw);
    }
    return orders;
}