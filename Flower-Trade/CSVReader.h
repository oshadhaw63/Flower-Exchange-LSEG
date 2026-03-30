#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>

struct RawOrder
{
    std::string client_id;
    std::string instrument;
    std::string side;
    std::string qty;
    std::string price;
};

class CSVReader
{
private:
    std::string filename;

public:
    // 1. Standard Constructor
    explicit CSVReader(const std::string &file);

    // 2. Copy Constructor
    CSVReader(const CSVReader &other);

    // 3. Copy Assignment Operator
    CSVReader &operator=(const CSVReader &other);

    // 4. Destructor
    ~CSVReader();

    std::vector<RawOrder> read_orders() const; // Notice the 'const' here too!
};

#endif