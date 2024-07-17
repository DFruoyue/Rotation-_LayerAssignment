#include "database.h"
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

XZA::Database::Database(){
    load_data();
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
        int dir; file >> dir;
        layers[l].direction = (XZA::Direction)dir;
        file >> layers[l].minlength;
        layers[l].capacity.resize(xSize, vector<int>(ySize));
        for(int x = 0; x < xSize; x++)
            for(int y = 0; y < ySize; y++)
                file >> layers[l].capacity[x][y];
    }

    file.close();
}

void XZA::Database::load_data(){
    load_Routing_Resource_file("Data/nvdla.cap");
}

void XZA::Database::log_load(){
    cout << "Layer Number: " << layerNum << endl;
    cout << "X Size: " << xSize << endl;
    cout << "Y Size: " << ySize << endl;
    cout << "Unit Wirelength Cost: " << unitWLcost << endl;
    cout << "Unit Via Cost: " << unitViacost << endl;
    cout << "Overflow Layer Weight: ";
    for(int i = 0; i < layerNum; i++)
        cout << OverFlowLayerWeight[i] << " ";
    cout << endl;
}

XZA::Database XZA::database;