// for tests

// #define CATCH_CONFIG_RUNNER
// #include "Catch2/catch.hpp"

// #include <iostream>

// int main(int argc, char* argv[])
// {
//     const int retval = Catch::Session().run(argc, argv);
    
//     return retval;
// }


#include "checkers_board/checkers_board.h"
// #include <iostream>

int main() 
{
    CheckersBoard cb;

    cb.Play();
    
    std::cout << "Process Finished" << std::endl;

    return 0;
}