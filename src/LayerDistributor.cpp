#include "LayerDistributor.h"
#include "Guide2DTree.h"
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
    Timer timer;
    std::ofstream outfile;
    outfile.open(outfilename);
    for(auto& net: db.nets){
        std::queue<TreeNode*> q;
        q.push(net.guide2D.root);
        outfile << net.name << std::endl << "(\n";
        while(!q.empty()){
            TreeNode* node = q.front();
            q.pop();
            for(auto& child: node -> childs){
                outfile << node -> loc.l << ' ' << node -> loc.x << ' ' << node -> loc.y << ' '
                        << child -> loc.l << ' ' << child -> loc.x << ' ' << child -> loc.y
                        <<std::endl;
                q.push(child);
            }
        }
        outfile << ")\n";
    }
    timer.output("写入结果");
}
Direction XZA::getDirection(const Location& loc1, const Location& loc2){
    if(loc1.x != loc2.x){
        return Direction::HORIZONTAL;
    }else if (loc1.y != loc2.y){
        return Direction::VERTICAL;
    }else{
        return Direction::VIA;
    }
}

Direction XZA::getDirection2D(const TreeNode* node1, const TreeNode* node2){
    if(node1 -> loc.x != node2 -> loc.x)
        return Direction::HORIZONTAL;
    else
        return Direction::VERTICAL;
}

void LayerDistributor::init(){
    Timer timer;
    for(auto& net: db.nets){
        std::queue<TreeNode*> q;
        q.push(net.guide2D.root);

        //处理所有的pin,添加Via
        while(!q.empty()){
            TreeNode* node = q.front();
            q.pop();

            //通过via连接pin
            if(node -> isPin){
                int minLayer = db.layerNum;
                int maxLayer = -1;
                for(auto& pinIdx: node -> pinIdx){
                    minLayer = std::min(minLayer, net.pins[pinIdx].l);
                    maxLayer = std::max(maxLayer, net.pins[pinIdx].l);
                }

                node -> loc.l = maxLayer;
                if(minLayer != maxLayer){
                    node -> childs.push_back(new TreeNode(node -> loc.x, node -> loc.y, minLayer));    //添加Via
                }else if(maxLayer == 0){
                    node -> loc.l = randomNum(db.layerNum - 1, 1);
                    node -> childs.push_back(new TreeNode(node -> loc.x, node -> loc.y, 0));    //添加Via
                }
            }

            for(auto& child: node -> childs){
                q.push(child);
            }
        }
    }
    timer.output("初始化pin和Via");
}