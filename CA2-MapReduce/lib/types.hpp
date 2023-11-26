#ifndef __TYPES_HPP__
#define __TYPES_HPP__

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


#endif