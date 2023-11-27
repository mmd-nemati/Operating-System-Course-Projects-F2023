#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <map>

constexpr int BUFFER_SIZE = 1024;
const std::string BILLS_SERVER = std::string("bills");
const std::string BILLS_COEFF_FILE_PATH = std::string("bills.csv");
const std::string REQUEST_BILLS_PREFIX = std::string("request_bills\n");

constexpr int MONTH_DAYS = 30;
constexpr int WATER_COEFF_INDEX = 0;
constexpr int GAS_COEFF_INDEX = 1;
constexpr int ELEC_COEFF_INDEX = 2;
constexpr double WATER_OVERUSE_COEFF = 1.25;
constexpr double ELEC_OVERUSE_COEFF = 1.25;
constexpr double ELEC_UNDERUSE_COEFF = 0.75;

enum class ResourceType {
    WATER,
    GAS,
    ELEC
};

struct RequestBillsData {
    int month;
    ResourceType type;
    std::string records;
};


#endif