#pragma once
#include <vector>
using namespace std;
namespace XZA{
    enum Direction{
        HORIZONTAL = 0,
        VERTICAL
    };
    class LayerInfo{
        public:
        string name;
        Direction direction;
        vector<vector<int>> capacity;
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

        // Load file
        void load_Routing_Resource_file(const string& filename);
        void load_Net_Information_file(const string& filename);

    public:
        Database();
        ~Database();
        void load_data();
    };
}