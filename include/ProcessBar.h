#include <iostream>
#include <iomanip>
void updateProgressBar(float percentage) {
    const int barWidth = 60; // 进度条的宽度
    std::cout << "\033[0;32m"; // 设置文本颜色为绿色
    std::cout << "[";
    int pos = barWidth * percentage;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "\033[0m "; // 重置文本颜色
    std::cout << "] " << std::fixed << std::setprecision(1) << percentage * 100 << " % \r";
    std::cout.flush(); // 确保立即输出
}