#pragma once

#include "database.h"
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "timer.hpp"

namespace XZA{
    class LayerAssignmentWorker{
    private:
        //绑定db和sl
        Database& db;
        Solution& sl;

        //贪心算法进行层分配
        void Assign();
        void AssignNetGreedy(const int& netIdx);
        
        //计算将一个wire布置在layer层带来的拥塞成本和via数量成本
        double CostofChangeWireToLayer(const int& layer, const int& wireIdx, const int& guideIdx);  
        double CostofchangedWire(const Wire& prevWire, const Wire& nextWire) const;
        double CostofchangedVia(const Via& prevVia, const Via& nextVia) const;
        
        //更新Wire变动导致的Gcell demand
        void updateDemand(const Wire& prevWire, const Wire& nextWire, const std::shared_ptr<Via> prevVia1 = nullptr, const Via* nextVia1 = nullptr, const std::shared_ptr<Via> prevVia2 = nullptr, const Via* nextVia2 = nullptr);
        void updateDemandofWire(const Wire& prevWire, const Wire& nextWire);
        void updateDemandofVia(const Via& prevVia, const Via& nextVia);

        //设置一个wire所处的layer, 并且更新db里的demand
        void setWireToLayer(const int& guideIdx, const int& WireIdx, const int& layer);

    public:
        LayerAssignmentWorker(Database& database, Solution& sl):
            db(database),sl(sl){};
        void run(){
            Timer timer2("层分配");
            Assign();               
            timer2.output("层分配");
        };

        //输出结果
        void outputdesign(const string& outfilename = "output.txt") const;

        //检查分配的layer, debug时用
        bool CheckViolation() const;
    };
}