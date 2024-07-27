#include "database.h"
#include "LayerAssignmentWorker.h"
#include "Solution.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "timer.hpp"
using namespace std;
using namespace XZA;
int main(){
    Timer timer("开始计时");
    Database db("Data/nvdla.cap");                      // 读取.cap文件
    Solution sl("Data/guide2D.txt", "Data/nvdla.net");  // 读取guide2D.txt和nvdla.net文件

    LayerAssignmentWorker ld(db, sl);

    ld.run();       //进行层分配

    Timer timer2("输出设计结果");
    ld.outputdesign("output.txt");
    timer2.output("输出设计结果");

    timer.output("程序总时间");
    return 0;
}