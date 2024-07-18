#include "database.h"
#include <iostream>
using namespace std;
void printProgressBar(float progress) {
    // ANSI转义序列，设置颜色为绿色
    string green = "\033[32m";
    // 重置颜色
    string reset = "\033[0m";
    string red = "\033[31m";
    int barWidth = 70;
    cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << green << "=" <<reset;
        else if (i == pos) cout << green << ">" << reset;
        else cout << ".";
    }
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush();
}
namespace XZA{
    void check_path_node_is_pin(const Database& db){
        fstream file;
        file.open("output.txt", ios::app);
        bool path_node_not_pin = false;
        int count = 0;
        for(const auto& net :db.nets){          //对于每一个net
            int path_index = 1;
            bool has_not_pin = false;
            for(const auto& path: net.guide2D){ //对于每一个path
                bool path_start_is_pin = false;
                bool path_end_is_pin = false;
                for(const auto&pin :net.pins){
                    if(path_start_is_pin && path_end_is_pin)
                        break;
                    for(const auto&loc : pin){
                        if(path_start_is_pin && path_end_is_pin)
                            break;
                        if(loc.x == path.start.x && loc.y == path.start.y){
                            path_start_is_pin = true;
                        }
                        if(loc.x == path.end.x && loc.y == path.end.y){
                            path_end_is_pin = true;
                        }
                    }
                }
                if(!path_start_is_pin || !path_end_is_pin){
                    file << "注意:net" << net.name <<"里的path["<< path_index <<"]不是pin" << endl;
                    has_not_pin = true;
                }
                path_index ++;
            }
            if(has_not_pin){
                path_node_not_pin = true;
                net.outputfile("output.txt");
            }
            count ++;
            printProgressBar(static_cast<float>(count) / db.netNum);
        }
        // 重置颜色
        string reset = "\033[0m";
        string red = "\033[31m";
        if(!path_node_not_pin)
            cout << red << "\n所有path node都是pin" << reset << endl;
    }

    void check_guide2D(const Database& db){
        int count = 0;
        int empty_pos = 0;
        bool guide2D_empty = false;
        bool has_empty = false;
        for(const auto& net : db.nets){
            if (net.guide2D.empty() && !guide2D_empty){
                empty_pos = count;
                guide2D_empty = true;
                has_empty = true;
            }
            else if (!net.guide2D.empty() && guide2D_empty){
                fstream file;
                file.open("output.txt", ios::app);
                file << "注意: " << empty_pos << "至" << count - 1 << "的guide2D为空";
                file << "net Name:" << db.nets[empty_pos].name << endl;
                file.close();
                guide2D_empty = false;
            }
            count ++;
            printProgressBar(static_cast<float>(count) / db.netNum);
        }
        if (guide2D_empty){
            fstream file;
            file.open("output.txt", ios::app);
            file << "注意: " << empty_pos << "至" << count - 1 << "的guide2D为空";
            file << "net Name:" << db.nets[empty_pos].name << endl;
            file.close();
        }
        if (!has_empty)
            cout << "\n所有net的guide2D都不为空" << endl;
        getchar();
    }
}