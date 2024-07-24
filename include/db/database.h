#pragma once
#include <vector>
#include "Gcell.h"
#include "Information.h"
#include "Gcellcost.h"
#include <iostream>
#include <fstream>
using namespace std;
namespace XZA{
    class Database{
    private:
        // Routing Resource
        int layerNum;

        int xSize;
        int ySize;
        double unitWLcost;
        int unitViacost;

        vector<int> HorizantalLengths;
        vector<int> VerticalLengths;
        
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
        Direction getDirectionofLayer(const int& l) const {
            return layers[l].direction;
        }
        double changeCostofGcell(const int& l, const int& x, const int& y, int delta);
    };

}