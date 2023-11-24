#include "../lib/building.hpp"

void Building::get_records(char* encoded_records, SourceType source) {
    switch (source) {
        case (SourceType::GAS):
            gas_records = RecordSerializer::decode(encoded_records);
            break;
        case (SourceType::ELEC):
            elec_records = RecordSerializer::decode(encoded_records);
            break;
        case (SourceType::WATER):
            water_records = RecordSerializer::decode(encoded_records);
            break;
        
        default:
            break;
    }
}

std::vector<Record*> Building::select_record_type(SourceType source) {
    std::vector<Record*> records;
    switch (source) {
        case (SourceType::GAS):
            records = gas_records;
            break;
        case (SourceType::ELEC):
            records = elec_records;
            break;
        case (SourceType::WATER):
            records = water_records;
            break;
        
        default:
            break;
    }

    return records;
}

std::map<int, int> Building::calculate_monthly_usage(SourceType source) {
    std::vector<Record*> records = select_record_type(source);

    std::map<int, int> monthly_usage;
    for (Record* record : records) {
        int daily_usage = 0;
        for (int usage : record->usages) {
            daily_usage += usage;
        }

        monthly_usage[record->month] += daily_usage;
    }

    return monthly_usage;
}

std::map<int, int> Building::calculate_max_usage_hour(SourceType source) {
    std::vector<Record*> records = select_record_type(source);
   std::map<int, std::vector<int>> monthly_usage;

    for (int month = 1; month <= 12; month++) {
        monthly_usage[month] = std::vector<int>(6, 0);
    }

    for (Record* record : records) {
        for (int hour = 0; hour < record->usages.size(); hour++) {
            monthly_usage[record->month][hour] += record->usages[hour];
        }
    }

    std::map<int, int> max_usage_hour;

    for (auto& pair : monthly_usage) {
        int month = pair.first;
        std::vector<int>& usages = pair.second;

        int max_usage = usages[0];
        int max_hour = 0;

        for (int hour = 1; hour < usages.size(); hour++) {
            if (usages[hour] > max_usage) {
                max_usage = usages[hour];
                max_hour = hour;
            }
        }

        max_usage_hour[month] = max_hour;
    }

    return max_usage_hour;
}