#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
namespace XZA{
    class Timer{
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            std::chrono::duration<double> duration;
        public:
            Timer(const std::string& task = ""){
                start = std::chrono::high_resolution_clock::now();
                std::cout << task << "*..." << std::endl;
            }
            void output(const std::string& task, const int& precision = 3){
                duration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start);
                std::cout << "   >"<< task << "用时: " << std::fixed << std::setprecision(precision) << duration.count() << " 秒." << std::endl;
            }
    };
}
