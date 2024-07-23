#include "LayerDistributor.h"
#include <queue>
#include "timer.hpp"
#include <random>
using namespace XZA;

int randomNum(int high, int low = 0){
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 标准 mersenne_twister_engine
    std::uniform_int_distribution<> distrib(low, high); // 定义范围
    return distrib(gen);
}

void LayerDistributor::iterate(){
    //迭代
}

void LayerDistributor::outputdesign(const string& outfilename){
    Timer timer("写入结果");

    timer.output("写入结果");
}


void LayerDistributor::init(){
    Timer timer("初始化LayerDistributor");
    for(auto& guide: sl.guides){
        for(int i = 0; i < guide.getfirstpinIdx(); i++){
            int costminLayer = 0;
            float mincost = INFINITY;

            for(int l = 1; l < db.layerNum; l++){
                if(db.getDirectionofLayer(l) != guide.getDirectionofWire(i))
                    continue;
                //计算cost增量,先不考虑via的事情
                
            }
        }
    }
    timer.output("初始化LayerDistributor");
}

void LayerDistributor::merge(){
    Timer timer("合并guide2D.txt和net信息");
    for(int i=0;i<db.netNum;i++){
        auto& net = db.nets[i];
        auto& guide = sl[i];

        for(auto& pinOptionalLocation : net.pinsOptionalLocations){
            for(auto& loc : pinOptionalLocation){
                if(guide.targetPin(loc)){
                    net.pins.emplace_back(loc);
                    break;
                }
            }
        }
    }
    timer.output("合并guide2D.txt和net信息");
}

void LayerDistributor::outoutdebug() const{
    for(int i=0;i<db.netNum;i++){
        db.nets[i].output();
        sl[i].output();
        cin.get();
    }
}
double LayerDistributor::costofaddWireinLayer(const int& layer, const int& wireIdx, const int& netIdx) const {
    Wire& wire = sl[netIdx].getWire(wireIdx);
    if(wire.direction != db.getDirectionofLayer(layer))
        return INFINITY;
    double Wirecost = 0, Viacost = 0;
    
    const double OverFlowLayerWeight = db.layers[layer].overFlowLayerWeight;
    if(wire.direction == HORIZONTAL){       //y不变,x变化
        double precost = 0;
        double nowcost = 0;
        int y = wire.start.loc.y;
        int xmin = min(wire.start.loc.x, wire.end.loc.x);
        int xmax = max(wire.start.loc.x, wire.end.loc.x);
        for(int x = xmin; x <= xmax; x++){
            precost += db.layers[layer].conjection[x][y].getState() * OverFlowLayerWeight;
            db.layers[layer].conjection[x][y].addDemand(1);
            nowcost += db.layers[layer].conjection[x][y].getState() * OverFlowLayerWeight;
            db.layers[layer].conjection[x][y].decreaseDemand(1);
        }
        Wirecost = nowcost - precost;
    }else{
        double precost = 0;
        double nowcost = 0;
        int x = wire.start.loc.x;           //y变化,x不变
        int ymin = min(wire.start.loc.y, wire.end.loc.y);
        int ymax = max(wire.start.loc.y, wire.end.loc.y);
        for(int y = ymin; y <= ymax; y++){
            precost += db.layers[layer].conjection[x][y].getState() * OverFlowLayerWeight;
            db.layers[layer].conjection[x][y].addDemand(1);
            nowcost += db.layers[layer].conjection[x][y].getState() * OverFlowLayerWeight;
            db.layers[layer].conjection[x][y].decreaseDemand(1);
        }
        Wirecost = nowcost - precost;
    }
    

    double Viacost = 0;

    
    const int formerLayer = sl[netIdx].getLayerofWire(wireIdx);

    if(wire.start.linkVia || wire.end.linkVia){
        int minLayerofstartVia = -1;
        int maxLayerofstartVia = -1;
        int minLayerofendVia = -1;
        int maxLayerofendVia = -1;
        double precostofstartVia = 0;
        double nowcostofstartVia = 0;
        if(wire.start.linkVia){
            const Via& startVia = sl[netIdx].getVia(wire.start.ViaIdx);
            minLayerofstartVia = startVia.getminLayer();
            maxLayerofstartVia = startVia.getmaxLayer();
            for()
            //如何维护demand
        }
        if(wire.end.linkVia){
            const Via& endVia = sl[netIdx].getVia(wire.end.ViaIdx);
            minLayerofendVia = endVia.getminLayer();
            maxLayerofendVia = endVia.getmaxLayer();
        }

        sl[netIdx].setLayerofWire(wireIdx, layer);
        if(wire.start.linkVia){
            
        }
        if(wire.end.linkVia){
            
        }
        sl[netIdx].setLayerofWire(wireIdx, formerLayer);
    }
    return Wirecost + Viacost;
}