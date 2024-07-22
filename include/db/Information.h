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
        Guide2D guide2D;
        Guide2DTree guide2DTree;
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
            cout << "Guide2DTree: " << endl;
            guide2DTree.output();
        }
        void outputfile(const string& filename) const{
            fstream file;
            file.open(filename, ios::app);
                file << "Net: " << name << " pins: " << endl;
            int i = 1;
            for(const auto& pin : pins){
                file << i << '.' << "(" << pin.l << ", " << pin.x << ", " << pin.y << ") ";
                file << endl;
                i++;
            }
            file << "Guide2D: " << endl;
            for(const auto& path : guide2D){
                file << "Path: (" << path.start.x << ", " << path.start.y << ") -> (" << path.end.x << ", " << path.end.y << ")";
                file << "\t Segements: ";
                for(const auto& seg : path.segments){
                    file << "[" << seg.start.x << " " << seg.start.y << " " << seg.end.x << " " << seg.end.y << "]";
                }
                file << endl;
            }
            file.close();
        }
    };
}
