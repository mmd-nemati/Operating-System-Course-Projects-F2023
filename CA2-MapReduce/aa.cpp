#include <sstream>
#include <vector>
#include <cstring>
#include <iostream>
#include <map>
class Record {
public: 
    Record() {}
    Record(int _year, int _month, int _day, int u0, int u1, int u2, int u3, int u4, int u5)
        : year(_year), month(_month), day(_day), usages({u0, u1, u2, u3, u4, u5}) {}
    int year;
    int month;
    int day;
    std::vector<int> usages;
};

namespace RecordSerializer {

    std::string serialize(const std::vector<Record*>& records);
    char* encode(const std::vector<Record*>& records);
    std::vector<Record*> decode(const char* encoded_records);

} // namespace RecordSerializer

std::string RecordSerializer::serialize(const std::vector<Record*>& records) {
    std::ostringstream oss;
    for (const auto& record : records) {
        oss << record->year << '-'
            << record->month << '-'
            << record->day << '-';
        for (const auto& usage : record->usages) {
            oss << usage << '-';
        }
        oss << '\n';
    }
    return oss.str();
}

char* RecordSerializer::encode(const std::vector<Record*>& records) {
    std::string serialized_records = RecordSerializer::serialize(records);
    char* encoded_records = new char[serialized_records.size() + 1];
    std::strcpy(encoded_records, serialized_records.c_str());
    return encoded_records;
}

std::vector<Record*> RecordSerializer::decode(const char* encoded_records) {
    std::vector<Record*> records;
    std::istringstream iss(encoded_records);
    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream line_stream(line);
        std::string field;
        std::vector<int> fields;
        while (std::getline(line_stream, field, '-')) {
            fields.push_back(std::stoi(field));
        }
        records.push_back(new Record(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], fields[7], fields[8]));
    }
    return records;
}

int main() {
    // Create some records
    std::vector<Record*> records;
    records.push_back(new Record(2023, 1,  21,   34, 32,   1,   2,  3,  6));
    records.push_back(new Record(2023, 2,  24,   1, 0,   9,   1,  3,  6));
    records.push_back(new Record(2023, 2,  24,   1, 0,   9,   19,  3,  6));
    records.push_back(new Record(2023, 11, 24,   1, 2,   3,   4,  5,  6));
    records.push_back(new Record(2023, 12, 25,   7, 189, 199, 10, 11201, 1002));
    records.push_back(new Record(2023, 12, 25,   7, 300,   199, 10, 11, 12));
    records.push_back(new Record(2023, 12, 25,   1000, 205, 199, 10, 11, 12));

        std::map<int, int> monthly_usage;
    for (Record* record : records) {
        int daily_usage = 0;
        for (int usage : record->usages) {
            daily_usage += usage;
        }

        monthly_usage[record->month] += daily_usage;
    }

    for (auto& pair : monthly_usage) {
        
        std::cout << "month: " << pair.first << " --> " << pair.second << std::endl;
    }
    // std::map<int, std::vector<int>> monthly_usage;

    // // Initialize each month's usage vector with 6 elements
    // for (int month = 1; month <= 12; month++) {
    //     monthly_usage[month] = std::vector<int>(6, 0);
    // }

    // // Add the usage for each hour to the corresponding month and hour
    // for (Record* record : records) {
    //     for (int hour = 0; hour < record->usages.size(); hour++) {
    //         monthly_usage[record->month][hour] += record->usages[hour];
    //     }
    // }

    // std::map<int, int> max_usage_hour;

    // // Find the hour with the maximum usage for each month
    // for (auto& pair : monthly_usage) {
    //     int month = pair.first;
    //     std::vector<int>& usages = pair.second;

    //     int max_usage = usages[0];
    //     int max_hour = 0;

    //     for (int hour = 1; hour < usages.size(); hour++) {
    //         if (usages[hour] > max_usage) {
    //             max_usage = usages[hour];
    //             max_hour = hour;
    //         }
    //     }

    //     max_usage_hour[month] = max_hour;
    // }
    // for (auto& pair : max_usage_hour) {
        
    //     std::cout << "month: " << pair.first << " --> " << pair.second << std::endl;
    // }

    // Encode the records
    // char* encoded_records = RecordSerializer::encode(records);

    // // Print the encoded records
    // std::cout << "Encoded records: " << encoded_records << std::endl;

    // // Decode the records
    // std::vector<Record*> decoded_records = RecordSerializer::decode(encoded_records);

    // // Print the decoded records
    // std::cout << "Decoded records: " << std::endl;
    // for (const auto& record : decoded_records) {
    //     std::cout << record->year << '-'
    //               << record->month << '-'
    //               << record->day << '-';
    //     for (const auto& usage : record->usages) {
    //         std::cout << usage << '-';
    //     }
    //     std::cout << '\n';
    // }

    // // Clean up
    // delete[] encoded_records;
    // for (auto record : records) {
    //     delete record;
    // }
    // for (auto record : decoded_records) {
    //     delete record;
    // }

    return 0;
}
