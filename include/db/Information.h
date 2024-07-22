#pragma once
#include "Gcell.h"
#include "Guide2D.h"
#include "Solution.h"
#include "Guide2DTree.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
namespace XZA{
    struct LayerInfo{   //每一层的布线资源信息
        string name;
        Direction direction;
        int minlength;
        vector<vector<int>> capacity;
        vector<vector<int>> demand;
    };

    struct NetInfo{
        const char* BLUE = "\033[34m";
        const char* RESET = "\033[0m";
        string name;
        int pinNum;
        vector<Location> pins;
        Guide2DTree guide2D;
        Solution solution;

        NetInfo(const string& name = ""): name(name), pinNum(0){}
        void output() const{
            cout << "Net: " << name << " pins: " << endl;
            int i = 1;
            for(const auto& pin : pins){
                cout << i << '.' << "(" << pin.l << ", " << pin.x << ", " << pin.y << ") ";
                cout << endl;
                i++;
            }
            cout << "Guide2D: " << endl;
            guide2D.output();
        }
    };
}
