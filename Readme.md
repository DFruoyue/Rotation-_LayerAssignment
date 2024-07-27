# 如何编译和运行

所有任务的详细命令写在`tasks.json`文件里，推荐使用VScode运行。

## 编译

运行 "Build layerAssignment and evaluator" 任务，使用cmake构建。

## 运行

需要三个数据文件：`guide2D.txt`、`nvdla.cap`和`nvdla.net`，放置在`./Data/`目录下 。

先运行"Run layerAssignment"任务，会输出布线结果文件"output.txt"；然后运行"Run evaluator"任务。

# 代码原理

## 总体框架

程序步骤为：

1. 读取`nvdla.cap`，将布线资源数据导入`database`类中
2. 读取`guide2D.txt`和`nvdla.net`，将每一个net的二维布线信息和pin信息导入`solution`类中
3. 布线过程由`LayerAssignmentWorker`类运行`run()`成员函数完成

 布线的算法为贪心算法：按照顺序对每一个net布线，按照guide2D.txt中的顺序访问wire，计算带来开销最小的放置层然后放置。

## 实现细节

### Cost.h/.cpp

定义计算Gcell的overflow的函数`calculateOverflowLoss`。

### Global.h

定义全局常量

### Gcell.h

定义了`Direction`枚举、`Location`结构体（描述3D位置）、`Location2D`结构体（描述2D位置）

### Information.h

定义`Conjection`类，封装了一个Gcell的拥塞情况：demand、capacity、overflowslope和overflow。

定义`LayerInfo`类，封装了一层中每一个Gcell的拥塞情况conjection、direction、name和溢出成本权重overFlowLayerWeight

### database.h/.cpp

定义了`database`类，用于管理布线资源。提供了更改demand的公有方法。

### Soluiton.h/.cpp

定义了`Solution`类，用于管理布线方案。提供了更改wire的公有方法，自动更新与该wire相连的Via的状态。

### LayerAssignmentWorker.h/.cpp

定义了`LayerAssignmentWorker`类，用于分配layer。

### ProcessBar.h和timer.hpp

程序运行中输出进度条和计时的小工具。





