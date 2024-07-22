#pragma once
#include <fstream>
#include "Gcell.h"
#include <vector>
#include <iostream>
namespace XZA{
    struct TreeNode{

        bool isPin = false;
        std::vector<int> pinIdx;

        Location loc;
        std::vector<TreeNode*> childs;
        TreeNode* parent = nullptr;

        TreeNode(): loc(-1, 0, 0){};
        TreeNode(const int &x, const int &y, const int&l = -1): loc(l, x, y){};
        TreeNode(const Location& loc): loc(loc){};
    };
    class Guide2DTree{
        
        private:
            void _output(TreeNode* node, int depth) const;
            void freeTree(TreeNode* node);
            void freeTree_withlog(TreeNode* node, int depth);

        public:
            TreeNode* root = nullptr;
            Guide2DTree()
                :root(nullptr){}
            
            void load(std::ifstream& file);
            void output() const;
            bool targetPin(const int& pinIdx, const Location& loc);
            void outputdesign() const;
    };
}