#ifndef __BUILDING_HPP__
#define __BUILDING_HPP__

#include <string>
#include <vector>
#include <map>

#include "../lib/record.hpp"
#include "../lib/record_serializer.hpp"
#include "../lib/types.hpp"

class Building {
    public: 
        Building(int _id, std::string _name)
            : id(_id), name(_name) {};
        
        void get_records(char* encoded_records, SourceType source);
        std::map<int, int> calculate_monthly_usage(SourceType source);
        std::map<int, int> calculate_max_usage_hour(SourceType source);
    private:
        std::vector<Record*> select_record_type(SourceType source);
        
        int id;
        std::string name;
        std::vector<Record*> gas_records;
        std::vector<Record*> elec_records;
        std::vector<Record*> water_records;
};

#endif 