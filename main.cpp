#include <iostream>
#include <cstdlib>
#include <ctime>

#include "checkers_board.h"


int main() 
{
    // std::cout << "Hello World" << std::endl;
    CheckersBoard cb;

    cb.Play();
    // std::srand(std::time(0));

    // // RAND_MAX = 32767;
    // for (int i = 0; i < 10 ; ++i)
    // {
    //     std::cout << "Random value on [0, " << 13 << "]: " << std::rand() % 13<< '\n';
    // }
    return 0;
}