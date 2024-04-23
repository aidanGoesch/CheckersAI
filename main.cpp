// for tests

// #define CATCH_CONFIG_RUNNER
// #include "Catch2/catch.hpp"

// #include <iostream>

// int main(int argc, char* argv[])
// {
//     const int retval = Catch::Session().run(argc, argv);
    
//     return retval;
// }


#include "checkers_board.h"

int main() 
{
    CheckersBoard cb;

    cb.Play();

    // "100010002000000100000000000000001";
    // auto tmp = cb.deserializeBoard("100010002000000100000000000000001");

    // for (auto row : tmp)
    // {
    //     for (auto e : row)
    //     {
    //         std::cout << e << " ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}