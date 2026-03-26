#pragma once
#include <string>
#include "CSVReader.h"

class OrderValidator {
public:
    static bool isValid(const RawOrder& raw, std::string& reject_reason);
};