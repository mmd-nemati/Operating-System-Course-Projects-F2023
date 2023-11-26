#ifndef __RECORD_UTILS_HPP__
#define __RECORD_UTILS_HPP__

#include <algorithm>

#include "record.hpp"
#include "types.hpp"

int util_calculate_monthly_usage(const std::vector<Record*>& records, int month);
int util_calculate_max_usage_hour(const std::vector<Record*>& records, int month);
double util_calculate_avg_usage(const std::vector<Record*>& records, int month);

#endif