#include "LayerDistributor.h"
#include <queue>
#include "timer.hpp"
#include <random>
#include "global.h"
#include "ProcessBar.h"
#include "timer.hpp"
using namespace XZA;

int randomNum(int high, int low = 0){
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 标准 mersenne_twister_engine
    std::uniform_int_distribution<> distrib(low, high); // 定义范围
    return distrib(gen);
}

void LayerDistributor::outputdesign(const string& outfilename) const{
    ofstream file(outfilename);
    for(const Guide& guide: sl.guides){
        file << guide.netname << endl;
        file << "(\n";
        for(const Wire& wire: guide.getWires()){
            file << wire.getEdge() << endl;
        }
        for(const Via& via: guide.getVias()){
            if(via.isValid())
                file << via.getEdge() << endl;
        }
        file << ")\n";
    }
    file.close();
}

double LayerDistributor::CostofchangedVia(const Via& prevVia, const Via& nextVia) const{
    if(!prevVia.isSettled() && !nextVia.isSettled())    //Via未被设置
        return 0;

    double conjectionCost = 0;
    double count = 0;
    if(nextVia.isCounted() && !prevVia.isCounted()){    //新的Via被计数, 旧的Via未被计数
        const int &minLayer = nextVia.getminLayer();
        const int &maxLayer = nextVia.getmaxLayer();
        const Location2D& loc2D = nextVia.getLocation2D();
        for(int l = minLayer + 1; l <= maxLayer - 1; l++)
            conjectionCost += db.changeCostofGcell(l, loc2D.x, loc2D.y, STACKED_VIA_DEMAND);
        count ++;
    }else if(!nextVia.isCounted() && prevVia.isCounted()){  //新的Via未被计数, 旧的Via被计数
        const int &minLayer = prevVia.getminLayer();
        const int &maxLayer = prevVia.getmaxLayer();
        const Location2D& loc2D = prevVia.getLocation2D();
        for(int l = minLayer + 1; l <= maxLayer - 1; l++)
            conjectionCost -= db.changeCostofGcell(l, loc2D.x, loc2D.y, -STACKED_VIA_DEMAND);
        count --;
    }else if(nextVia.isCounted() && prevVia.isCounted()){
        const int& minLayer = std::min(prevVia.getminLayer(), nextVia.getminLayer());
        const int& maxLayer = std::max(prevVia.getmaxLayer(), nextVia.getmaxLayer());
        const Location2D& loc2D = nextVia.getLocation2D();
        for(int l = minLayer + 1; l <= maxLayer - 1; l++){
            if(l < prevVia.getminLayer() + 1 || l > prevVia.getmaxLayer() - 1)    //超出原先Via Stacked的范围,说明增添了Via stacked
                conjectionCost += db.changeCostofGcell(l, loc2D.x, loc2D.y, STACKED_VIA_DEMAND);
            if(l < nextVia.getminLayer() + 1 || l > nextVia.getmaxLayer() - 1)    //超出新的Via Stacked的范围,说明减少了Via stacked
                conjectionCost -= db.changeCostofGcell(l, loc2D.x, loc2D.y, -STACKED_VIA_DEMAND);
        }
    }
    return conjectionCost + count * db.unitViacost;
}

double LayerDistributor::CostofchangedWire(const Wire& prevWire, const Wire& nextWire) const{
    double conjectionCost = 0;
    const int& prevLayer = prevWire.getLayer();
    const int& layer = nextWire.getLayer();
    switch(prevWire.getDirection()){
        case HORIZONTAL:{//x变化
            const int& y = prevWire.getStart().y;
            const int& xstart = prevWire.getStart().x;
            const int& xend = prevWire.getEnd().x;
            const int& xmin = std::min(xstart, xend);
            const int& xmax = std::max(xstart, xend);
            for(int x = xmin;  x<= xmax; x++)
                conjectionCost += db.changeCostofGcell(layer, x, y, WIRE_DEMAND) - db.changeCostofGcell(prevLayer, x, y, - WIRE_DEMAND);
            break;
        }
        case VERTICAL:{
            const int& x = prevWire.getStart().x;
            const int& ystart = prevWire.getStart().y;
            const int& yend = prevWire.getEnd().y;
            const int& ymin = std::min(ystart, yend);
            const int& ymax = std::max(ystart, yend);
            for(int y = ymin; y <= ymax; y++)
                conjectionCost += db.changeCostofGcell(layer, x, y, WIRE_DEMAND) - db.changeCostofGcell(prevLayer, x, y, - WIRE_DEMAND);
            break;
        }
    }
    return conjectionCost;
}

double LayerDistributor::CostofChangeWireToLayer(const int& layer, const int& wireIdx, const int& guideIdx){
    const Wire prevWire = sl[guideIdx].getWire(wireIdx);
    const int& prevLayer = prevWire.getLayer();
    if(layer == prevLayer)
        return 0;
    
    std::unique_ptr<Via> prevVia1 = nullptr;
    std::unique_ptr<Via> prevVia2 = nullptr;
    const Via* nextVia1 = nullptr;
    const Via* nextVia2 = nullptr;

    if(prevWire.isStartLinkVia())
        prevVia1 = std::make_unique<Via>( sl[guideIdx].getVia(prevWire.getStartViaIdx()) );
    if(prevWire.isEndLinkVia())
        prevVia2 = std::make_unique<Via>( sl[guideIdx].getVia(prevWire.getEndViaIdx()) );

    sl.setLayerofWire(guideIdx, wireIdx, layer);                //设置Wire的layer
    const Wire& nextWire = sl[guideIdx].getWire(wireIdx);
    if(prevWire.isStartLinkVia()){nextVia1 = &sl[guideIdx].getVia(prevWire.getStartViaIdx());}
    if(prevWire.isEndLinkVia()){nextVia2 = &sl[guideIdx].getVia(prevWire.getEndViaIdx());}
    sl.setLayerofWire(guideIdx, wireIdx, prevLayer);  //恢复Wire的layer

    double cost = 0;
    //计算Via的cost

    cost += CostofchangedWire(prevWire, nextWire);
    if(prevWire.isStartLinkVia())
        cost += CostofchangedVia(*prevVia1, *nextVia1);
    if(prevWire.isEndLinkVia())
        cost += CostofchangedVia(*prevVia2, *nextVia2);

    return cost;
}

void LayerDistributor::setWireToLayer(const int& guideIdx, const int& WireIdx, const int& layer){
    Guide& guide = sl.guides[guideIdx];
    //复制变化前的Wire
    const Wire prevWire = guide.getWire(WireIdx);
    const int& prevLayer = prevWire.getLayer();

    if(prevLayer == layer)
        return;

    //复制变化前的Via
    std::shared_ptr<Via> prevVia1 = nullptr;
    std::shared_ptr<Via> prevVia2 = nullptr;
    if(prevWire.isStartLinkVia())
        prevVia1 = std::make_shared<Via>( guide.getVia( prevWire.getStartViaIdx() ) );
    if(prevWire.isEndLinkVia())
        prevVia2 = std::make_shared<Via>( guide.getVia( prevWire.getEndViaIdx() ) );

    sl.setLayerofWire(guideIdx, WireIdx, layer);    //设置Wire的layer

    //引用变化后的Wire和Via
    const Wire& nextWire = guide.getWire(WireIdx);
    const Via *nextVia1 = nullptr, *nextVia2 = nullptr;
    if(nextWire.isStartLinkVia())
        nextVia1 = &guide.getVia(nextWire.getStartViaIdx());
    if(nextWire.isEndLinkVia())
        nextVia2 = &guide.getVia(nextWire.getEndViaIdx());

    //更新Demand
    updateDemand(prevWire, nextWire, prevVia1, nextVia1, prevVia2, nextVia2);
}

/*
void LayerDistributor::initConjection(){    //可能并不需要
    for(const Guide& guide: sl.guides){
        for(const Wire& wire: guide.getWires()){
            const int& layer = wire.getLayer();
            switch(wire.getDirection()){
                case VERTICAL:{
                    const int& y = wire.getStart().y;
                    const int& xstart = wire.getStart().x;
                    const int& xend = wire.getEnd().x;
                    const int& xmin = std::min(xstart, xend);
                    const int& xmax = std::max(xstart, xend);
                    for(int x = xmin; x <= xmax; x++){
                        db.increaseDemand(layer,x, y, WIRE_DEMAND);
                    }
                    break;
                }
                case HORIZONTAL:{
                    const int& x = wire.getStart().x;
                    const int& ystart = wire.getStart().y;
                    const int& yend = wire.getEnd().y;
                    const int& ymin = std::min(ystart, yend);
                    const int& ymax = std::max(ystart, yend);
                    for(int y = ymin; y <= ymax; y++){
                        db.increaseDemand(layer, x, y, WIRE_DEMAND);
                    }   
                    break;
                }
                default:
                    cerr << "错误访问:Wire的方向不明" << endl;
                    exit(0);
            }
        }
        for(const Via& via: guide.getVias()){
            if(via.isSettled() && via.isValid()){
                const Location2D& loc = via.getLocation2D();
                const int& lmin = via.getminLayer();
                const int& lmax = via.getmaxLayer();
                for(int l = lmin + 1; l <= lmax - 1; l++)
                    db.layers[l].increaseDemand(loc.x, loc.y, STACKED_VIA_DEMAND);
            }
        }
    }
}
*/

void LayerDistributor::Assign(){
    const int NetNum = sl.getNetNum();
    for(int guideIdx = 0; guideIdx < NetNum; guideIdx++){
        updateProgressBar(1.0 * (guideIdx + 1)/NetNum);
        AssignNetGreedy(guideIdx);
    }
}

bool LayerDistributor::CheckViolation()const{
    Timer timer("检查布线结果");
    for(const Guide& guide: sl.guides){
        for(const Wire& wire: guide.getWires()){
            const int& layer = wire.getLayer();
            if(wire.getDirection() != db.getDirectionofLayer(layer)){
                cerr << "错误:Wire的方向与所在层的方向不一致" << endl;
                return true;
            }else if(layer < 1 || layer >= db.layerNum){
                cerr << "错误:Wire的层号不在范围内" << endl;
                return true;
            }
        }
    }
    timer.output("检查布线结果");
    return false;
}

void LayerDistributor::updateDemand(const Wire& prevWire, const Wire& nextWire, const std::shared_ptr<Via> prevVia1, const Via* nextVia1, const std::shared_ptr<Via> prevVia2, const Via* nextVia2){
    updateDemandofWire(prevWire, nextWire);
    if(prevVia1 != nullptr)
        updateDemandofVia(*prevVia1, *nextVia1);
    if(prevVia2 != nullptr)
        updateDemandofVia(*prevVia2, *nextVia2);
}

void LayerDistributor::updateDemandofWire(const Wire& prevWire, const Wire& nextWire){
    const int& prevLayer = prevWire.getLayer();
    const int& layer = nextWire.getLayer();
    if(prevLayer == layer)
        return;
    switch(prevWire.getDirection()){
        case HORIZONTAL:{
            const int& y = prevWire.getStart().x;
            const int& xstart = prevWire.getStart().x;
            const int& xend = prevWire.getEnd().x;
            const int& xmin = std::min(xstart, xend);
            const int& xmax = std::max(xstart, xend);
            for(int x = xmin; x <= xmax; x++){
                db.increaseDemand(layer, x, y, WIRE_DEMAND);
                db.decreaseDemand(prevLayer, x, y, WIRE_DEMAND);
            }
            break;
        }
        case VERTICAL:{
            const int& x = prevWire.getStart().x;
            const int& ystart = prevWire.getStart().y;
            const int& yend = prevWire.getEnd().y;
            const int& ymin = std::min(ystart, yend);
            const int& ymax = std::max(ystart, yend);
            for(int y = ymin; y <= ymax; y++){
                db.increaseDemand(layer, x, y, WIRE_DEMAND);
                db.decreaseDemand(prevLayer, x, y, WIRE_DEMAND);
            }
            break;
        }
        default:
            cerr << "错误访问:Wire的方向不明" << endl;
            exit(0);
    }
}

void LayerDistributor::updateDemandofVia(const Via& prevVia, const Via& nextVia){
    if(!prevVia.isSettled() && !nextVia.isSettled())    //Via未被设置
        return;

    const Location2D& loc = prevVia.getLocation2D();
    if(loc != nextVia.getLocation2D()){
        cerr << "错误:Via的位置发生了变化" << endl;
        exit(0);
    }

    const int& prevminLayer = prevVia.getminLayer();
    const int& prevmaxLayer = prevVia.getmaxLayer();
    const int& nextminLayer = nextVia.getminLayer();
    const int& nextmaxLayer = nextVia.getmaxLayer();

    const int& lmin = std::min(prevminLayer, nextminLayer);
    const int& lmax = std::max(prevmaxLayer, nextmaxLayer);

    if(!prevVia.isSettled() && nextVia.isSettled()){    //新增整条Via
        for(int l = nextminLayer + 1; l <= nextmaxLayer - 1; l++)
            db.increaseDemand(l, loc.x, loc.y, STACKED_VIA_DEMAND);
        return;
    }else if(prevVia.isSettled() && nextVia.isSettled()){
        for(int l = lmin + 1; l <= lmax - 1; l++){
            if(l < prevminLayer + 1 || l > prevmaxLayer - 1)    //超出原先Via Unstacked的范围,说明增添了Via stacked
                db.increaseDemand(l, loc.x, loc.y, STACKED_VIA_DEMAND);
            if(l < nextminLayer + 1 || l > nextmaxLayer - 1)    //超出新的Via Unstacked的范围,说明减少了Via stacked
                db.decreaseDemand(l, loc.x, loc.y, STACKED_VIA_DEMAND);
        }
        return;
    }else{                                                      //删除整条Via
        for(int l = prevminLayer + 1; l <= prevmaxLayer - 1; l++)
            db.decreaseDemand(l, loc.x, loc.y, STACKED_VIA_DEMAND);
        return;
    }
}

void LayerDistributor::AssignNetGreedy(const int& netIdx){
    Guide& guide = sl.guides[netIdx];
    const int wireNum = guide.getWireNum();

    for(int wireIdx = 0; wireIdx < wireNum; wireIdx++){
        const Wire& wire = guide.getWire(wireIdx);
        const int& prevLayer = wire.getLayer();

        double minCost = INFINITY;
        int targetLayer = BLANKLAYER;

        for(int layer = 1; layer < db.layerNum; layer++){
            if(layer == prevLayer)
                continue;
            if(db.getDirectionofLayer(layer) != wire.getDirection())
                continue;

            double cost = CostofChangeWireToLayer(layer, wireIdx, netIdx);

            if(cost < minCost){
                minCost = cost;
                targetLayer = layer;
            }
        }
            setWireToLayer(netIdx, wireIdx, targetLayer);
    }

}