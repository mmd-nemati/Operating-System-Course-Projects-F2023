#include <sstream>
#include <vector>
#include <cstring>
#include "record.hpp"


namespace RecordSerializer {
    std::string serialize(const std::vector<Record*>& records);
    char* encode(const std::vector<Record*>& records);
    std::vector<Record*> decode(const char* encoded_records);
}