#include "../lib/resource_counter.hpp"
#include <iostream>
ResourceCounter::ResourceCounter(const char* _path) {
    path = _path;
}

std::vector<Record*> ResourceCounter::read_records() {
    io::CSVReader<9> in(path);
    in.read_header(io::ignore_extra_column, "Year", "Month", "Day", std::to_string(0), 
        std::to_string(1), std::to_string(2), std::to_string(3), std::to_string(4), std::to_string(5));

    int year, month, day, u0, u1, u2, u3, u4, u5;
    while(in.read_row(year, month, day, u0, u1, u2, u3, u4, u5)) {
        records.push_back(new Record(year, month, day, u0, u1, u2, u3, u4, u5));
    }

    return records;
}