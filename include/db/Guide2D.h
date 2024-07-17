#pragma once
#include <vector>
namespace XZA{
    struct PathNode{
        int x, y;
        PathNode(int x = 0, int y = 0): x(x), y(y){}
        bool operator==(const PathNode& other) const {
            return x == other.x && y == other.y;
        }  
    };
    struct Segment{
        PathNode start, end;
    };
    struct Path{
        PathNode start, end;
        std::vector<Segment> segments;
    };
    using Guide2D = std::vector<Path>;
}