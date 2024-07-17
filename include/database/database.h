#pragma once
#include <vector>
using namespace std;
namespace XZA{
    class Database{
    private:
        int layerNum;
        int xSize;
        int ySize;
        double unitWLcost;
        int unitViacost;
        vector<int> OFweight;
    public:
        Database();
        ~Database();
    };
}