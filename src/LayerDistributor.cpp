#include "LayerDistributor.h"
#include <queue>
#include "timer.hpp"
#include <random>
#include "global.h"
#include "ProcessBar.h"
using namespace XZA;

int randomNum(int high, int low = 0){
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 标准 mersenne_twister_engine
    std::uniform_int_distribution<> distrib(low, high); // 定义范围
    return distrib(gen);
}

void LayerDistributor::outputdesign(const string& outfilename){
    Timer timer("写入结果");

    timer.output("写入结果");
}

double LayerDistributor::costofChangeWireToLayer(const int& layer, const int& wireIdx, const int& guideIdx){
    const EdgesChanged &es = setLayerofWirewillChangeEdges(guideIdx, wireIdx, layer);
    double conjectionCost = conjectionCostofEdgesChanged(es);
    double ValidViaCost = 0;
    for(const EdgeChanged& e: es){
        if(e.direction == VIA){
            if(e.prevEdge.start.l == e.prevEdge.end.l && e.nextEdge.start.l < e.nextEdge.end.l)
                ValidViaCost += db.unitViacost;
            else if(e.prevEdge.start.l < e.prevEdge.end.l && e.nextEdge.start.l == e.nextEdge.end.l)
                ValidViaCost -= db.unitViacost;
        }
    }
    return conjectionCost + ValidViaCost;
}

EdgesChanged LayerDistributor::setLayerofWirewillChangeEdges(const int& guideIdx, const int& WireIdx, const int& layer){
    EdgesChanged e;
    Wire prevWire = sl[guideIdx].getWire(WireIdx);
    Via prevVia1;
    Via prevVia2;
    if(prevWire.getStart().linkVia){
        prevVia1 = sl[guideIdx].getVia(prevWire.getStart().ViaIdx);
    }
    if(prevWire.getEnd().linkVia){
        prevVia2 = sl[guideIdx].getVia(prevWire.getEnd().ViaIdx);
    }

    sl.setLayerofWire(guideIdx, WireIdx, layer);

    e.emplace_back(prevWire.getEdge(), sl[guideIdx].getWire(WireIdx).getEdge(), prevWire.getDirection());
    if(prevWire.getStart().linkVia){
        const Via& nextVia1 = sl[guideIdx].getVia(prevWire.getStart().ViaIdx);
        if(prevVia1.getminLayer() != nextVia1.getminLayer()||prevVia1.getmaxLayer() != nextVia1.getmaxLayer())
            e.emplace_back(prevVia1.getEdge(), nextVia1.getEdge(), VIA);
    }
    if(prevWire.getEnd().linkVia){
        const Via& nextVia2 = sl[guideIdx].getVia(prevWire.getEnd().ViaIdx);
        if(prevVia2.getminLayer() != nextVia2.getminLayer()||prevVia2.getmaxLayer() != nextVia2.getmaxLayer())
            e.emplace_back(prevVia2.getEdge(), nextVia2.getEdge(), VIA);
    }

    sl.setLayerofWire(guideIdx, WireIdx, prevWire.getLayer());

    return e;
}

void LayerDistributor::setLayerofWire(const int& guideIdx, const int& WireIdx, const int& layer){
    Guide& guide = sl.guides[guideIdx];
    const Wire prevWire = guide.getWire(WireIdx);
    const int& prevLayer = prevWire.getLayer();

    if(prevLayer == layer)
        return;

    Via prevVia1, prevVia2;
    if(prevWire.getStart().linkVia)
        prevVia1 = guide.getVia(prevWire.getStart().ViaIdx);
    if(prevWire.getEnd().linkVia)
        prevVia2 = guide.getVia(prevWire.getEnd().ViaIdx);

    guide.setLayerofWire(WireIdx, layer);

    const Wire& nextWire = guide.getWire(WireIdx);  //更新Wire变化前后的demand情况
    switch(prevWire.getDirection()){
        case VERTICAL:{
            const int& y = prevWire.getStart().loc.y;
            const int xmin = prevWire.getStart().loc.x;
            const int xmax = prevWire.getEnd().loc.x;
            for(int x = xmin; x <= xmax; x++){
                db.layers[layer].conjection[x][y].increaseDemand(WIRE_DEMAND);
                db.layers[prevLayer].conjection[x][y].decreaseDemand(WIRE_DEMAND);
            }
            break;
        }
        case HORIZONTAL:{
            const int& x = prevWire.getStart().loc.x;
            const int ymin = prevWire.getStart().loc.y;
            const int ymax = prevWire.getEnd().loc.y;
            for(int y = ymin; y <= ymax; y++){
                db.layers[layer].conjection[x][y].increaseDemand(WIRE_DEMAND);
                db.layers[prevLayer].conjection[x][y].decreaseDemand(WIRE_DEMAND);
            }
            break;
        }
        default:
            cerr << "错误访问:Wire的方向不明" << endl;
            exit(0);

    }
    if(prevWire.getStart().linkVia){    //更新Via变化前后的demand情况
        Via& nextVia1 = guide.getVia(nextWire.getStart().ViaIdx);
        const int& x = prevVia1.getx();
        const int& y = prevVia1.gety();
        const int& lmin = std::min(prevVia1.getminLayer(), nextVia1.getminLayer());
        const int& lmax = std::max(prevVia1.getmaxLayer(), nextVia1.getmaxLayer());

        for(int l = lmin + 1; l <= lmax - 1; l++){
            if(l < prevVia1.getminLayer() + 1 || l > prevVia1.getmaxLayer() - 1)    //超出原先Via Unstacked的范围,说明增添了Via unstacked
                db.layers[l].conjection[x][y].increaseDemand(UNSTACKED_VIA_DEMAND);
            if(l < nextVia1.getminLayer() + 1 || l > nextVia1.getmaxLayer() - 1)    //超出新的Via Unstacked的范围,说明减少了Via unstacked
                db.layers[l].conjection[x][y].decreaseDemand(UNSTACKED_VIA_DEMAND);
        }
    }
    if(prevWire.getEnd().linkVia){
        Via& nextVia2 = guide.getVia(nextWire.getEnd().ViaIdx);
        const int& x = prevVia2.getx();
        const int& y = prevVia2.gety();
        const int& lmin = std::min(prevVia2.getminLayer(), nextVia2.getminLayer());
        const int& lmax = std::max(prevVia2.getmaxLayer(), nextVia2.getmaxLayer());

        for(int l = lmin + 1; l <= lmax - 1; l++){
            if(l < prevVia2.getminLayer() + 1 || l > prevVia2.getmaxLayer() - 1)    //超出原先Via Unstacked的范围,说明增添了Via unstacked
                db.layers[l].conjection[x][y].increaseDemand(UNSTACKED_VIA_DEMAND);
            if(l < nextVia2.getminLayer() + 1 || l > nextVia2.getmaxLayer() - 1)    //超出新的Via Unstacked的范围,说明减少了Via unstacked
                db.layers[l].conjection[x][y].decreaseDemand(UNSTACKED_VIA_DEMAND);
        }
    }

}

double LayerDistributor::conjectionCostofEdgesChanged(const EdgesChanged& es) const{
    double cost = 0;
    for(const EdgeChanged& e: es){
        switch(e.direction){
            case HORIZONTAL:{//x变化
                const int& prevl = e.prevEdge.start.l;
                const int& nextl = e.nextEdge.start.l;
                const int& y = e.prevEdge.start.y;
                const int& xmin = e.prevEdge.start.x;
                const int& xmax = e.prevEdge.end.x;
                for(int x = xmin;  x<= xmax; x++)
                    cost += db.changeCostofGcell(nextl, x, y, WIRE_DEMAND) - db.changeCostofGcell(prevl, x, y, - WIRE_DEMAND);
                break;
            }
            case VERTICAL:{
                const int& prevl = e.prevEdge.start.l;
                const int& nextl = e.nextEdge.start.l;
                const int& x = e.prevEdge.start.x;
                const int& ymin = e.prevEdge.start.y;
                const int& ymax = e.prevEdge.end.y;
                for(int y = ymin; y <= ymax; y++)
                    cost += db.changeCostofGcell(nextl, x, y, WIRE_DEMAND) - db.changeCostofGcell(prevl, x, y, - WIRE_DEMAND);
                break;
            }
            case VIA:{//如何处理Via的cost
                const int& x = e.prevEdge.start.x;
                const int& y = e.prevEdge.start.y;
                const int& lmin = std::min(e.prevEdge.start.l, e.nextEdge.start.l);
                const int& lmax = std::max(e.prevEdge.start.l, e.nextEdge.start.l);
                for(int l = lmin + 1; l <= lmax - 1; l++){
                    if( l < e.prevEdge.start.l + 1 || l > e.nextEdge.start.l - 1) //超出了原先的Via范围,意味着增加了Via段
                        cost += db.changeCostofGcell(l, x, y, UNSTACKED_VIA_DEMAND);
                    else if( l < e.nextEdge.start.l + 1 || l > e.prevEdge.start.l - 1) //超出了新的Via范围,意味着减少了Via段
                        cost -= db.changeCostofGcell(l, x, y, - UNSTACKED_VIA_DEMAND);
                }
                break;
            }
            default:
                break;
        }
    }
    return cost;
}

void LayerDistributor::initConjection(){

    for(const Guide& guide: sl.guides){
        for(const Wire& wire: guide.wires){
            const int& layer = wire.getLayer();
            switch(wire.getDirection()){
                case VERTICAL:{
                    const int& y = wire.getStart().loc.y;
                    const int& xmin = wire.getStart().loc.x;
                    const int& xmax = wire.getEnd().loc.x;
                    for(int x = xmin; x <= xmax; x++)
                        db.layers[layer].conjection[x][y].increaseDemand(WIRE_DEMAND);
                    break;
                }
                case HORIZONTAL:{
                    const int& x = wire.getStart().loc.x;
                    const int& ymin = wire.getStart().loc.y;
                    const int& ymax = wire.getEnd().loc.y;
                    for(int y = ymin; y <= ymax; y++)
                        db.layers[layer].conjection[x][y].increaseDemand(WIRE_DEMAND);
                    break;
                }
                default:
                    cerr << "错误访问:Wire的方向不明" << endl;
                    exit(0);
            }
        }
        for(const Via& via: guide.vias){
            if(via.valid()){
                const int& x = via.getx();
                const int& y = via.gety();
                const int& lmin = via.getminLayer();
                const int& lmax = via.getmaxLayer();
                for(int l = lmin + 1; l <= lmax - 1; l++)
                    db.layers[l].conjection[x][y].increaseDemand(UNSTACKED_VIA_DEMAND);
            }

        }
    }
    cout << endl;
}

void LayerDistributor::greedyAssgin(){
    const int NetNum = sl.getNetNum();
    for(int guideIdx = 0; guideIdx < sl.guides.size(); guideIdx++){
        updateProgressBar(1.0 * (guideIdx + 1)/NetNum);
        Guide& guide = sl.guides[guideIdx];
        for(int wireIdx = 0; wireIdx < guide.wires.size(); wireIdx++){
            Wire& wire = guide.wires[wireIdx];

            const int& prevLayer = wire.getLayer();
            double minCost = INFINITY;

            int minLayer = -1;

            for(int layer = 1; layer < db.layerNum; layer++){
                if(layer == prevLayer)
                    continue;
                if(db.getDirectionofLayer(layer) != wire.getDirection())
                    continue;

                double cost = costofChangeWireToLayer(layer, wireIdx, guideIdx);
                if(cost < minCost){
                    minCost = cost;
                    minLayer = layer;
                }
            }

            if(minLayer != -1){
                setLayerofWire(guideIdx, wireIdx, minLayer);
            }
        }
    }
}