#include "OrderValidator.h"

bool OrderValidator::isValid(const RawOrder& raw, std::string& reject_reason) {
    if (raw.client_id.empty() || raw.instrument.empty() || raw.side.empty() || raw.qty.empty() || raw.price.empty()) {
        reject_reason = "Missing required field";
        return false;
    }

    int side;
    int qty;
    double price;

    try {
        side = std::stoi(raw.side);
        qty = std::stoi(raw.qty);
        price = std::stod(raw.price);
    } catch (...) {
        reject_reason = "Invalid format";
        return false;
    }

    if (raw.instrument != "Rose" && raw.instrument != "Lavender" && raw.instrument != "Lotus" && raw.instrument != "Tulip" && raw.instrument != "Orchid") {
        reject_reason = "Invalid instrument";
        return false;
    }
    
    if (side != 1 && side != 2) {
        reject_reason = "Invalid side";
        return false;
    }
    
    if (price <= 0) {
        reject_reason = "Invalid price";
        return false;
    }
    
    if (qty < 10 || qty > 1000) {
        reject_reason = "Invalid size";
        return false;
    }
    
    if (qty % 10 != 0) {
        reject_reason = "Invalid size";
        return false;
    }

    return true;
}