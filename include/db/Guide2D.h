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
        bool startisPin = false;
        bool endisPin = false;
        int startPinIdx = -1;
        int endPinIdx = -1;
        PathNode start, end;
        std::vector<Segment> segments;
        Path(){}
        bool isPin() const{
            return startisPin || endisPin;
        }
    };
    using Guide2D = std::vector<Path>;
}