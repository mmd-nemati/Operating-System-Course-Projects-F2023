#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <map>
#include <vector>

constexpr int BUFFER_SIZE = 1024;


const std::string WHOLE_USAGE_REPORT = std::string("whole-usage");
const std::string AVERAGE_USAGE_REPORT = std::string("average-usage");
const std::string MAX_USAGE_HOUR_REPORT = std::string("max-usage-hour");
const std::string BILLS_REPORT = std::string("bills");
const std::string DIFF_MAX_AVG_REPORT = std::string("diff-max-avg");
const std::string KILL_CMD = std::string("kill");

const std::string PROMPTS_DELIMITER = std::string("------------------------------------------------------------------------\n");
const std::string GET_BUILDINGS_PROMPT = std::string(">> Enter building name:\n");
const std::string GET_RESOURCES_PROMPT = std::string(">> Enter one or more resources, separated by '/' [gas | water | electricity]:\n");
const std::string GET_REPORT_PROMPT = std::string(">> Enter only one of the reports [whole-usage | average-usage | max-usage-hour | bills | diff-max-avg]:\n");
const std::string GET_MONTH_PROMPT = std::string(">> Enter one month for the report:\n");

const std::string PATH_TO_BULDING_PROGRAM = std::string("building.out");
const std::string PATH_TO_RESOURCE_PROGRAM = std::string("resource_counter.out");
const std::string PATH_TO_BILLS_PROGRAM = std::string("bills.out");

const std::string BILLS_SERVER = std::string("billpipe");
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
    GAS,
    WATER,
    ELEC
};

struct RequestBillsData {
    std::string server_name;
    int month;
    ResourceType type;
    std::string records;
};

struct BuildingRequestData {
    int month;
    std::string report;
    std::vector<std::string> resources;
};

#endif