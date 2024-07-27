#include "Solution.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include "timer.hpp"
#include "global.h"
using namespace XZA;
using namespace std;

//每一个net都有一个guide, 一个guide包含了所有的wire和via
void Solution::loadfile(const string& filename){
    Timer timer("导入guide2D.txt文件");
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
            continue;
        }
        
        lines.pop_back();   //去掉多余的点
        int startx, starty, endx, endy;

        queue<Clue> q;
        int count = 0;
        int TailWireIdx = -1;
        int HeadWireIdx = -1;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            count ++;  
            //每次处理一条path, 从后往前处理
            auto& ss = *it;

            ss >> endx >> endy >> startx >> starty; //读取掉开头

            ss >> endx >> endy >> startx >> starty; //读取第一段
            guide.wires.emplace_back(BLANKLAYER, std::move(startx), std::move(starty), std::move(endx), std::move(endy));
            TailWireIdx = guide.wires.size() - 1;

            while(ss >> endx >> endy >> startx >> starty){//如果有后续Segment
                //处理每一个segment
                Location2D start = Location2D(std::move(startx), std::move(starty));
                Location2D end = Location2D(std::move(endx), std::move(endy));
                guide.wires.emplace_back(std::move(start), std::move(end), BLANKLAYER);
                int newWireIdx = guide.wires.size() - 1;
                //将新加的segment的end和上一个segment的start连接
                guide.Link(Clue(newWireIdx, WIRE_END), Clue(newWireIdx-1, WIRE_START));
            }

            HeadWireIdx = guide.wires.size() - 1;

            //对vector内容取指针后操作需要确保vector不会resize
            Wire& HeadWire = guide.wires[HeadWireIdx];

            if(count == 1){
                //处理第一条path,queue初始化
                q.push(Clue(HeadWireIdx, WIRE_START));
                q.push(Clue(TailWireIdx, WIRE_END));
            }else{
                while(!q.empty()){
                    Clue clue = q.front();
                    const Location2D& loc2D = guide.getLocation2D(clue);
                    if(loc2D != HeadWire.getStart()){
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
                guide.Link(q.front(), Clue(HeadWireIdx, WIRE_START));

                //将末尾的Node添加至queue
                q.push(Clue(TailWireIdx, WIRE_END));
            }
            TailWireIdx = -1;
            HeadWireIdx = -1;
        }

        //释放queue
        while(!q.empty())
            q.pop();
    }
    file.close();
    timer.output("导入guide2D.txt文件");
}

bool Guide::targetPin(const Location& loc){
    int wireNum = wires.size();
    if(wireNum == 0){
        Via& via = vias[0];
        if(via.getLocation2D() != Location2D(loc)){
            return false;
        }else{
            Pin& pin = pins.emplace_back(std::move(loc));
            Clue pinClue = Clue(pins.size() - 1, PIN);
            addNodetoVia(0, pinClue);
            return true;
        }
    }
    bool Pinbuilt = false;
    Clue pinClue;
    for(int i=0;i<wireNum;i++){
        Wire& wire = wires[i];
        if(wire.start  == Location2D(loc)){
            if(!Pinbuilt){
                Pin& pin = pins.emplace_back(std::move(loc));
                pinClue = Clue(pins.size() - 1, PIN);
                Pinbuilt = true;
            }
            Link(Clue(i, WIRE_START), pinClue);
        }
        if(wire.end  == Location2D(loc)){
            if(!Pinbuilt){
                Pin& pin = pins.emplace_back(std::move(loc));
                pinClue = Clue(pins.size() - 1, PIN);
                Pinbuilt = true;
            }
            Link(Clue(i, WIRE_END), pinClue);
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
        if(guide.wires.size() == 0 && !guide.vias[0].isValid()){
            //添加一个虚拟的Pin,使得这个via能够输出
            guide.Virtualpins.emplace_back(guide.vias[0].getmaxLayer() + 1, guide.vias[0].getLocation2D());
            Clue pinClue = Clue(guide.Virtualpins.size() - 1, VirtualPIN);
            guide.addNodetoVia(0, pinClue);
        }

    }
    file.close();
    timer.output("合并guide2D.txt和net信息");
}

const int& Guide::Link(const Clue& nc1, const Clue& nc2){
    bool *plinkVia1 = nullptr, *plinkVia2 = nullptr;
    int *pViaIdx1 = nullptr, *pViaIdx2 = nullptr;
    switch(nc1.second){
        case WIRE_START:
            plinkVia1 = &wires[nc1.first].startLinkVia;
            pViaIdx1 = &wires[nc1.first].startViaIdx;
            break;
        case WIRE_END:
            plinkVia1 = &wires[nc1.first].endLinkVia;
            pViaIdx1 = &wires[nc1.first].endViaIdx;
            break;
        case PIN:
            plinkVia1 = &pins[nc1.first].linkVia;
            pViaIdx1 = &pins[nc1.first].ViaIdx;
            break;
        case VirtualPIN:
            plinkVia1 = &Virtualpins[nc1.first].linkVia;
            pViaIdx1 = &Virtualpins[nc1.first].ViaIdx;
            break;
    }
    switch(nc2.second){
        case WIRE_START:
            plinkVia2 = &wires[nc2.first].startLinkVia;
            pViaIdx2 = &wires[nc2.first].startViaIdx;
            break;
        case WIRE_END:
            plinkVia2 = &wires[nc2.first].endLinkVia;
            pViaIdx2 = &wires[nc2.first].endViaIdx;
            break;
        case PIN:
            plinkVia2 = &pins[nc2.first].linkVia;
            pViaIdx2 = &pins[nc2.first].ViaIdx;
            break;
        case VirtualPIN:
            plinkVia2 = &Virtualpins[nc2.first].linkVia;
            pViaIdx2 = &Virtualpins[nc2.first].ViaIdx;
            break;
    }

    if( *plinkVia1 && *plinkVia2){
        return *pViaIdx1;
    }

    const Location2D& loc = getLocation2D(nc1);

    if(*plinkVia1){
        *pViaIdx2 = *pViaIdx1;
        addNodetoVia(*pViaIdx1, nc2);
    }
    else if(*plinkVia2){
        *pViaIdx1 = *pViaIdx2;
        addNodetoVia(*pViaIdx1, nc1);

    }else{  //新建一个Via
        vias.emplace_back(loc);
        addNodetoVia(vias.size()-1, nc1);
        addNodetoVia(vias.size()-1, nc2);
    }
    return *pViaIdx1;
}

void Guide::output(ostream& os) const{
    os << "Guide: " << netname << endl;
    const string my_blank = string(4, ' ');
    os << pins.size() << " Pins:\n";
    for(const Pin& pin: pins){
        os << my_blank << pin << std::endl;
    }
    os << Virtualpins.size() << " Virtual Pins:\n";
    for(const Pin& pin: Virtualpins){
        os << my_blank << pin << std::endl;
    }
    os << wires.size() << " Wires:\n";
    for(const Wire& wire: wires){
        os << my_blank << wire.getEdge() << ' ' << wire.direction << endl;
    }

    os << ViaCount << " Valid Vias:\n";
    for(auto& via: vias){
        if(via.isValid())
            os << my_blank << via.getEdge() << endl;
    }

    os << " Invalid Vias:\n";
    for(auto& via: vias){
        if(!via.isValid())
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
    if(ViaIdx >= vias.size() || ViaIdx < 0){
        cerr << "Error: ViaIdx out of range: ViaIdx = " << ViaIdx << ", size of vias = " << vias.size() << endl;
        exit(1);
    }

    Via& via = vias[ViaIdx];

    switch(NodeClue.second){
        case VirtualPIN:
            Virtualpins[NodeClue.first].ViaIdx = ViaIdx;
            Virtualpins[NodeClue.first].linkVia = true;
            break;
        case PIN:
            pins[NodeClue.first].ViaIdx = ViaIdx;
            pins[NodeClue.first].linkVia = true;
            break;
        case WIRE_START:
            wires[NodeClue.first].startViaIdx = ViaIdx;
            wires[NodeClue.first].startLinkVia = true;
            break;
        case WIRE_END:
            wires[NodeClue.first].endViaIdx = ViaIdx;
            wires[NodeClue.first].endLinkVia = true;
            break;
        default:
            cerr << "Error: NodeClue type error" << endl;
            exit(1);
    }

    via.addNode(NodeClue);

    //对Via数据更新
    updateVia(ViaIdx);
}

const Location2D& Guide::getLocation2D(const Clue& NodeClue) const{
   try{
        switch(NodeClue.second){
            case VirtualPIN:
                return Virtualpins.at(NodeClue.first).loc2D;
            case PIN:
                return pins.at(NodeClue.first).loc2D;
            case WIRE_START:
                return wires.at(NodeClue.first).start;
            case WIRE_END:
                return wires.at(NodeClue.first).end;
            default:
                cerr << "Error: NodeClue type error" << endl;
                exit(1);
        }
    }
    catch(const std::out_of_range& e){
        cerr << "Error: Out of range! " << e.what() << endl;
        exit(1);
    }
}

const int& Guide::getNodeLayer(const Clue& NodeClue)const {
    try{
        switch(NodeClue.second){
            case VirtualPIN:
                return Virtualpins.at(NodeClue.first).layer;
            case PIN:
                return pins.at(NodeClue.first).layer;
            case WIRE_START:
            case WIRE_END:
                return wires.at(NodeClue.first).layer;
            default:
                cerr << "Error: NodeClue type error" << endl;
                exit(1);
        }
    }
    catch(const std::out_of_range& e){
        cerr << "Error: Out of range! " << e.what() << endl;
        exit(1);
    }
}

void Guide::addVia(const int& x, const int& y){vias.emplace_back(x, y);}

void Guide::setLayerofWire(const int& WireIdx, const int& l){
    Wire& wire = wires[WireIdx];
    wire.setLayer(l);

    //更新Via
    if(wire.startLinkVia)
        updateVia(wire.startViaIdx);
    if(wire.endLinkVia)
        updateVia(wire.endViaIdx);
}
void Solution::setLayerofWire(const int& guideIdx, const int& WireIdx, const int& layer){
    //获取原先的区域
    guides[guideIdx].setLayerofWire(WireIdx, layer);
}

void Guide::updateVia(const int& ViaIdx){
    bool preViaCounted = vias[ViaIdx].isCounted();
    Via &via = vias[ViaIdx];
    via.minLayer = INFINITY;
    via.maxLayer = 0;
    bool allset = true;
    //遍历绑定此Via的所有node, 更新Via跨越的layer
    for(const Clue& nodeClue: via.NodeClues){
        int NodeLayer = getNodeLayer(nodeClue);
        via.minLayer = std::min(via.minLayer, NodeLayer);
        via.maxLayer = std::max(via.maxLayer, NodeLayer);
        //如果有一个node的layer是BLANKLAYER, 那么这个Via就是尚未设置的
        if(NodeLayer == BLANKLAYER)
            allset = false;
    }

    via.settled = std::move(allset);

    //更新ValidViaCount, 如果Via的最大层和最小层相同, 那么这个Via是无效的
    via.valid = via.minLayer < via.maxLayer;

    if(!preViaCounted && via.isCounted())
        ViaCount ++;
    else if(preViaCounted && !via.isCounted())
        ViaCount --;
}

const Guide& Solution::getGuide(const int& guideIdx) const{
    return guides[guideIdx];
}