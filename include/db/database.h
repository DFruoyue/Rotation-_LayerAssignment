#pragma once
#include <vector>
#include "Gcell.h"
#include "Guide2D.h"
#include <iostream>
#include <fstream>
using namespace std;
namespace XZA{
    struct LayerInfo{   //每一层的布线资源信息
        string name;
        Direction direction;
        int minlength;
        vector<vector<int>> capacity;
        vector<vector<int>> demand;
    };
    struct NetInfo{
        string name;
        int pinNum;
        vector<Location> pins;
        Guide2D guide2D;

        NetInfo(const string& name = ""): name(name), pinNum(0){}
        void output() const{
            cout << "Net: " << name << " pins: " << endl;
            int i = 1;
            for(const auto& pin : pins){
                cout << i << '.' << "(" << pin.l << ", " << pin.x << ", " << pin.y << ") ";
                cout << endl;
                i++;
            }
            cout << "Guide2D: " << endl;
            for(const auto& path : guide2D){
                cout << "Path: (" << path.start.x << ", " << path.start.y << ") -> (" << path.end.x << ", " << path.end.y << ")";
                cout << "\t Segements: ";
                for(const auto& seg : path.segments){
                    printf("[%d %d %d %d]", seg.start.x, seg.start.y, seg.end.x, seg.end.y);
                }
                cout << endl;
            }
        }
        void outputfile(const string& filename) const{
            fstream file;
            file.open(filename, ios::app);
                file << "Net: " << name << " pins: " << endl;
            int i = 1;
            for(const auto& pin : pins){
                file << i << '.' << "(" << pin.l << ", " << pin.x << ", " << pin.y << ") ";
                file << endl;
                i++;
            }
            file << "Guide2D: " << endl;
            for(const auto& path : guide2D){
                file << "Path: (" << path.start.x << ", " << path.start.y << ") -> (" << path.end.x << ", " << path.end.y << ")";
                file << "\t Segements: ";
                for(const auto& seg : path.segments){
                    file << "[" << seg.start.x << " " << seg.start.y << " " << seg.end.x << " " << seg.end.y << "]";
                }
                file << endl;
            }
            file.close();
        }
    };

    class Database{
    private:
        // Routing Resource
        int netNum;
        int layerNum;
        int xSize;
        int ySize;
        double unitWLcost;
        int unitViacost;
        vector<int> OverFlowLayerWeight;
        vector<int> HorizantalLengths;
        vector<int> VerticalLengths;
        vector<LayerInfo> layers;

        // Net Information
        vector<NetInfo> nets;

        // Load file
        void load_Routing_Resource_file(const string& filename);
        void load_Net_Information_file(const string& filename);
        void load_Guide2D_file(const string& filenam);
        void load_Net_and_guide2D_file(const string& netfilename, const string& guide2Dfilename);
    public:
        Database(
            const string& capfile = "Data/nvdla.cap",
            const string& netfile = "Data/nvdla.net",
            const string& guide2Dfile = "Data/guide2D.txt"
        );
        void load_data(
            const string& capfile,
            const string& netfile,
            const string& guide2Dfile
        );
        void outputdebug() const;
        friend void check_path_node_is_pin(const Database& db);
        friend void check_guide2D(const Database& db);

        friend class LayerDistributor;
    };
    
    extern Database db;
}