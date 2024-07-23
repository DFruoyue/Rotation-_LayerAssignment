#pragma once

#include "Gcell.h"
#include <vector>
#include <string>
#include <fstream>
namespace XZA{
    struct Edge{
        Location start, end;
        Edge(const Location& start,const Location& end)
            : start(start), end(end)
        {}
        Edge(int l1, int x1, int y1, int l2, int x2, int y2)
            : start(l1, x1, y1), end(l2, x2, y2)
        {}
        friend std::ostream& operator<<(std::ostream& os, const Edge& edge){
            return os << edge.start << edge.end;
        }
    };

    class Via;
    class Node{
        public:
            Location loc;
            bool linkVia;
            Node(const Location& loc, const bool& linkVia = false)
                : loc(loc), linkVia(linkVia)
            {}
            Node(const int& l, const int& x, const int& y, const bool& linkVia = false)
                : loc(l, x, y), linkVia(linkVia)
            {}

            Via* pVia;

            void setVia(Via* pVia){
                linkVia = true;
                this -> pVia = pVia;
            }
    };

    class Via{
        friend class Wire;

        private:
            std::vector<Node*>   pNodes;

        public:
            Via(std::vector<Node*> pNodes)
                : pNodes(pNodes)
            {}
            Via()
            {}

            void addNode(Node* pNode){
                bool isInserted = false;
                for(int i=0;i<pNodes.size();i++){
                    if(pNodes[i]-> loc.l >= pNode->loc.l){
                        pNodes.insert(pNodes.begin() + i, pNode);
                        isInserted = true;
                        break;
                    }
                }
                if(!isInserted){
                    pNodes.push_back(pNode);
                }
            }
            Edge getEdge() const{
                return Edge(pNodes[0]->loc, pNodes[pNodes.size() - 1]->loc);
            }
            void update(){
                for(int i=0;i<pNodes.size() - 1;i++){
                    if(pNodes[i]->loc.l > pNodes[i+1]->loc.l)
                        std::swap(pNodes[i], pNodes[i+1]);
                }
            }
    };
    
    class Wire{
        private:
            Node start, end;

        public:
            Edge getEdge() const{
                return Edge(start.loc, end.loc);
            }
            void setLayer(const int& l){
                start.loc.l = l;
                if(start.linkVia)
                    start.pVia -> update();

                end.loc.l = l;
                if(end.linkVia)
                    end.pVia -> update();
            }
            Wire()
            : start(-1, 0, 0, false), end(-1, 0, 0, false)
            {}
            Wire(const Location& start, const Location& end)
                : start(start, false), end(end, false)
            {}
            Wire(const int& lstart, const int& xstart, const int& ystart, const int& lend, const int& xend, const int& yend)
                : start(lstart, xstart, ystart, false), end(lend, xend, yend, false)
            {}
        friend class Guide;
    };


    class Guide{
        private:
            std::vector<Wire> wires;
            std::vector<Via> vias;
            
        public:
            std::string netname;
            Guide();
            void loadfile(std::ifstream& guide2Dfile);//初始化, 创建Via和Wire
            void targetPin(const Location& loc);
            const Via* Link(Node* pNode1, Node* pNode2);    //在wire之间创建Via,自动管理
            void output() const;
    };
    using Solution = std::vector<Guide>;
}