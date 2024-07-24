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
    for(Guide& guide: sl.guides){
        for(Wire& wire: guide.wires){
            //计算每一条wire添加的cost

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
        sl[i].output(std::cout);
        cin.get();
    }
}
double LayerDistributor::costofaddWireinLayer(const int& layer, const int& wireIdx, Guide& guide){
    double Wirecost = 0;
    double Viacost = 0;

    return Wirecost + Viacost;
}