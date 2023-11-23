#ifndef __RESOURCE_COUNTER_HPP__
#define __RESOURCE_COUNTER_HPP__

#include <string>

#include "record.hpp"
#include "csv.h"

class ResourceCounter {
    public:
        ResourceCounter(char* _path);
        std::vector<Record*> read_records();

    private:
        std::string path;
        std::vector<Record*> records;
};

#endif 