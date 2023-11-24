#include "../lib/record_serializer.hpp"

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
        records.push_back(new Record(fields[0], fields[1], fields[2], fields[3],
            fields[4], fields[5], fields[6], fields[7], fields[8]));
    }
    return records;
}