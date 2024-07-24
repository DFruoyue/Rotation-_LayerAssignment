#pragma once

#include "database.h"
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <vector>

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

        void merge();   //将初始化的solution与net信息合并，得到pin的位置
        void iterate();
        void initConjection();
    public:
        LayerDistributor(Database& database, Solution& sl):
            db(database),sl(sl){};
        void run(){
        };
        void init();
        void outputdesign(const string& outfilename = "output.txt");
        double costofaddWireinLayer(const int& layer, const int& WireIdx, Guide& guide);
        EdgesChanged setLayerofWirewillChangeEdges(const int& guideIdx, const int& WireIdx, const int& layer);
        void setLayerofWire(const int& guideIdx, const int& WireIdx, const int& layer);
        double costofEdgesChanged(const EdgesChanged& e) const;
    };
}