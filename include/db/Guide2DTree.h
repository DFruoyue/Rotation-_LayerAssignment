#pragma once
#include <fstream>
#include "Gcell.h"
#include <vector>
namespace XZA{
    struct TreeNode{

        bool isPin = false;
        std::vector<int> pinIdx;

        Location loc;
        std::vector<TreeNode*> child = std::vector<TreeNode*>(0);
        TreeNode* parent = nullptr;

        TreeNode(): loc(0, 0, 0){};
        TreeNode(const int &x, const int &y, const int&l = 0): loc(l, x, y){};
        TreeNode(const Location& loc): loc(loc){};
    };
    class Guide2DTree{
        private:
            void _output(TreeNode* node, int depth) const;
            void _delete(TreeNode* node);
        public:
            TreeNode* root = nullptr;
            Guide2DTree():root(nullptr){}
            ~Guide2DTree();
            void load(std::ifstream& file);
            void output() const;
            bool targetPin(const int& pinIdx, const Location& loc);
    };
}