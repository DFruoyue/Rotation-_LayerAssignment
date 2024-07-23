#pragma once
#include <vector>
#include "Gcell.h"
#include "Information.h"
#include <iostream>
#include <fstream>
using namespace std;
namespace XZA{
    class Database{
    private:
        // Routing Resource
        int netNum;
        int layerNum;
        int xSize;
        int ySize;
        double unitWLcost;
        int unitViacost;
        vector<int> OverFlowLayerWeight;
        vector<int> HorizantalLengths;
        vector<int> VerticalLengths;
        
        vector<LayerInfo> layers;

        // Net Information
        vector<NetInfo> nets;

        // Load file

        void load_Routing_Resource_file(const string& filename);
        void load_Net_Information_file(const string& filename);
        void load_Guide2D_file(const string& filenam);
        void load_Net_and_guide2D_file(const string& netfilename, const string& guide2Dfilename);
    public:
        Database(
            const string& capfile = "Data/nvdla.cap",
            const string& netfile = "Data/nvdla.net"
        );
        void load_data(
            const string& capfile,
            const string& netfile
        );
        void outputdebug() const;
        
        friend void check_path_node_is_pin(const Database& db);
        friend void check_guide2D(const Database& db);

        friend class LayerDistributor;
        const int getnetNum() const {return netNum;}
    };

}