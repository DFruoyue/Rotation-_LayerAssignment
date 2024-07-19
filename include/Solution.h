#pragma once

#include "Gcell.h"
#include <vector>
namespace XZA{
    enum EdgeType{
        Via,
        Wire
    };
    struct Edge{
        EdgeType type;
        Direction direction;
        int xl, yl, zl, xh, yh, zh;
        Edge(): type(EdgeType::Wire), direction(Direction::HORIZONTAL), xl(0), yl(0), zl(0), xh(0), yh(0), zh(0)
        {}
        Edge(EdgeType type, Direction direction, int xl, int yl, int zl, int xh, int yh, int zh):
            type(type), direction(direction), xl(xl), yl(yl), zl(zl), xh(xh), yh(yh), zh(zh){}
    };
    using Solution = std::vector<Edge>;
}
