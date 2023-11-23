#include "record.hpp"

Record::Record(int _year, int _month, int _day, int u0, int u1, int u2, int u3, int u4, int u5) : usages(6){
    year = _year;
    month = _month;
    day = _day;
    usages[0] = u0;
    usages[1] = u1;

}