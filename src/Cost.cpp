#include "Cost.h"
#include <cmath>
double calculateCongestionState(const int& demand, const int& capacity){
    return std::exp(static_cast<float>(demand) / capacity);
}