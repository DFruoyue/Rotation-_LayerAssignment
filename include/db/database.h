#pragma once
#include <vector>
#include "Gcell.h"
#include "Guide2D.h"
using namespace std;
namespace XZA{
    struct LayerInfo{   //每一层的布线资源信息
        string name;
        Direction direction;
        int minlength;
        vector<vector<int>> capacity;
        vector<vector<int>> demand;
    };
    struct NetInfo{
        string name;
        vector<Pin> pins;
        Guide2D guide2D;

        NetInfo(const string& name = ""): name(name){}
    };

    class Database{
    private:
        // Routing Resource
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
    public:
        Database(
            const string& capfile = "Data/nvdla.cap",
            const string& netfile = "Data/nvdla.net",
            const string& guide2Dfile = "Data/guide2D.txt"
        );
        void load_data(
            const string& capfile = "Data/nvdla.cap",
            const string& netfile = "Data/nvdla.net",
            const string& guide2Dfile = "Data/guide2D.txt"
        );
    };
    
    extern Database db;
}