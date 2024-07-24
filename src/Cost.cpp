#include "Cost.h"
#include <cmath>
double calculateCongestionState(const int& demand, const int& capacity){
    if(capacity != 0){
        return std::exp(0.5 * (demand - capacity) / 2.0);
    }else{
        return std::exp(1.5 * demand / 2.0);
    }
}