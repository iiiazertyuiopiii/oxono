#include <iostream>
#include <vector>
#include <string>

#include "Board.hpp"

using namespace std;

int main()
{
    cout << "*********************************************\n";
    Board b;
    b.dump();
    while(b.playFirstValidMove()){
        b.dump();
    }
    b.dump();
}   