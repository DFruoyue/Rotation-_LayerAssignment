#pragma once
#include <vector>
using namespace std;
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL
    };
    struct Location{//Gcell的位置，用整数表示
        //layer
        int l;
        //position
        int x;
        int y;
        
        Location()
            : l(0), x(0), y(0)
        {}
        Location(int l, int x, int y)
            : l(l), x(x), y(y)
        {}
    };
    using Pin = vector<Location>;
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
    public:
        Database();
        Database(const string& capfile, const string& netfile);
        void load_data(const string& capfile = "Data/nvdla.cap", const string& netfile = "Data/nvdla.net");
    };
    extern Database db;
}