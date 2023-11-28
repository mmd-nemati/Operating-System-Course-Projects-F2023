#ifndef __BILLS_HPP__
#define __BILLS_HPP__

#include <string>
#include <vector>

#include "record.hpp"
#include "record_serializer.hpp"
#include "types.hpp"
#include "resource_coefficient.hpp"
#include "record_utils.hpp"
#include "utils.hpp"

#define CSV_IO_NO_THREAD 
#include "csv.h"

class Bills {
    public: 
        Bills(int _id, std::string _csv_path);
        void read_coeffs();
        void save_records(const char* encoded_records);
        double calculate_bill(ResourceType source, int month);
        RequestBillsData* decode_request(std::string request);

    private:
        double calculate_water_bill(int month);
        double calculate_gas_bill(int month);
        double calculate_elec_bill(int month);
        int get_coeff(int month);

        std::map<std::string, ResourceType> resource_type_map;
        int id;
        std::string csv_path;
        std::vector<ResourceCoefficient*> resources_coeffs;
        std::vector<Record*> records;
};

#endif 