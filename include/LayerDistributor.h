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
        double CostofChangeWireToLayer(const int& layer, const int& wireIdx, const int& guideIdx);
        EdgesChanged setLayerofWirewillChangeEdges(const int& guideIdx, const int& WireIdx, const int& layer);
        void setWireToLayer(const int& guideIdx, const int& WireIdx, const int& layer);
        double conjectionCostofEdgesChanged(const EdgesChanged& e) const;

        void Assign();
        void updateDemand(const Wire& prevWire, const Wire& nextWire, const std::shared_ptr<Via> prevVia1 = nullptr, const Via* nextVia1 = nullptr, const std::shared_ptr<Via> prevVia2 = nullptr, const Via* nextVia2 = nullptr);
        void updateDemandofWire(const Wire& prevWire, const Wire& nextWire);
        void updateDemandofVia(const Via& prevVia, const Via& nextVia);
        void AssignNetGreedy(const int& netIdx);
        double CostofchangedWire(const Wire& prevWire, const Wire& nextWire) const;
        double CostofchangedVia(const Via& prevVia, const Via& nextVia) const;

    public:
        LayerDistributor(Database& database, Solution& sl):
            db(database),sl(sl){};
        void run(){
            Timer timer2("层分配");
            Assign();
            timer2.output("层分配");
        };
        void outputdesign(const string& outfilename = "output.txt") const;
        bool CheckViolation() const;
    };
}