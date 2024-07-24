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

        if(lines.size() == 1){      //处理极品点
            int x,y;
            lines[0] >> x >> y;
            guide.addVia(x, y);
        }
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
    if(wireNum == 0){
        Via& via = vias[0];
        if(via.getx() != loc.x || via.gety() != loc.y){
            return false;
        }else{
            Pin& pin = pins.emplace_back(loc);
            Clue pinClue = Clue(pins.size() - 1, PIN);
            addNodetoVia(0, pinClue);
            return true;
        }
    }
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

void Solution::mergefile(const string& filename){
    const string redundant_chars = "(),[]";

    Timer timer("合并guide2D.txt和net信息");
    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }

    string line, name;
    for(Guide& guide: guides){
        if(!getline(file, name))  //第一行输入的是net的名字
            break;
        getline(file, line);      //读取掉'('

        while(getline(file, line) && line != ")"){
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars](char c) {
                return redundant_chars.find(c) != string::npos;
                }), line.end());
            istringstream ss(line);
            Location loc;
            do{
                ss >> loc.l >> loc.x >> loc.y;
            }while(!guide.targetPin(loc));
        }
        if(guide.wires.size() == 0){
            Via& via = guide.vias[0];
            if(via.getminLayer() == via.getmaxLayer()){
                //添加一个虚拟的Pin,使得这个via能够输出
                guide.Virtualpins.emplace_back(via.getmaxLayer() + 1, via.getx(), via.gety());
                Clue pinClue = Clue(guide.Virtualpins.size() - 1, VirtualPIN);
                guide.addNodetoVia(0, pinClue);
            }
        }
    }
    file.close();
    timer.output("合并guide2D.txt和net信息");
}

int Guide::Link(const Clue& nc1, const Clue& nc2){
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

void Guide::output(ostream& os) const{
    os << "Guide: " << netname << endl;
    const string my_blank = string(4, ' ');
    os << pins.size() << " Pins:\n";
    for(const Pin& pin: pins){
        os << my_blank << pin.node.loc << std::endl;
    }
    os << Virtualpins.size() << " Virtual Pins:\n";
    for(const Pin& pin: Virtualpins){
        os << my_blank << pin.node.loc << std::endl;
    }
    os << wires.size() << " Wires:\n";
    for(const Wire& wire: wires){
        os << my_blank << wire.getEdge() << ' ' << wire.direction << endl;
    }
    os << vias.size() << " Vias:\n";
    for(auto& via: vias){
        os << my_blank << via.getEdge() << endl;
    }
}
void Solution::outputdebug(const string& filename) const{
    ofstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }
    for(auto& guide: guides){
        guide.output(file);
    }
    file.close();
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
    if(NodeClue.second == VirtualPIN)
        return Virtualpins[NodeClue.first].node;
    else if(NodeClue.second == PIN)
        return pins[NodeClue.first].node;
    else if(NodeClue.second == START)
        return wires[NodeClue.first].start;
    else
        return wires[NodeClue.first].end;
}

void Guide::addVia(const int& x, const int& y){
    vias.emplace_back(x, y);
}