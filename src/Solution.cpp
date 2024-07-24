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
    
    string name;
    while(true){
        std::vector<std::istringstream> lines;
        //读取掉net的名字和读取掉'('
        if(!getline(file, name))
            break;
        getline(file, line);
        auto& guide = guides.emplace_back(name);

        while(getline(file, line) && line != ")"){ //处理path
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars2](char c) {
                return redundant_chars2.find(c) != std::string::npos;}), line.end());
        lines.emplace_back(istringstream(line));
        }//得到所有的path

        lines.pop_back();   //去掉多余的点
        int startx, starty, endx, endy;

        queue<Clue> q;
        int count = 0;
        int TailWireIdx = -1;
        int HeadWireIdx = -1;
        Wire* pHeadWire = nullptr;
        Wire* pTailWire = nullptr;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
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
            Wire& HeadWire = guide.wires[HeadWireIdx];
            pTailWire = & guide.wires[TailWireIdx];

            if(count == 1){
                //处理第一条path,queue初始化
                q.push(Clue(HeadWireIdx, START));
                q.push(Clue(TailWireIdx, END));
            }else{
                while(!q.empty()){
                    Clue clue = q.front();
                    Node& node = guide.getNode(clue);
                    if(node.loc.x != HeadWire.getStart().loc.x 
                    || node.loc.y != HeadWire.getStart().loc.y){
                        q.pop();
                    }else{
                        break;
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
            TailWireIdx = -1;
            HeadWireIdx = -1;
            pHeadWire = nullptr;
            pTailWire = nullptr;
        }

        //释放queue
        while(!q.empty())
            q.pop();
    }
    file.close();
    timer.output("读取guide2D.txt");
}

bool Guide::targetPin(const Location& loc){
    int wireNum = wires.size();
    bool Pinbuilt = false;
    Clue pinClue;
    for(int i=0;i<wireNum;i++){
        Wire& wire = wires[i];
        if(wire.start.loc.x == loc.x && wire.start.loc.y == loc.y){
            if(!Pinbuilt){
                Pin& pin = pins.emplace_back(loc);
                pinClue = Clue(pins.size() - 1, PIN);
                Pinbuilt = true;
            }
            Link(Clue(i, START), pinClue);
        }
        if(wire.end.loc.x == loc.x && wire.end.loc.y == loc.y){
            if(!Pinbuilt){
                Pin& pin = pins.emplace_back(loc);
                pinClue = Clue(pins.size() - 1, PIN);
                Pinbuilt = true;
            }
            Link(Clue(i, END), pinClue);
        }
    }
    return Pinbuilt;
}

const int Guide::Link(const Clue& nc1, const Clue& nc2){
    int ViaIdx = -1;
    Node& node1 = getNode(nc1);
    Node& node2 = getNode(nc2);

    if(node1.linkVia && node2.linkVia){
        return node1.ViaIdx;
    }

    if(node1.linkVia){
        ViaIdx = node1.ViaIdx;
        addNodetoVia(ViaIdx, nc2);

    }
    else if(node2.linkVia){
        ViaIdx = node2.ViaIdx;
        addNodetoVia(ViaIdx, nc1);

    }else{
        vias.emplace_back(node1.loc.x, node1.loc.y);
        ViaIdx = vias.size() - 1;

        addNodetoVia(ViaIdx, nc1);
        addNodetoVia(ViaIdx, nc2);
    }
    return ViaIdx;
}

void Guide::output() const{
    cout << "Guide: " << netname << endl;
    const string my_blank = string(4, ' ');
    cout << pins.size() << " Pins:\n";
    for(const Pin& pin: pins){
        cout << my_blank << pin.node.loc << std::endl;
    }
    cout << wires.size() << " Wires:\n";
    for(const Wire& wire: wires){
        cout << my_blank << wire.getEdge() << ' ' << wire.direction << endl;
    }
    cout << vias.size() << " Vias:\n";
    for(auto& via: vias){
        cout << my_blank << via.getEdge() << endl;
    }
}

void Guide::addNodetoVia(const int& ViaIdx, const Clue& NodeClue){
    if(ViaIdx >= vias.size()){
        cerr << "Error: ViaIdx out of range" << endl;
        exit(1);
    }

    Via& via = vias[ViaIdx];
    Node& node = getNode(NodeClue);

    //对Via数据更新
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
    if(NodeClue.second == PIN)
        return pins[NodeClue.first].node;
    else if(NodeClue.second == START)
        return wires[NodeClue.first].start;
    else
        return wires[NodeClue.first].end;
}