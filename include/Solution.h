#pragma once

#include "Gcell.h"
#include <vector>
#include <string>
#include <fstream>
namespace XZA{
    enum Forward{
        START,
        END
    };
    using Clue = std::pair<int, Forward>;
    struct Edge{
        Location start, end;
        Edge(const Location& start,const Location& end)
            : start(start), end(end)
        {}
        Edge(int l1, int x1, int y1, int l2, int x2, int y2)
            : start(l1, x1, y1), end(l2, x2, y2)
        {}
        friend std::ostream& operator<<(std::ostream& os, const Edge& edge){
            return os << edge.start << " "<< edge.end;
        }
    };

    class Via;
    class Node{
        public:
            Location loc;
            bool linkVia;
            Node(const Location& loc, const bool& linkVia = false)
                : loc(loc), linkVia(linkVia), ViaIdx(-1)
            {}
            Node(const int& l, const int& x, const int& y, const bool& linkVia = false)
                : loc(l, x, y), linkVia(linkVia), ViaIdx(-1)
            {}
            Node(): loc(-1, 0, 0), linkVia(false), ViaIdx(-1)
            {}

            int ViaIdx;

            void setVia(const int ViaIdx){
                linkVia = true;
                this -> ViaIdx = ViaIdx;
            }
    };

    class Via{
        friend class Guide;

        private:
            int maxLayer, minLayer;
            int x, y;
            std::vector<Clue>   NodeClues;
            void addNode(const Clue& NodeClue){
                NodeClues.push_back(NodeClue);
            }
        public:
            Via()
            {}
            Via(const int&x, const int& y)
                :x(x), y(y)
            {}
            Edge getEdge() const{
                return Edge(minLayer, x, y, maxLayer, x, y);
            }
    };
    
    class Wire{
        public:
            Node start, end;
            Edge getEdge() const{
                return Edge(start.loc, end.loc);
            }
            Wire()
            : start(), end()
            {}
            Wire(const Node& start, const Node& end)
                : start(start), end(end)
            {}
            Wire(const Location& startloc, const Location& endloc)
                : start(startloc, false), end(endloc, false)
            {}
            Wire(const int& lstart, const int& xstart, const int& ystart, const int& lend, const int& xend, const int& yend)
                : start(lstart, xstart, ystart, false), end(lend, xend, yend, false)
            {}
        friend class Guide;
    };


    class Guide{
        friend class Solution;
        private:
            std::vector<Wire> wires;
            std::vector<Via> vias;
            int firstpinIdx = -1;

        public:
            std::string netname;
            Guide(const std::string& name): netname(name){}
            
            
            void output() const;                                        //输出Guide

            Node& getNode(const Clue& NodeClue);                        //获取Node

            bool targetPin(const Location& loc);                        //标记pin的loc是否有效
            void addNodetoVia(const int& ViaIdx, const Clue& NodeClue); //将node添加到Via中
            void setLayerofWire(const int& WireIdx, const int& l);      //设置wire的layer
            const int Link(const Clue& nc1, const Clue& nc2);           //在wire之间创建Via,自动管理
    };
    
    class Solution{
        private:
            std::vector<Guide> guides;
        public:
            void reserve(const int& n){
                guides.reserve(n);
            };
            Guide& operator[](const int& i){
                return guides[i];
            }
            void output() const{
                for(auto& guide: guides)
                    guide.output();
            }
            void loadfile(const std::string& filename);
            Solution(const std::string& filename){
                loadfile(filename);
            }
    };
}