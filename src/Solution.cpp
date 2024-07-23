#include "Solution.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include "timer.hpp"
using namespace XZA;
using namespace std;

//每一个net都有一个guide, 一个guide包含了所有的wire和via
void Solution::loadfile(const string& filename){

    Timer timer("读取guide2D.txt");
    const string redundant_chars2 = ":->(),[]";

    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }

    std::string line;
    std::vector<std::istringstream> lines;
    

    while(true){
        string name;
        //读取掉net的名字和读取掉'('
        if(!getline(file, name))
            break;
        getline(file, line);
        auto& guide = guides.emplace_back(name);
        cout << name << endl;

        while(getline(file, line) && line != ")"){ //处理path
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars2](char c) {
                return redundant_chars2.find(c) != std::string::npos;}), line.end());
        lines.emplace_back(istringstream(line));
        }//得到所有的path

        lines.pop_back();   //去掉多余的点
        int startx, starty, endx, endy;

        queue<Clue> q;
        int count = 0;
        int TailWireIdx = 0;
        int HeadWireIdx = 0;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            TailWireIdx = -1;
            HeadWireIdx = -1;
            count ++;  
            //每次处理一条path, 从后往前处理
            auto& ss = *it;

            ss >> endx >> endy >> startx >> starty; //读取掉开头

            ss >> endx >> endy >> startx >> starty; //读取第一段
            guide.wires.emplace_back(-1, startx, starty, -1, endx, endy);
            TailWireIdx = guide.wires.size() - 1;

            while(ss >> endx >> endy >> startx >> starty){//如果有后续Segment
                //处理每一个segment
                Node start(-1, startx, starty);
                Node end(-1, endx, endy);

                guide.wires.emplace_back(start, end);
                int newWireIdx = guide.wires.size() - 1;
                //将新加的segment的end和上一个segment的start连接
                guide.Link(Clue(newWireIdx, END), Clue(newWireIdx-1, START));
            }

            HeadWireIdx = guide.wires.size() - 1;

            //对vector内容取指针后操作需要确保vector不会resize
            Wire* pHeadWire = & guide.wires[HeadWireIdx];
            Wire* pTailWire = & guide.wires[TailWireIdx];

            if(count == 1){
                //处理第一条path,queue初始化
                q.push(Clue(HeadWireIdx, START));
                q.push(Clue(TailWireIdx, END));
            }else{
                while(!q.empty()){
                    Clue clue = q.front();
                    Node& node = guide.getNode(clue);
                    if(node.loc.x != pHeadWire->start.loc.x 
                    || node.loc.y != pHeadWire->start.loc.y){
                        q.pop();
                    }
                }
                if(q.empty()){
                    cerr << "Error: Cannot find the start node of the path" << endl;
                    exit(1);
                }

                //将开头的Node添加Via
                guide.Link(q.front(), Clue(HeadWireIdx, START));

                //将末尾的Node添加至queue
                q.push(Clue(TailWireIdx, END));
            }
        }

        //释放queue
        while(!q.empty())
            q.pop();
    }
    file.close();
    timer.output("读取guide2D.txt");
}

void Guide::targetPin(const Location& loc){
    Wire& wire_Pin = wires.emplace_back(loc, loc);
    Clue pinClue(wires.size() - 1, START);
    int wireNum = wires.size();
    for(int i=0;i < wireNum; i++){

        Wire& wire = wires[i];
        if(wire.start.loc.x == loc.x && wire.start.loc.y == loc.y){
            Link(Clue(i, START), pinClue);
        }
        if(wire.end.loc.x == loc.x && wire.end.loc.y == loc.y){
            Link(Clue(i, END), pinClue);
        }
    }
}

const int Guide::Link(const Clue& nc1, const Clue& nc2){
    int ViaIdx = -1;
    Node& node1 = getNode(nc1);
    Node& node2 = getNode(nc2);
    if(node1.linkVia){
        ViaIdx = node1.ViaIdx;
        node2.setVia(ViaIdx);
        addNodetoVia(ViaIdx, nc2);
    }
    else if(node2.linkVia){
        ViaIdx = node2.ViaIdx;
        node1.setVia(ViaIdx);
        addNodetoVia(ViaIdx, nc1);
    }else{
        vias.emplace_back(node1.loc.x, node1.loc.y);
        ViaIdx = vias.size() - 1;

        node1.setVia(ViaIdx);
        node2.setVia(ViaIdx);

        addNodetoVia(ViaIdx, nc1);
        addNodetoVia(ViaIdx, nc2);
    }
    return ViaIdx;
}

void Guide::output() const{
    cout << "Guide: " << netname << endl;
    cout << "Wires: " << wires.size() << endl;
    const string my_blank = string(7, ' ');
    for(auto& wire: wires){
        cout << my_blank << wire.getEdge() << endl;
    }
    cout << "Vias: " << vias.size() << endl;
    for(auto& via: vias){
        cout << my_blank << via.getEdge() << endl;
    }
}

void Guide::addNodetoVia(const int& ViaIdx, const Clue& NodeClue){
    Via& via = vias[ViaIdx];
    Node& node = this -> getNode(NodeClue);

    via.minLayer = std::min(via.minLayer, node.loc.l);
    via.maxLayer = std::max(via.maxLayer, node.loc.l);

    via.addNode(NodeClue);

    node.setVia(ViaIdx);
}

void Guide::setLayerofWire(const int& WireIdx, const int& l){
    Wire& wire = wires[WireIdx];
    wire.start.loc.l = l;
    wire.end.loc.l = l;

    if(wire.start.linkVia){
        vias[wire.start.ViaIdx].minLayer = std::min(vias[wire.start.ViaIdx].minLayer, l);
        vias[wire.start.ViaIdx].maxLayer = std::max(vias[wire.start.ViaIdx].maxLayer, l);
    }
    if(wire.end.linkVia){
        vias[wire.end.ViaIdx].minLayer = std::min(vias[wire.end.ViaIdx].minLayer, l);
        vias[wire.end.ViaIdx].maxLayer = std::max(vias[wire.end.ViaIdx].maxLayer, l);
    }
}

Node& Guide::getNode(const Clue& NodeClue){
    if(NodeClue.second == START)
        return wires[NodeClue.first].start;
    else
        return wires[NodeClue.first].end;
}