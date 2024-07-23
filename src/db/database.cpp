#include "database.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "timer.hpp"
using namespace std;

XZA::Database::Database(const string& capfile, const string& netfile)
:netNum(0),layerNum(0)
{
    Timer timer("初始化数据库");
    load_data(capfile, netfile);
    timer.output("初始化数据库");
}

void XZA::Database::load_data(const string& capfile, const string& netfile){
    load_Routing_Resource_file(capfile);
    load_Net_Information_file(netfile);
}

void XZA::Database::load_Routing_Resource_file(const string& filename){
    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }

    file >> layerNum >> xSize >> ySize >> unitWLcost >> unitViacost;

    layers.resize(layerNum);
    for(int i = 0; i < layerNum; i++)
        file >> layers[i].overFlowLayerWeight;

    HorizantalLengths.reserve(xSize);
    for(int i = 0; i < xSize; i++)
        file >> HorizantalLengths[i];

    VerticalLengths.reserve(ySize);
    for(int i = 0; i < ySize; i++)
        file >> VerticalLengths[i];

    
    for(int l = 0; l < layerNum; l++){
        file >> layers[l].name;
        int dir; file >> dir; layers[l].direction = (XZA::Direction)dir;
        file >> layers[l].minlength;
        layers[l].conjection.resize(xSize, vector<Conjection>(ySize));
        int capacity;
        for(int x = 0; x < xSize; x++)
            for(int y = 0; y < ySize; y++){
                file >> capacity;
                layers[l].conjection[x][y].set(0, capacity);
            }
    }

    file.close();
}

/*
void XZA::Database::load_Net_and_guide2D_file(const string& netfilename, const string& guide2Dfilename){
    ifstream netfile(netfilename);
    ifstream guide2Dfile(guide2Dfilename);

    const string redundant_chars1 = "(),[]";
    if(!netfile.is_open()){
        cerr << "Error: Cannot open file " << netfilename << endl;
        exit(1);
    }
    if(!guide2Dfile.is_open()){
        cerr << "Error: Cannot open file " << guide2Dfilename << endl;
        exit(1);
    }

    string netline, guide2Dline;
    nets.reserve(1000);
    int count = 0;
    while(true){
        string name;
        if(!getline(netfile, name))  //第一行输入的是net的名字
            break;

        auto &net = nets.emplace_back(name);
        netNum ++;

        vector<std::pair<PinOptionalLocations, int>> OptionalLocations_of_pins;      //存储每一个pin可选的位置和其序号
        
        //处理net文件
        int pinIdx = 0;
        getline(netfile, netline);       //读取掉'('
        while(getline(netfile, netline) && netline != ")"){
            netline.erase(remove_if(netline.begin(), netline.end(), [&redundant_chars1](char c) {
                return redundant_chars1.find(c) != string::npos;
                }), netline.end());
            istringstream ss(netline);
            std::pair<PinOptionalLocations, int>& OptionalLocations_of_pin = OptionalLocations_of_pins.emplace_back();
            OptionalLocations_of_pin.second = pinIdx;
            Location loc;
            while(ss >> loc.l >> loc.x >> loc.y)
                OptionalLocations_of_pin.first.emplace_back(loc);
            pinIdx ++;
        }
        net.pinNum = pinIdx;
        net.pins.resize(pinIdx);

        //处理guide2D文件
        net.guide2D.load(guide2Dfile);

        //整合两个文件,固定net中pin的位置
        for(auto& OptionalLocations_of_pin : OptionalLocations_of_pins){
            for(auto& loc : OptionalLocations_of_pin.first){
                if(net.guide2D.targetPin(OptionalLocations_of_pin.second, loc)){
                    net.pins[OptionalLocations_of_pin.second] = loc;
                    break;
                }
            }
        }
    }
    netfile.close();
    guide2Dfile.close();
}
*/

void XZA::Database::load_Net_Information_file(const string& filename){
    ifstream file(filename);
    string redundant_chars = "(),[]";
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }
    string line;
    nets.reserve(1000);
    while(true){
        string name;
        if(!getline(file, name))  //第一行输入的是net的名字
            break;
        nets.emplace_back(name);
        netNum ++;
        getline(file, line);       //读取掉'('
        while(getline(file, line) && line != ")"){
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars](char c) {
                return redundant_chars.find(c) != string::npos;
                }), line.end());
            istringstream ss(line);
            PinOptionalLocations& pin = nets.back().pinsOptionalLocations.emplace_back();
            Location loc;
            while(ss >> loc.l >> loc.x >> loc.y)
                pin.emplace_back(loc);
        }
    }
    file.close();
}

/*
void XZA::Database::load_Guide2D_file(const string& filename){
    ifstream file(filename);
    const string redundant_chars = ":->(),[]";
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }
    string line;
    for(auto &net : nets){
        getline(file, line); //读取掉net的名字
        getline(file, line); //读取掉'('
        while(getline(file, line) && line != ")"){ //处理path
            line.erase(remove_if(line.begin(), line.end(), [&redundant_chars](char c) {
                return redundant_chars.find(c) != string::npos;
                }), line.end());
            istringstream ss(line);
            Path& path = net.guide2D.emplace_back();
            ss >> path.start.x >> path.start.y >> path.end.x >> path.end.y;
            Segment seg;
            while(ss >> seg.start.x >> seg.start.y >> seg.end.x >> seg.end.y)
                net.guide2D.back().segments.emplace_back(seg);
        }
    }
    file.close();
}

*/
void XZA::Database::outputdebug() const{
    for(auto& net: nets){
        net.output();
        cin.get();
    }
}