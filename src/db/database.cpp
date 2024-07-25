#include "database.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "timer.hpp"
#include "global.h"
#include <cmath>
using namespace std;

XZA::Database::Database(const string& capfile)
:layerNum(0){
    Timer timer("导入.cap文件");
    load_data(capfile);
    timer.output("导入.cap文件");
}

void XZA::Database::load_data(const string& capfile){
    load_Routing_Resource_file(capfile);
}

void XZA::Database::load_Routing_Resource_file(const string& filename){
    ifstream file(filename);
    string line;
    istringstream ss;
    int lineCount = 0;
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }
    getline(file, line);    lineCount++;
    ss = istringstream(line);
    ss >> layerNum >> xSize >> ySize;

    layers.resize(layerNum);
    getline(file, line);    lineCount++;
    ss = istringstream(line);
    float unitWirecost;
    ss >> unitWirecost >> unitViacost;

    for(int l = 0; l < layerNum; l++){
        int OFW;
        ss >> OFW;
        layers[l].setOverFlowLayerWeight(OFW);
    }

    getline(file, line);    lineCount++;

    getline(file, line);    lineCount++;
    
    int dir;
    string LayerName;
    for(int l = 0; l < layerNum; l++){
        getline(file, line);    lineCount++;
        ss = istringstream(line);
        ss >> LayerName >> dir;
        layers[l].setName(LayerName);
        layers[l].setDirection(static_cast<Direction>(dir));

        layers[l].resize(xSize, ySize);
        int capacity;
        for(int y = 0; y < ySize; y++){
            getline(file, line);    lineCount++;
            ss = istringstream(line);
            for(int x = 0; x < xSize; x++){
                ss >> capacity;
                layers[l].setCapacity(x, y, capacity * UNIT_CAPACITY );
            }
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
            istringstream file(netline);
            std::pair<PinOptionalLocations, int>& OptionalLocations_of_pin = OptionalLocations_of_pins.emplace_back();
            OptionalLocations_of_pin.second = pinIdx;
            Location loc;
            while(file >> loc.l >> loc.x >> loc.y)
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

/*
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
            istringstream file(line);
            PinOptionalLocations& pin = nets.back().pinsOptionalLocations.emplace_back();
            Location loc;
            while(file >> loc.l >> loc.x >> loc.y)
                pin.emplace_back(loc);
        }
    }
    file.close();
}
*/


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
            istringstream file(line);
            Path& path = net.guide2D.emplace_back();
            file >> path.start.x >> path.start.y >> path.end.x >> path.end.y;
            Segment seg;
            while(file >> seg.start.x >> seg.start.y >> seg.end.x >> seg.end.y)
                net.guide2D.back().segments.emplace_back(seg);
        }
    }
    file.close();
}
*/

/*
void XZA::Database::outputdebug() const{
    for(auto& net: nets){
        net.output();
        cin.get();
    }
}
*/

double XZA::Database::changeCostofGcell(const int& l, const int& x, const int& y, int delta_demand){
    if(l == BLANKLAYER)
        return 0;
    return layers[l].CostofChangeDemand(x, y, delta_demand);
}

void XZA::Database::StaticDemand() const{
    const int maxDemand = 3000;
    vector<int> totalcount( 2 * maxDemand);
    int maxConjection = 0;
    Location maxLoc;
    int Max = -maxDemand;
    int Min = maxDemand;
    for(int l = 1; l < layerNum; l++){
        cout << "Layer " << l << ": ";
        vector<int> count( 2 * maxDemand );
        int max = -maxDemand;
        int min = maxDemand;
        double overflowCount = 0;
        for(int x = 0; x < xSize; x++){
            for(int y = 0; y < ySize; y++){
                int DemandMinusCapacity = layers[l].getDemandMinusCapacity(x, y);
                count[DemandMinusCapacity + maxDemand]++;
                totalcount[DemandMinusCapacity + maxDemand]++;
                max = std::max(max, DemandMinusCapacity);
                min = std::min(min, DemandMinusCapacity);
                overflowCount +=  layers[l].getOverflow(x, y);

                if(DemandMinusCapacity > maxConjection){
                    maxConjection = DemandMinusCapacity;
                    maxLoc = Location(l, x, y);
                }
            }
        }
        for(int i = min; i <= max; i++){
            cout << fixed << setprecision(1) << setw(4) << 1.0*i/2 << setw(30) << " (demand - capacity) count: " << right << setw(11) <<  count[i+maxDemand] << endl;
        }
        cout << "Overflow count: " << overflowCount << endl;
        Max = std::max(Max, max);
        Min = std::min(Min, min);
    }
    
    for(int i = Min; i <= Max; i++){
        cout << fixed << setprecision(1) << setw(4) << 1.0*i/2 << setw(30) << " (demand - capacity) count: " << right << setw(11) <<  totalcount[i+maxDemand] << endl;
    }
    cout << "Max conjection: " << maxConjection << " at " << maxLoc << endl;
}

void XZA::Database::increaseDemand(const int& l, const int& x, const int& y, const int& positive_delta){
    if( l == BLANKLAYER)
        return;
    layers[l].increaseDemand(x, y, positive_delta);
}
void XZA::Database::decreaseDemand(const int& l, const int& x, const int& y, const int& positive_delta){
    if( l == BLANKLAYER)
        return;
    layers[l].decreaseDemand(x, y, positive_delta);
}