#ifndef __RESOURCE_COEFFICIENT_HPP__
#define __RESOURCE_COEFFICIENT_HPP__

#include <vector>
#include "types.hpp"
#include <iostream>
class ResourceCoefficient {
    public: 
        ResourceCoefficient(int _year, int _month, int c1, int c2, int c3)
            : year(_year), month(_month), coeffs({c1, c2, c3}) {};
            
        int year;
        int month;
        std::vector<int> coeffs;
};

#endif