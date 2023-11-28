#ifndef __BUILDING_HPP__
#define __BUILDING_HPP__

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "../lib/record.hpp"
#include "../lib/record_serializer.hpp"
#include "../lib/types.hpp"
#include "../lib/record_utils.hpp"

class Building {
    public: 
        Building(int _id, std::string _name)
            : id(_id), name(_name) {};
        
        void save_records(const char* encoded_records, ResourceType source);
        std::string get_records(ResourceType source);
        int calculate_monthly_usage(ResourceType source, int month);
        int calculate_max_usage_hour(ResourceType source, int month);
        double calculate_avg_usage(ResourceType source, int month);
        double calculate_diff_max_avg(ResourceType source, int month);
        // send_records(type, month) to bills;
    private:
        // std::vector<Record*> select_record_type(ResourceType source);
        const std::vector<Record*>& select_record_type(ResourceType source);
        
        int id;
        std::string name;
        std::vector<Record*> gas_records;
        std::vector<Record*> elec_records;
        std::vector<Record*> water_records;
};

#endif 