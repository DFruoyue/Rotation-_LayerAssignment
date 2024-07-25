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

        int xSize;
        int ySize;
        int unitViacost;
        int layerNum;
        
        vector<LayerInfo> layers;

        // Load file

        void load_Routing_Resource_file(const string& filename);
//        void load_Net_Information_file(const string& filename);
//        void load_Guide2D_file(const string& filenam);
//        void load_Net_and_guide2D_file(const string& netfilename, const string& guide2Dfilename);
    public:
        Database(const string& capfile = "Data/nvdla.cap");
        void load_data(const string& capfile);
//        void outputdebug() const;

        friend class LayerDistributor;
        Direction getDirectionofLayer(const int& l) const {return layers[l].getDirection();}
        double changeCostofGcell(const int& l, const int& x, const int& y, int delta);
        void StaticDemand() const;
        void increaseDemand(const int& l, const int& x, const int& y, const int& positive_delta);
        void decreaseDemand(const int& l, const int& x, const int& y, const int& positive_delta);
    };

}