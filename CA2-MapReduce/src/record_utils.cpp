#include "../lib/record_utils.hpp"

int util_calculate_monthly_usage(const std::vector<Record*>& records, int month) {
    int monthly_usage = 0;
    for (const Record* record : records)
        if (record->month == month)
            for (int usage : record->usages) 
                monthly_usage += usage;

    return monthly_usage;
}

int util_calculate_max_usage_hour(const std::vector<Record*>& records, int month) {
    std::vector<int> monthly_usage = std::vector<int>(6, 0);

    for (const Record* record : records)
        if (record->month == month)
            for (int hour = 0; hour < record->usages.size(); hour++) 
                monthly_usage[hour] += record->usages[hour];

    auto it = std::max_element(monthly_usage.begin(), monthly_usage.end());
    int max_usage_hour = std::distance(monthly_usage.begin(), it);

    return max_usage_hour;
}

double util_calculate_avg_usage(const std::vector<Record*>& records, int month) {
    int usage = util_calculate_monthly_usage(records, month);
    return (1.0 * usage) / (1.0 * MONTH_DAYS);
}