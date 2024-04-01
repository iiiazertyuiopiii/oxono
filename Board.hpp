#include "stdint.h"
#include <iostream>

class Board {

    //moves is a list of triple (totem position,cell position,move type)
    struct move{
        unsigned int totem;
        unsigned int cell;
        bool isO;
    };
    typedef std::vector<move> moves;

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
        static constexpr int64_t FULL_COL = 63;
        int64_t cells; // bitmap of the non empty cells
        int64_t cur_cells; // bitmap of the cells of active player
        
        int64_t cellsO; // bitmap of the 'o/O' cells

        int64_t cellsX; // bitmap of the 'x/X' cells 
        
        unsigned int movesO[2]; //number of 'o/O' moves
        unsigned int movesX[2]; //number of 'x/X' moves
        unsigned int totemO;
        unsigned int totemX;

        bool curPlayer;

    std::vector<unsigned int> allFreeCells(){
        std::vector<unsigned int> res;
        int i=0;
        for(i=0;i<36;i++){
            if(!full(i) && i != totemO && i != totemX) res.push_back(i);
        }
        return res;
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

    std::vector<unsigned int> legalMovesAround(unsigned int pos){
        std::vector<unsigned int> result;
        if(pos%6 !=5 && !full(pos+1)){ //try down
            result.push_back(pos+1);
        } 
        if(pos>5 && !full(pos-6)){ //left
            result.push_back(pos-6);
        }
        if(pos%6 != 0 && !full(pos-1)){ //up
            result.push_back(pos-1);
        }
        if(pos<30 && !full(pos+6)){ //right
            result.push_back(pos+6);
        }
        if(!result.empty()){
            return result;
        } else { //if totem is surrounded, we can play anwyhere
            return allFreeCells();
        }
    }


    std::vector<unsigned int> legalTotemMoves(unsigned int pos){
        std::vector<unsigned int> res;
        bool noD = false,noL = false,noU = false,noR = false;
        //try all 4 directions but stop upon hitting wall or full cell
        for(int i=0;i<=4;i++){
            
            if(pos%6 < 6-(i+1) && !full(pos+1+i) && !noD){ //try down
                res.push_back(pos+(i+1));
            } else {
                noD = true;
            } 
            
            if(pos>5+6*i && !full(pos-6*(i+1)) && !noL){ //try left
                res.push_back(pos-(6*(i+1)));
            } else {
                noL = true;
            }
            
            if(pos%6 > i && !full(pos-(i+1)) && !noU){
                res.push_back(pos-(i+1));
            } else {
                noU = true;
            }
            
            if(pos<30-6*i && !full(pos+6*(i+1)) && !noR){
                res.push_back(pos+(6*(i+1)));
            } else {
                noR = true;
            }
        }
        //try all 4 directions skipping full cells but stop upon finding first free cell
        if(res.empty()){
             bool d = false,l = false,u = false,r = false;
            for(int i=1;i<=4;i++){
                if(pos%6 < 6-(i+1) && !full(pos+1+i) && !d){ //try down
                    res.push_back(pos+(i+1));
                    d=true;
                } 
                if(pos>5+6*i && !full(pos-6*(i+1)) && !l){ //try left
                    res.push_back(pos-(6*(i+1)));
                    l=true;
                } 
                if(pos%6 > i && !full(pos-(i+1)) && !u){
                    res.push_back(pos-(i+1));
                    u = true;
                } 
                if(pos<30-6*i && !full(pos+6*(i+1)) && !r){
                    res.push_back(pos+(6*(i+1)));
                    r = true;
                }
            }
        }
        if(!res.empty()){
            /*std::cout << "Legal moves for totem : ";
            for(unsigned int i : res){
                std::cout << i << " ";
            }
            std::cout << std::endl;*/
            return res;
        } else { //in the rare case nothing is free for the totem horizontally/diagonally, it can go anywhere
            std::cout << "all moves legal for totem!";
            return allFreeCells();
        }
    }

    int64_t computeWinning(int64_t c, unsigned int played){
        int64_t winning = 0;
        //horizontal
        int64_t p = (c << 6) & (c << 2 * 6);
        winning |= p & (c << 3 * 6);
        winning |= p & (c >> 6);
        p = (c >> 6) & (c >> 2 * 6);
        winning |= p & (c << 6);
        winning |= p & (c >> 3 * 6);

        //vertical - don't allow crossing current column
        p = (c << 1) & (c << 2);
        if(played % 6 > 2){
            winning |= p & (c << 3);
        }
        if(played % 6 > 1 && played % 6 < 5){
            winning |= p & (c >> 1);
        }
        p = (c >> 1) & (c >> 2);
        if(played % 6 < 4 && played % 6 > 0){
            winning |= p & (c << 1);
        }
        if(played % 6 < 3){
            winning |= p & (c >> 3);
        }
        return winning;
    }

    bool isWinningMove(unsigned int played, bool playO){
        
        // check win by color
        if(computeWinning(cur_cells, played) & (1LL << played)){
            std::cout << "Win by color !\n";
            return true;
        }
        //check win by x/X or o/O
        if(computeWinning(playO?cellsO:cellsX, played) & (1LL << played)){
            std::cout << "Win by shape !\n";
            return true;
        } 

        return false;
    }

    moves getNextMoves(){
        moves result;

        bool canPlayO = movesO[curPlayer] < 8; //try to play o first always. in this game it's always possible to move totem
        bool canPlayX = movesX[curPlayer] < 8;
        
        if(!canPlayO && !canPlayX){ //current player can't play anymore, so game over
            return result;
        }

        if(canPlayO){
            unsigned int oldT = totemO;
            for(unsigned int t : legalTotemMoves(totemO)){
                totemO = t; //legal moves need to be computed with the new totem position
                for(unsigned int c : legalMovesAround(t)){
                    result.push_back({t,c,true});
                }
            }
            totemO = oldT;
        }

        if(canPlayX){
            unsigned int oldT = totemX;
            for(unsigned int t : legalTotemMoves(totemX)){
                totemX = t;
                for(unsigned int c : legalMovesAround(t)){
                    result.push_back({t,c,false});
                }
            }
            totemX = oldT;
        }

        return result;
    }

    move getFirstLegalMove(moves& m){
        return m.at(0);
    }

    move getRdMove(moves& m){
        return m.at(std::rand()%m.size());
    }

    public:

    bool playLoop(){
        moves allMoves = getNextMoves();
        if(allMoves.empty()){ //current player can't play anymore, so game over
            return false;
        }
        //move m = getFirstLegalMove(allMoves);
        move m = getRdMove(allMoves);
        unsigned int played = m.cell;
        m.isO?totemO = m.totem : totemX = m.totem;
        set(played,m.isO);
        if(isWinningMove(played,m.isO)){
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