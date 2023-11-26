#include "../lib/building.hpp"

void Building::save_records(char* encoded_records, ResourceType source) {
    switch (source) {
        case (ResourceType::GAS):
            gas_records = RecordSerializer::decode(encoded_records);
            break;
        case (ResourceType::ELEC):
            elec_records = RecordSerializer::decode(encoded_records);
            break;
        case (ResourceType::WATER):
            water_records = RecordSerializer::decode(encoded_records);
            break;
        
        default:
            break;
    }
}

const std::vector<Record*>& Building::select_record_type(ResourceType source) {
    switch (source) {
        case (ResourceType::GAS):
            return gas_records;
            break;
        case (ResourceType::ELEC):
            return elec_records;
            break;
        case (ResourceType::WATER):
            return water_records;
            break;
        
        default:
            throw std::invalid_argument("Invalid source type");
    }
}

int Building::calculate_monthly_usage(ResourceType source, int month) {
    return util_calculate_monthly_usage(select_record_type(source), month);

    // const std::vector<Record*>& records = select_record_type(source);
    // int monthly_usage = 0;
    // for (const Record* record : records)
    //     if (record->month == month)
    //         for (int usage : record->usages) 
    //             monthly_usage += usage;

    // return monthly_usage;
}

int Building::calculate_max_usage_hour(ResourceType source, int month) {
    return util_calculate_max_usage_hour(select_record_type(source), month);
    // const std::vector<Record*>& records = select_record_type(source);
    // std::vector<int> monthly_usage = std::vector<int>(6, 0);

    // for (const Record* record : records)
    //     if (record->month == month)
    //         for (int hour = 0; hour < record->usages.size(); hour++) 
    //             monthly_usage[hour] += record->usages[hour];

    // auto it = std::max_element(monthly_usage.begin(), monthly_usage.end());
    // int max_usage_hour = std::distance(monthly_usage.begin(), it);

    // return max_usage_hour;
}

double Building::calculate_avg_usage(ResourceType source, int month) {
    int usage = this->calculate_monthly_usage(source, month);
    return (1.0 * usage) / (1.0 * MONTH_DAYS);
}

double Building::calculate_diff_max_avg(ResourceType source, int month) {
    int max_hour = this->calculate_max_usage_hour(source, month);
    const std::vector<Record*>& records = select_record_type(source);
    
    double max_hour_usage = 0;
    double avg_usage = this->calculate_avg_usage(source, month);
    for (const Record* record : records) 
        if (record->month == month)
            max_hour_usage += record->usages[max_hour];
        
    return max_hour_usage - avg_usage;
}
