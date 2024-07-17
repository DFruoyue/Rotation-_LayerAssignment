#pragma once
#include <vector>
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL = 1
    };
    struct Location{//Gcell的位置，用整数表示
        //layer
        int l;
        //position
        int x;
        int y;

        Location()
            : l(0), x(0), y(0)
        {}
        Location(int l, int x, int y)
            : l(l), x(x), y(y)
        {}
    };
    using Pin = std::vector<Location>;
}