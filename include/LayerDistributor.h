#include "database.h"
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace XZA{
    class LayerDistributor{
    private:
        Database& db;
        Solution& sl;

        void merge();   //将初始化的solution与net信息合并，得到pin的位置
        void iterate();
    public:
        LayerDistributor(Database& database, Solution& sl):
            db(database),sl(sl){};
        void run(){
            merge();     //pin不设置via,即pin所在的segement其层即为pin所在的层
        };
        void init();
        void outputdesign(const string& outfilename = "output.txt");
    };

    Direction getDirection(const Location& loc1, const Location& loc2);
    Direction getDirection2D(const TreeNode* node1, const TreeNode* node2);
}