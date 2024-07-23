#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL = 1,
        UNDEFINED = 2
    };
    struct Location{//Gcell的位置，用整数表示
        //layer
        int l;
        //position
        int x;
        int y;

        Location()
            : l(-1), x(0), y(0)
        {}
        Location(int l, int x, int y)
            : l(l), x(x), y(y)
        {}

        bool operator==(const Location& other) const{
            return l == other.l && x == other.x && y == other.y;
        }
        bool operator!=(const Location& other) const{
            return !(*this == other);
        }
        friend std::ostream& operator<<(std::ostream& os, const Location& loc){
            return os << std::setw(5) << loc.l << " " << std::setw(5) << loc.x << " " << std::setw(5) <<  loc.y;
        }
    };
    using PinOptionalLocations = std::vector<Location>;
}