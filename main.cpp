#include <iostream>
#include <vector>
#include <string>

#include "Board.hpp"

using namespace std;

int main()
{
    unsigned int seed = time(nullptr);
    cout << "********************************************* Seed : " << seed << endl;
    srand(seed);
    Board b;
    b.dump();
    while(b.playLoop()){
        b.dump();
    }
    b.dump();
}   