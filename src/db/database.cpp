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

//计算改变Gcell需求的cost
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