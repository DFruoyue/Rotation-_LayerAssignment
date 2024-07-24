#include "database.h"
#include "LayerDistributor.h"
#include "Solution.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "timer.hpp"
using namespace std;
using namespace XZA;
int main(){
    Database db("Data/nvdla.cap");
    Solution sl("Data/guide2D.txt", "Data/nvdla.net");

    int netNum = sl.getNetNum();

    LayerDistributor ld(db, sl);

    ld.run();

    if(ld.CheckViolation())
        return 0;
    else
        cout << "布线无误\n";

    db.StaticDemand();

    Timer timer1("输出debug结果");
    sl.outputdebug("outputDebug.txt");
    timer1.output("输出debug结果");
    Timer timer2("输出设计结果");
    ld.outputdesign("output.txt");
    timer2.output("输出设计结果");
    return 0;
}