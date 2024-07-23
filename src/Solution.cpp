#include "Solution.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
using namespace XZA;
using namespace std;

//每一个net都有一个guide, 一个guide包含了所有的wire和via
void Guide::loadfile(ifstream& file){
    string  line;
    vector<string> lines;
    const string redundant_chars2 = ":->(),[]";

    while(true){
        std::string line;
        std::vector<std::istringstream> lines;
        //读取掉net的名字和读取掉'('
        getline(file, line);
        netname = line;
        getline(file, line);

        while(getline(file, line) && line != ")"){ //处理path
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars2](char c) {
                return redundant_chars2.find(c) != std::string::npos;}), line.end());
        lines.emplace_back(istringstream(line));
        }//得到所有的path

        wires.reserve(lines.size());

        auto &ss = lines.back();
        Wire& rootwire = wires.emplace_back();
        ss >> rootwire.start.loc.x >> rootwire.start.loc.y >> rootwire.end.loc.x >> rootwire.end.loc.y;
        lines.pop_back();

        //用一个queue来还原Tree
        std::queue<Node*> q;
        q.push( &wires[0].start );
        int tempx1, tempy1, tempx2, tempy2;
        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {  
            //每次处理一条path, 从后往前处理
            auto& ss = *it;

            ss >> tempx1 >> tempy1 >> tempx2 >> tempy2; //读取掉开头

            //迭代掉开头,确定现在应该添加的node
            while(tempx1 != q.front()->loc.x || tempy1 != q.front()->loc.y)
                q.pop();
            Via via;
            const int NewwireIdx = wires.size();

            while(ss >> tempx1 >> tempy1 >> tempx2 >> tempy2){
                wires.emplace_back(-1, tempx1, tempy1, -1, tempx2, tempy2);
                //从第二个segment被添加起,将新添加的segment与之前的segment连接Via
                if(wires.size() > NewwireIdx + 1){
                    Wire& lastWire = wires.back();
                    Wire& preWire = wires[wires.size() - 2];
                    
                    Link(&lastWire.start, &preWire.end);
                }
            }
            //将开头的node添加Via
            Link(&wires[NewwireIdx].start, q.front());


            //将末尾的Node添加至queue
            q.push(&wires.back().end);

        }

        //释放queue
        while(!q.empty())
            q.pop();
    }
}

void Guide::targetPin(const Location& loc){
    Wire& wire_Pin = wires.emplace_back(loc, loc);
    int wireNum = wires.size();
    for(int i=0;i < wireNum; i++){
        Wire& wire = wires[i];

        if(wire.start.loc.x == loc.x && wire.start.loc.y == loc.y)
            Link(&wire.start, &wire_Pin.start);

        if(wire.end.loc.x == loc.x && wire.end.loc.y == loc.y)
            Link(&wire.end, &wire_Pin.start);

    }
}

const Via* Guide::Link(Node* pnode1, Node* pnode2){
    Via* pVia = nullptr;
    if(pnode1 -> linkVia){
        pnode2 -> linkVia = true;

        pVia = pnode1 -> pVia;

        pVia -> addNode(pnode2);
    }
    else if(pnode2 -> linkVia){
        pnode1 -> linkVia = true;

        pVia = pnode2 -> pVia;

        pVia -> addNode(pnode1);

    }else{
        pnode1 -> linkVia = true;
        pnode2 -> linkVia = true;

        pVia = &vias.emplace_back();

        pVia -> addNode(pnode1);
        pVia -> addNode(pnode2);
    }
    return pVia;
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