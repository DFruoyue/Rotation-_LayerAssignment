#pragma once

#include "Gcell.h"
#include <vector>
#include <string>
#include <fstream>
namespace XZA{
    enum Forward{
        START,
        END,
        PIN,
        VirtualPIN,
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
            Node(const Node& node): loc(node.loc), linkVia(node.linkVia), ViaIdx(node.ViaIdx){}
            Node(const Location& loc, const bool& linkVia = false): loc(loc), linkVia(linkVia), ViaIdx(-1){}
            Node(const int& l, const int& x, const int& y, const bool& linkVia = false): loc(l, x, y), linkVia(linkVia), ViaIdx(-1){}
            Node(): loc(-1, 0, 0), linkVia(false), ViaIdx(-1){}

            int ViaIdx;

            void setVia(const int ViaIdx){
                linkVia = true;
                this -> ViaIdx = ViaIdx;
            }
    };

    class Via{
        friend class Guide;

        private:
            int maxLayer = -1;
            int minLayer = INFINITY;
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
            const int& getminLayer() const{
                return minLayer;
            }
            const int& getmaxLayer() const{
                return maxLayer;
            }
            const int& getx() const{
                return x;
            }
            const int& gety() const{
                return y;
            }
    };
    
    class Wire{
        friend class Guide;

        private:
            Node start, end;
            Direction direction;

        public:
            Edge getEdge() const{
                return Edge(start.loc, end.loc);
            }
            Wire(const Node& start, const Node& end)
                : start(start), end(end)
            {
                if(start.loc.x != end.loc.x)
                    direction = HORIZONTAL;
                else if(start.loc.y != end.loc.y)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }
            Wire(const Location& startloc, const Location& endloc)
                : start(startloc, false), end(endloc, false)
            {
                if(startloc.x != endloc.x)
                    direction = HORIZONTAL;
                else if(startloc.y != endloc.y)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }
            Wire(const int& lstart, const int& xstart, const int& ystart, const int& lend, const int& xend, const int& yend)
                : start(lstart, xstart, ystart, false), end(lend, xend, yend, false)
            {
                if(xstart != xend)
                    direction = HORIZONTAL;
                else if(ystart != yend)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }
            Direction getDirection() const{return direction;}
            const Node& getStart()const{return start;}
            const Node& getEnd() const{return end;}
    };

    class Pin{
        friend class Guide;
        private:
            Node node;
        public:
            Pin(const Node& node): node(node){}
            Pin(const Location& loc): node(loc, false){}
            Pin(const int& l, const int& x, const int& y): node(l, x, y, false){}

    };


    class Guide{
        friend class Solution;
        friend class LayerDistributor;
        private:
            std::vector<Wire> wires;
            std::vector<Via> vias;
            std::vector<Pin> pins;
            std::vector<Pin> Virtualpins;

        public:
            std::string netname;
            Guide(const std::string& name): netname(name){}
            
            void output(std::ostream& os) const;                                        //输出Guide

            Node& getNode(const Clue& NodeClue);                        //获取Node

            bool targetPin(const Location& loc);                        //标记pin的loc是否有效
            void addNodetoVia(const int& ViaIdx, const Clue& NodeClue); //将node添加到Via中
            void setLayerofWire(const int& WireIdx, const int& l);      //设置wire的layer
            int Link(const Clue& nc1, const Clue& nc2);           //在wire之间创建Via,自动管理
            void addVia(const int& x, const int& y);                    //添加Via
            Wire& getWire(const int& i){
                return wires[i];
            }
            Via& getVia(const int& i){
                return vias[i];
            }
            Direction getDirectionofWire(const int& i){
                return wires[i].direction;
            }
            int getLayerofWire(const int& i){
                return wires[i].start.loc.l;
            }
    };
    
    class Solution{
        friend class LayerDistributor;
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
                    guide.output(std::cout);
            }
            
            Solution(const std::string& guide2Dfilename, const std::string& netfilename){
                loadfile(guide2Dfilename);
                mergefile(netfilename);
            }

            void loadfile(const std::string& filename);
            void mergefile(const std::string& filename);
            void outputdebug(const std::string& filename) const;
    };
}