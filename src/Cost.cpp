#include "Cost.h"
#include <cmath>
double calculateOverflowLoss(const int& DemindMinusCapacity, const float& OverflowSlope){
    return std::exp(OverflowSlope * DemindMinusCapacity / 2.0);
}