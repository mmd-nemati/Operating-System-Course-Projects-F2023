#ifndef __RECORD_HPP__
#define __RECORD_HPP__

#include <string>
#include <vector>

class Record {
    public: 
        Record(int _year, int _month, int _day, int u0, int u1, int u2, int u3, int u4, int u5)
            : year(_year), month(_month), day(_day), usages({u0, u1, u2, u3, u4, u5}) {};
            
        int year;
        int month;
        int day;
        std::vector<int> usages;

};

#endif 