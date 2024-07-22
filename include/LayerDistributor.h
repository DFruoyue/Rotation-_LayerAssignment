#include "database.h"
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace XZA{
    class LayerDistributor{
    private:
        Database& db;
        void init();
        void iterate();
    public:
        LayerDistributor(Database& database): db(database){};
        void run(){
            init();     //pin不设置via,即pin所在的segement其层即为pin所在的层
        };
        void outputdesign(const string& outfilename = "output.txt");
    };

    Direction getDirection(const Location& loc1, const Location& loc2);
    Direction getDirection2D(const TreeNode* node1, const TreeNode* node2);
}