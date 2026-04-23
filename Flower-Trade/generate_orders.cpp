#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>

int main() {
    std::ofstream file("orders.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return 1;
    }

    file << "Client Order ID,Instrument,Side,Quantity,Price" << std::endl;

    std::vector<std::string> instruments = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> side_dist(1, 2);
    std::uniform_int_distribution<> quantity_dist(1, 100);
    std::uniform_real_distribution<> price_dist(10.0, 50.0);
    std::uniform_int_distribution<> instrument_dist(0, instruments.size() - 1);

    for (int i = 1; i <= 1000; ++i) {
        file << i << ","
             << instruments[instrument_dist(gen)] << ","
             << side_dist(gen) << ","
             << quantity_dist(gen) * 10 << ","
             << std::fixed << std::setprecision(2) << price_dist(gen) << std::endl;
    }

    file.close();
    std::cout << "Generated orders.csv with 1000 orders." << std::endl;

    return 0;
}
