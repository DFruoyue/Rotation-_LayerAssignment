#include "LayerDistributor.h"
using namespace XZA;
void LayerDistributor::init(){
    for(auto& net: db.nets){
        net.solution.reserve(net.guide2D.size());    //起码有这么多个路径
        for(auto& path: net.guide2D){               //处理guide2D中的每个path
            if(path.segments.empty()){              //如果path的segemnts为空,创建via
                if(path.isPin()){                   //如果path是pin,固定其层
                    net.solution.emplace_back(
                        EdgeType::Via,
                        Direction::VIA,
                        path.start.x, path.start.y, net.pins[path.startPinIdx].l,
                        path.start.x, path.start.y, net.pins[path.startPinIdx].l + 1
                    );
                }                    

            }
            else{
                if(path.isPin()){
                    if(path.startisPin){
                        net.solution.emplace_back(
                            EdgeType::Via,
                            Direction::VIA,
                            path.start.x, path.start.y, net.pins[path.startPinIdx].l,
                            path.start.x, path.start.y, net.pins[path.startPinIdx].l + 1
                        );
                    if(path.endisPin){
                        net.solution.emplace_back(
                            EdgeType::Via,
                            Direction::VIA,
                            path.end.x, path.end.y, net.pins[path.endPinIdx].l,
                            path.end.x, path.end.y, net.pins[path.endPinIdx].l + 1
                        );
                    }
                }else{
                    //path如果不是pin,则需要考虑segement
                }
            }
        }
        }
    }
}
void LayerDistributor::iterate(){
    int a = 1;
    //迭代
}