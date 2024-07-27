#pragma once

#include "Gcell.h"
#include <vector>
#include <string>
#include <fstream>
#include "global.h"
namespace XZA{
    enum NodeType{
        WIRE_START,
        WIRE_END,
        PIN,
        VirtualPIN,
    };
    using Clue = std::pair<int, NodeType>;
    struct Edge{
        Location start, end;
        Edge(const Location& start,const Location& end): start(start), end(end){}
        Edge(const Location2D& start, const Location2D& end, const int& l): start(l, start.x, start.y), end(l, end.x, end.y){}
        Edge(const int& l1, const int& x1, const int& y1, const int& l2, const int& x2, const int& y2): start(l1, x1, y1), end(l2, x2, y2){}
//        Edge(int&& l1, int&& x1, int&& y1, int&& l2, int&& x2, int&& y2) noexcept: start(std::move(l1), std::move(x1), std::move(y1)), end(std::move(l2), std::move(x2), std::move(y2)){}
        friend std::ostream& operator<<(std::ostream& os, const Edge& edge){return os << edge.start << " "<< edge.end;}
    };

    class Via{
        friend class Guide;

        private:
            bool settled = false;       //是否已经被设置
            bool valid = false;         //是否有效
            int maxLayer = BLANKLAYER;
            int minLayer = BLANKLAYER;
            Location2D loc2D;
            std::vector<Clue>   NodeClues;
            void addNode(const Clue& NodeClue){NodeClues.emplace_back(NodeClue);}
            
        public:
            Via(){}
            Via(const Location2D& Loc2D):loc2D(Loc2D){}
            Via(const int&x, const int& y):loc2D(x, y){}

            const Edge getEdge() const{return Edge(minLayer, loc2D.x, loc2D.y, maxLayer, loc2D.x, loc2D.y);}
            const int& getminLayer() const{return minLayer;}
            const int& getmaxLayer() const{return maxLayer;}
            const Location2D& getLocation2D() const{return loc2D;}
            const bool& isValid() const{return valid;}
            const bool& isSettled() const{return settled;}
            const bool isCounted() const {return valid && settled;}

            bool operator==(const Via& other) const{return loc2D == other.loc2D;}
            bool operator!=(const Via& other) const{return !(*this == other);}
    };
    
    class Wire{
        friend class Guide;

        private:
            Location2D start, end;
            Direction direction;
            int layer;
            bool startLinkVia = false;
            bool endLinkVia = false;
            int startViaIdx = -1;
            int endViaIdx = -1;

        public:
            Wire(Location2D&& startloc2D, Location2D&& endloc2D, int&& layer = BLANKLAYER) noexcept
                : start(std::move(startloc2D)), end(std::move(endloc2D)), layer(layer){
                if(start.x != end.x)
                    direction = HORIZONTAL;
                else if(start.y != end.y)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }
            Wire(const Location2D& startloc2D, const Location2D& endloc2D, const int& layer = BLANKLAYER)
                : start(startloc2D), end(endloc2D), layer(layer){
                if(start.x != end.x)
                    direction = HORIZONTAL;
                else if(start.y != end.y)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }
            Wire(int&& layer, int&& xstart, int&& ystart, int&& xend, int&& yend) noexcept
                :layer(layer) ,start(std::move(xstart), std::move(ystart)), end(std::move(xend), std::move(yend)){
                if(xstart != xend)
                    direction = HORIZONTAL;
                else if(ystart != yend)
                    direction = VERTICAL;
                else
                    direction = UNDEFINED;
            }

            const Direction& getDirection() const{return direction;}
            const Location2D& getStart()const{return start;}
            const Location2D& getEnd() const{return end;}
            const int& getLayer()const{return layer;}

            void setLayer(const int& l){layer = l;}

            const Edge getEdge() const{
                if(direction == HORIZONTAL && start.x < end.x){
                    return Edge(start, end, layer);
                }else if(direction == VERTICAL && start.y < end.y){
                    return Edge(start, end, layer);
                }else{
                    return Edge(end, start, layer);
                }
            }

            const bool& isStartLinkVia() const{return startLinkVia;}
            const bool& isEndLinkVia() const{return endLinkVia;}
            const int& getStartViaIdx() const{return startViaIdx;}
            const int& getEndViaIdx() const{return endViaIdx;}
    };

    class Pin{
        friend class Guide;
        private:
            Location2D loc2D;
            int layer;
            bool linkVia = false;
            int ViaIdx = -1;

        public:
            Pin(const int& layer, const Location2D& loc2D):layer(layer), loc2D(loc2D){}
            Pin(const int&layer, const int& x, const int& y): layer(layer), loc2D(x, y){}

            Pin(int&& layer, int&& x, int&& y) noexcept: layer(std::move(layer)), loc2D(std::move(x), std::move(y)) {}
            Pin(const Location&& loc) noexcept: layer(std::move(loc.l)), loc2D(std::move(loc.x), std::move(loc.y)) {}
            
            friend std::ostream& operator<<(std::ostream& os, const Pin& pin){return os << pin.loc2D << " " << pin.layer;}
    };

    class Guide{
        friend class Solution;

        private:
            std::vector<Wire> wires;
            std::vector<Via> vias;
            std::vector<Pin> pins;
            std::vector<Pin> Virtualpins;
            int ViaCount = 0;
            void updateVia(const int& ViaIdx);
            bool targetPin(const Location& loc);                        //标记pin的loc是否有效
            void addNodetoVia(const int& ViaIdx, const Clue& NodeClue); //将node添加到Via中
            void setLayerofWire(const int& WireIdx, const int& l);      //设置wire的layer
            const int& Link(const Clue& nc1, const Clue& nc2);           //在wire之间创建Via,自动管理
            void addVia(const int& x, const int& y);                    //添加Via

        public:
            std::string netname;
            Guide(const std::string& name): netname(name){}
            
            void output(std::ostream& os) const;                                        //输出Guide

            const Location2D& getLocation2D(const Clue& NodeClue)const;                        //获取Node
            const int& getNodeLayer(const Clue& NodeClue)const;             //获取Node的layer

            const Wire& getWire(const int& i) const{return wires[i];}
            const std::vector<Wire>& getWires() const{return wires;}
            const Via& getVia(const int& i) const{return vias[i];}
            const std::vector<Via>& getVias() const{return vias;}

            const Direction& getDirectionofWire(const int& i) const{return wires[i].direction;}
            const int& getLayerofWire(const int& i) const{return wires[i].layer;}
            const int getWireNum() const{return wires.size();}
            const int getViaNum() const{return vias.size();}
            const int getViaCount() const{return ViaCount;}
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
                    guide.output(std::cout);
            }
            
            Solution(const std::string& guide2Dfilename, const std::string& netfilename){loadfile(guide2Dfilename);mergefile(netfilename);}
            const int getNetNum() const{return guides.size();}
            const Guide& getGuide(const int& guideIdx) const;
            
            void loadfile(const std::string& filename);
            void mergefile(const std::string& filename);
            void outputdebug(const std::string& filename) const;
            void setLayerofWire(const int& guideIdx, const int& WireIdx, const int& layer);
            
    };
}