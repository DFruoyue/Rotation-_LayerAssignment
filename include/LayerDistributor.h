#pragma once

#include "database.h"
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "timer.hpp"

namespace XZA{
    struct EdgeChanged{
        Edge prevEdge;
        Edge nextEdge;
        Direction direction;
        EdgeChanged(const Edge& prevEdge, const Edge& nextEdge, const Direction& direction):
            prevEdge(prevEdge), nextEdge(nextEdge), direction(direction){};
    };
    using EdgesChanged = std::vector<EdgeChanged>;
    class LayerDistributor{
    private:
        Database& db;
        Solution& sl;
        double costofChangeWireToLayer(const int& layer, const int& wireIdx, const int& guideIdx);
        EdgesChanged setLayerofWirewillChangeEdges(const int& guideIdx, const int& WireIdx, const int& layer);
        void setLayerofWire(const int& guideIdx, const int& WireIdx, const int& layer);
        double conjectionCostofEdgesChanged(const EdgesChanged& e) const;

        void initConjection();
        void greedyAssign();

    public:
        LayerDistributor(Database& database, Solution& sl):
            db(database),sl(sl){};
        void run(){
            Timer timer1("初始化conjection");
            initConjection();
            timer1.output("初始化conjection");
            Timer timer2("层分配");
            greedyAssign();
            timer2.output("层分配");
        };
        void outputdesign(const string& outfilename = "output.txt") const;
        bool CheckViolation() const;
    };
}