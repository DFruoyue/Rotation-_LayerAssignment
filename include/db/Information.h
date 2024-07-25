#pragma once

#include "Gcell.h"
#include "Solution.h"
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
            double overflow = 0;
            float overflowslope = 0;
        public:
            Conjection(){}
            void setCapacity(const int& capacity){
                this -> capacity = capacity;
                if(capacity)
                    overflowslope = 0.5;
                else
                    overflowslope = 1.5;
                overflow = calculateOverflowLoss(0, capacity);
            }
            void increaseDemand(const int& positive_delta){
                this -> demand += positive_delta;
                overflow = calculateOverflowLoss(this -> demand, capacity);
            }
            void decreaseDemand(const int& positive_delta){
                this -> demand -= positive_delta;
                overflow = calculateOverflowLoss(this -> demand, capacity);
            }
            const int& getDemand() const {return demand;}
            const int& getCapacity() const {return capacity;}
            int getDemandMinusCapacity() const {return demand - capacity;}
            const double& getOverflow() const {return overflow;}
            const float& getOverflowSlope() const {return overflowslope;}
    };
    class LayerInfo{   //每一层的布线资源信息
        friend class Database;
        private:
            string name;
            Direction direction = Direction::UNDEFINED;
            vector<vector<Conjection>> conjection;
            int overFlowLayerWeight;

            //对LayerInfo的操作
            void setDirection(const Direction& direction){this -> direction = direction;}
            void setName(const string& name){this -> name = name;}
            void setOverFlowLayerWeight(const int& overFlowLayerWeight){this -> overFlowLayerWeight = overFlowLayerWeight;}
            void resize(const int& xsize, const int& ysize){conjection.resize(xsize, std::vector<Conjection>(ysize));}

            //对conjection的操作
            void setDemand(const int& x, const int& y, const int& demand){conjection[x][y].setCapacity(demand);}
            void setCapacity(const int& x, const int& y, const int& capacity){conjection[x][y].setCapacity(capacity);}
            void increaseDemand(const int& x, const int& y, const int& positive_delta){conjection[x][y].increaseDemand(positive_delta);}
            void decreaseDemand(const int& x, const int& y, const int& positive_delta){conjection[x][y].decreaseDemand(positive_delta);}

        public:
            LayerInfo(){}
            double getOverflowWeight() const {return overFlowLayerWeight;}
            double CostofChangeDemand(const int& x, const int& y, const int& delta_demand) const{
                const Conjection& c = conjection[x][y];
                double newOverflow = calculateOverflowLoss(c.getDemandMinusCapacity() + delta_demand, c.getOverflowSlope());
                return overFlowLayerWeight*(newOverflow - c.getOverflow());
            }

            //获取LayerInfo的信息
            const Direction& getDirection() const{return direction;}
            const string& getName() const{return name;}

            //获取conjection的信息
            const int& getDemand(const int& x, const int& y) const{return conjection[x][y].getDemand();}
            const int& getCapacity(const int& x, const int& y) const{return conjection[x][y].getCapacity();}
            const int getDemandMinusCapacity(const int& x, const int& y) const{return conjection[x][y].getDemandMinusCapacity();}
            const double& getOverflow(const int& x, const int& y) const{return conjection[x][y].getOverflow();}
    };
}