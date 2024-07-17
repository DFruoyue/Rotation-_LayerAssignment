#include "database.h"
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

XZA::Database XZA::db;

XZA::Database::Database(){
    load_data();
}
XZA::Database::Database(const string& capfile, const string& netfile){
    load_data(capfile, netfile);
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

void XZA::Database::load_data(const string& capfile, const string& netfile){
    load_Routing_Resource_file(capfile);
    load_Net_Information_file(netfile);
}

void XZA::Database::load_Net_Information_file(const string& filename){
    ifstream file(filename);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }
    nets.reserve(1000);
    while(true){    //循环读取nets
        char c;
        string name;
        file >> name;
        if(name == "EOF") break;
        cin >> c;
        if(c != '('){
            cerr << "Error: Invalid file format, '(' was lost." << endl;
            exit(1);
        }
        NetInfo net;
        net.name = name;
        cin >> c;
        while(c == '['){    //循环读取每个net中的pins
            //在net.pins中添加一个pin
            Pin &pin = net.pins.emplace_back();
            do{//每个pin有好几个可选点,可选点之间用‘,’分隔
                //在pin中添加loc
                Location &loc = pin.emplace_back();
                scanf("(%d,%d,%d)", &loc.l, &loc.x, &loc.y);
                cin >> c;
            }
            while( c == ',');
            //读取']'后, pin读取完毕
            if(c != ']'){
                cerr << "Error: Invalid file format, ']' was lost." << endl;
                exit(1);
            }
            //读取下一个pin, 以'['开头
            cin >> c;
        }
        nets.push_back(net);
        //读取')'后, net读取完毕
        if( c != ')'){
            cerr << "Error: Invalid file format, ')' was lost." << endl;
            exit(1);
        }
    }
    file.close();
}