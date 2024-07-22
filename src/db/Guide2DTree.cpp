#include "Guide2DTree.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <queue>
#include <iostream>
#include <stack>
void XZA::Guide2DTree::load(std::ifstream& file){
    const std::string redundant_chars2 = ":->(),[]";
    root = new TreeNode();
    std::string line;
    std::vector<std::istringstream> lines;
    //读取掉net的名字和读取掉'('
    getline(file, line);
    getline(file, line);

    while(getline(file, line) && line != ")"){ //处理path
        line.erase(remove_if(line.begin(), line.end(), [&redundant_chars2](char c) {
            return redundant_chars2.find(c) != std::string::npos;}), line.end());
    lines.emplace_back(std::istringstream(line));
    }//得到所有的path

    auto &ss = lines.back();lines.pop_back();
    ss >> root-> loc.x >> root-> loc.y;

    //用一个queue来还原Tree
    std::queue<TreeNode*> q;
    q.push(root);

    for (auto it = lines.rbegin(); it != lines.rend(); ++it) {  
        //每次处理一条path, 从后往前处理
        auto& ss = *it;
        TreeNode * start = new TreeNode;
        TreeNode * end = new TreeNode;
        ss >> end->loc.x >> end->loc.y >> start->loc.x >> start->loc.y;
        start -> child.push_back(end);
        end -> parent = start;
        int tempx, tempy;
        ss >> tempx >> tempy;
        TreeNode *mid = end;
        while(ss >> tempx >> tempy >> tempx >> tempy){
            mid -> parent = new TreeNode(tempx, tempy);
            mid -> parent -> child.push_back(mid);
            mid = mid -> parent;
        }
        mid -> parent = start;
        start -> child[0] = mid;

        //将path加入到Tree中
        while(start -> loc != q.front() -> loc)
            q.pop();
        q.front() -> child.push_back(start->child[0]);
        start -> child[0] -> parent = q.front();
        q.push(end);
    }

    //释放queue
    while(!q.empty())
        q.pop();
}
bool XZA::Guide2DTree::targetPin(const int& pinIdx, const XZA::Location& loc){
    bool flag = false;
    std::stack<TreeNode*> s;
    s.push(root);
    while(!s.empty()){
        TreeNode* node = s.top();
        s.pop();
        if(node -> loc.x == loc.x && node -> loc.y == loc.y){
            flag = true;
            node->isPin = true;
            node->pinIdx.push_back(pinIdx);
        }
            
        for(auto& child : node -> child)
            s.push(child);
    }
    return flag;
}
XZA::Guide2DTree::~Guide2DTree(){
    _delete(root);
}
void XZA::Guide2DTree::_delete(XZA::TreeNode* node){
    if(node == nullptr)
        return;
    for(auto& child : node -> child)
        _delete(child);
    delete node;
}
void XZA::Guide2DTree::output() const{
    _output(root, 0);
}
const int internal = 4;
void XZA::Guide2DTree::_output(XZA::TreeNode* node, int depth) const{
    const char* BLUE = "\033[34m";
    const char* RESET = "\033[0m";
    if(node == nullptr)
        return;
    std::cout << std::string(depth * internal, ' ');
    if(node -> isPin)
        std::cout << BLUE;
    std::cout << std::left << std::setw(10) << '(' + std::to_string(node -> loc.x) + ',' + std::to_string(node -> loc.y) + ')' << RESET << std::endl;
    for(auto& child : node -> child)
        _output(child, depth+1);
}