#pragma once

#include "Gcell.h"
#include "Solution.h"
#include "Gcellcost.h"
#include "Cost.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


using namespace std;
namespace XZA{
    class Conjection{
        private:
            int capacity = 0;
            int demand = 0;
            double state = 0;
            GcellCost changeInfluence;
            void updateChangeInfluence(const double& nowState){
                changeInfluence.increase1Cost = calculateCongestionState(demand + 1, capacity) - nowState;
                changeInfluence.increase2Cost = calculateCongestionState(demand + 2, capacity) - nowState;
                changeInfluence.decrease1Cost = nowState - calculateCongestionState(demand - 1, capacity);
                changeInfluence.decrease2Cost = nowState - calculateCongestionState(demand - 2, capacity);
            }
        public:
            Conjection(){}
            void setCapacity(const int& capacity){
                this -> capacity = capacity;
                state = calculateCongestionState(0, capacity);
                updateChangeInfluence(state);
            }

            double getState() const {return state;}

            void increaseDemand(const int& demand){
                this -> demand += demand;
                state = calculateCongestionState(this -> demand, capacity);
                updateChangeInfluence(state);
            }
            void decreaseDemand(const int& demand){
                this -> demand -= demand;
                state = calculateCongestionState(this -> demand, capacity);
                updateChangeInfluence(state);
            }
            double getChangeInfluence(const int& delta) const{
                switch(delta){
                    case 1:
                        return changeInfluence.increase1Cost;
                    case -1:
                        return changeInfluence.decrease1Cost;
                    case 2:
                        return changeInfluence.increase2Cost;
                    case -2:
                        return changeInfluence.decrease2Cost;
                    default:
                        cerr << "错误访问:delat = " << delta << endl;
                        exit(0);
                }
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