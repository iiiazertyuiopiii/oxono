#include "stdint.h"
#include <iostream>

class Board {

    public:
    /*
    0  6 12 18 24 30
    1  7 13 19 25 31
    2  8 14 20 26 32
    3  9 15 21 27 33
    4 10 16 22 28 34
    5 11 17 23 29 35
    */

    Board() : cells(0),cur_cells(0),cellsO(0),cellsX(0),movesO{0,0},movesX{0,0},totemO(14),totemX(21),curPlayer(0){}

    private:
        int64_t cells; // bitmap of the non empty cells
        int64_t cur_cells; // bitmap of the cells of active player
        
        int64_t cellsO; // bitmap of the 'o/O' cells

        int64_t cellsX; // bitmap of the 'x/X' cells 
        
        unsigned int movesO[2]; //number of 'o/O' moves
        unsigned int movesX[2]; //number of 'x/X' moves
        unsigned int totemO;
        unsigned int totemX;

        bool curPlayer;

    unsigned int firstFreeCell(){
        int i=0;
        for(i=0;i<36;i++){
            if(!full(i) && i != totemO && i != totemX)break;
        }
        return i;
    }

    bool full(unsigned int pos){
        return cells & (1LL << pos) || pos == totemO || pos == totemX;
    }

    bool isCur(unsigned int pos){
        return cur_cells & (1LL << pos);
    }

    bool isO(unsigned int pos){
        return cellsO & (1LL << pos);
    }

    bool isX(unsigned int pos){
        return cellsX & (1LL << pos);
    }

    void set(unsigned int pos, bool o){
        cells |= 1LL << pos;
        cur_cells |= 1LL << pos;
        o?cellsO |= 1LL << pos:cellsX |= 1LL << pos;
        o?movesO[curPlayer] = movesO[curPlayer]+1 : movesX[curPlayer] = movesX[curPlayer]+1;
    }

    unsigned int tryPlayAround(unsigned int pos, bool o){
        if(pos%6 !=5 && !full(pos+1)){ //try down
            pos=pos+1;
        } else if(pos>5 && !full(pos-6)){ //left
            pos=pos-6;
        } else if(pos%6 != 0 && !full(pos-1)){ //up
            pos=pos-1;
        } else if(pos<30 && !full(pos+6)){ //right
            pos=pos+6;
        } else { //if totem is surrounded, we can play anwyhere
            pos = firstFreeCell();
        }
        set(pos,o);
        return pos;
    }

    unsigned int moveTotem(unsigned int pos){
        for(int i=0;i<4;i++){
            if(pos%6 < 6-(i+1) && !full(pos+1+i)){ //try down
               return pos+=(i+1);
            } else if(pos>5+6*i && !full(pos-6*(i+1))){ //try left
                return pos-=(6*(i+1));
            } else if(pos%6 > i && !full(pos-(i+1))){
                return pos-=(i+1);
            } else if(pos<30-6*i && !full(pos+6*(i+1))){
                return pos+=(6*(i+1));
            }
        }
        return firstFreeCell();
    }

    int64_t computeWinning(int64_t c){
        int64_t winning = 0;
        int64_t p = (c << 6) & (c << 2 * 6);
        winning |= p & (c << 3 * 6);
        winning |= p & (c >> 6);
        p = (c >> 6) & (c >> 2 * 6);
        winning |= p & (c << 6);
        winning |= p & (c >> 3 * 6);

        //vertical
        p = (c << 1) & (c << 2 );
        winning |= p & (c << 3);
        winning |= p & (c >> 1);
        p = (c >> 1) & (c >> 2);
        winning |= p & (c << 1);
        winning |= p & (c >> 3);
        return winning;
    }

    bool isWinningMove(unsigned int played, bool playO){
        
        // check win by color
        if(computeWinning(cur_cells) & (1LL << played)) return true;
        //check win by x/X or o/O
        if(computeWinning(playO?cellsO:cellsX) & (1LL << played)) return true;

        return false;
    }

    public:

    bool playFirstValidMove(){
        bool playO = movesO[curPlayer] < 8; //try to play o first always. in this game it's always possible to move totem
        bool playX = movesX[curPlayer] < 8;
        if(!playO && !playX){ //current player can't play anymore, so game over
            return false;
        }
        unsigned int played;
        if (playO){
            totemO = moveTotem(totemO);
            played = tryPlayAround(totemO,true);
        } else {
            totemX = moveTotem(totemX);
            played = tryPlayAround(totemX,false);
        }
        if(isWinningMove(played,playO)){
            return false;
        }
        curPlayer = !curPlayer;
        cur_cells = cells & ~cur_cells;
        return true;
    }

    void dump(){
        for(int i=0;i<6;i++){
            for(int j=i;j<=30+i;j+=6){
                if(totemO == j){
                    std::cout << "@ ";
                } else if(totemX == j){
                    std::cout << "# ";
                } else if(full(j)){
                    if(isO(j)){
                        if(isCur(j) && curPlayer || !isCur(j) && !curPlayer){
                          std::cout << "O "; 
                        } else {
                          std::cout << "o ";
                        }
                    } else {
                        if(isCur(j) && curPlayer || !isCur(j) && !curPlayer){
                            std::cout << "X ";
                        } else {
                            std::cout << "x ";
                        }
                    }
                } else {
                    std::cout << "- ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "Total played o : " << +movesO[0] << ",total played x : " << +movesX[0] << std::endl;
        std::cout << "Total played O : " << +movesO[1] << ",total played X : " << +movesX[1] << std::endl;
    }

};