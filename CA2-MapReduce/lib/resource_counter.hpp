
#ifndef __RESOURCE_COUNTER_HPP__
#define __RESOURCE_COUNTER_HPP__

#include <string>

#include "record.hpp"
#define CSV_IO_NO_THREAD 
#include "csv.h"

class ResourceCounter {
    public:
        ResourceCounter(const char* _path);
        std::vector<Record*> read_records();

    private:
        std::string path;
        std::vector<Record*> records;
};

#endif 