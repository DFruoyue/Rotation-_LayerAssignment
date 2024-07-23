#pragma once
#include "Gcell.h"
#include "Solution.h"
#include "Cost.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;
namespace XZA{
    class Conjection{
        private:
            int capacity;
            int demand;
            double state = 0;
        public:
            Conjection(const int& demand, const int& capacity){
                this -> demand = demand;
                this -> capacity = capacity;
                this -> state = calculateCongestionState(demand, capacity);
            }
            void set(const int& demand, const int& capacity){
                this -> demand = demand;
                this -> capacity = capacity;
                if(demand)
                    state = calculateCongestionState(demand, capacity);
            }
            double getState() const {return state;}
            void addDemand(const int& demand){
                this -> demand += demand;
                state = calculateCongestionState(this -> demand, capacity);
            }
            void decreaseDemand(const int& demand){
                this -> demand -= demand;
                state = calculateCongestionState(this -> demand, capacity);
            }
    };
    struct LayerInfo{   //每一层的布线资源信息
        string name;
        Direction direction;
        int minlength;
        vector<vector<Conjection>> conjection;
        int overFlowLayerWeight;
    };

    struct NetInfo{
        const char* BLUE = "\033[34m";
        const char* RESET = "\033[0m";
        string name;
        int pinNum;
        vector<Location> pins;
        vector<PinOptionalLocations> pinsOptionalLocations;

        NetInfo(const string& name = ""): name(name), pinNum(0){}
        void output() const{
            cout << "Net: " << name << " pins: " << endl;
            int i = 1;
            for(const auto& pin : pins){
                cout << i << '.' << "(" << pin.l << ", " << pin.x << ", " << pin.y << ") ";
                cout << endl;
                i++;
            }
        }
    };
}
