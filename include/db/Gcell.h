#pragma once
#include <vector>
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL = 1,
        VIA = 2,
        UNDEFINED = 3
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
    };
    using PinOptionalLocations = std::vector<Location>;
}