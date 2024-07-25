#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL = 1,
        VIA = 2,
        UNDEFINED = 3
    };
    struct Location{//Gcell的位置，用整数表示
        int l;
        int x;
        int y;

        Location(): l(0), x(0), y(0){}
        Location(int&& l, int&& x, int&& y) noexcept: l(l), x(x), y(y){}
        Location(const int& l, const int& x, const int& y): l(l), x(x), y(y){}

        bool operator==(const Location& other) const{return l == other.l && x == other.x && y == other.y;}
        bool operator!=(const Location& other) const{return !(*this == other);}

        friend std::ostream& operator<<(std::ostream& os, const Location& loc){return os << loc.x << " " <<  loc.y << " " << loc.l;}
    };
    struct Location2D{
        int x;
        int y;

        Location2D(): x(0), y(0){}

        Location2D(const Location2D& other): x(other.x), y(other.y){}
        Location2D(Location2D&& other) noexcept: x(std::move(other.x)), y(std::move(other.y)){}
        Location2D(int&& x, int&& y) noexcept: x(std::move(x)), y(std::move(y)){}

        Location2D(const int& x, const int& y): x(x), y(y){}
        Location2D(const Location& loc):x(loc.x), y(loc.y){}

        bool operator==(const Location2D& other) const{return x == other.x && y == other.y;}
        bool operator!=(const Location2D& other) const{return !(*this == other);}

        Location2D& operator=(Location2D&& other) noexcept{
            x = std::move(other.x);
            y = std::move(other.y);
            return *this;
        }
        Location2D& operator=(const Location2D& other){
            x = other.x;
            y = other.y;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const Location2D& loc){return os << loc.x << " " <<  loc.y;}
    };
    using PinOptionalLocations = std::vector<Location>;
}