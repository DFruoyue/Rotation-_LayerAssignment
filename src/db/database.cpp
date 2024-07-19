#include "database.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "timer.hpp"
using namespace std;

XZA::Database XZA::db = XZA::Database("Data/nvdla.cap", "Data/nvdla.net", "Data/guide2D.txt");

XZA::Database::Database(const string& capfile, const string& netfile, const string& guide2Dfile)
:netNum(0),layerNum(0)
{
    Timer timer;
    load_data(capfile, netfile, guide2Dfile);
    timer.output("初始化数据库");
}

void XZA::Database::load_data(const string& capfile, const string& netfile, const string& guide2Dfile){
    load_Routing_Resource_file(capfile);
    load_Net_Information_file(netfile);
    load_Guide2D_file(guide2Dfile);
}

void XZA::Database::load_Routing_Resource_file(const string& filename){
    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }

    file >> layerNum >> xSize >> ySize >> unitWLcost >> unitViacost;

    OverFlowLayerWeight.reserve(layerNum);
    for(int i = 0; i < layerNum; i++)
        file >> OverFlowLayerWeight[i];

    HorizantalLengths.reserve(xSize);
    for(int i = 0; i < xSize; i++)
        file >> HorizantalLengths[i];

    VerticalLengths.reserve(ySize);
    for(int i = 0; i < ySize; i++)
        file >> VerticalLengths[i];

    layers.reserve(layerNum);
    for(int l = 0; l < layerNum; l++){
        file >> layers[l].name;
        int dir; file >> dir; layers[l].direction = (XZA::Direction)dir;
        file >> layers[l].minlength;
        layers[l].capacity.resize(xSize, vector<int>(ySize));
        for(int x = 0; x < xSize; x++)
            for(int y = 0; y < ySize; y++)
                file >> layers[l].capacity[x][y];
    }

    file.close();
}

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
            Pin& pin = nets.back().pins.emplace_back();
            Location loc;
            while(ss >> loc.l >> loc.x >> loc.y)
                pin.emplace_back(loc);
        }
    }
    file.close();
}

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
}

void XZA::Database::outputdebug() const{
    for(auto& net: nets){
        net.output();
        cin.get();
    }
}