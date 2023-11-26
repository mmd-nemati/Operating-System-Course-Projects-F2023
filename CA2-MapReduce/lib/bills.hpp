#ifndef __BILLS_HPP__
#define __BILLS_HPP__

#include <string>
#include <vector>

#include "record.hpp"
#include "record_serializer.hpp"
#include "types.hpp"
#include "resource_coefficient.hpp"
#include "record_utils.hpp"

#define CSV_IO_NO_THREAD 
#include "csv.h"

class Bills {
    public: 
        Bills(int _id, char* _path)
            : id(_id), path(_path) {};
        void read_coeffs();
        void save_records(char* encoded_records);
        double calculate_bill(ResourceType source, int month);

    private:
        double calculate_water_bill(int month);
        double calculate_gas_bill(int month);
        double calculate_elec_bill(int month);
        int get_coeff(int month);

        int id;
        std::string path;
        std::vector<ResourceCoefficient*> resources_coeffs;
        std::vector<Record*> records;
};

#endif 