#include "Catch2/catch.hpp"
#include "checkers_board.h"



TEST_CASE("serializationTestOne", "[Required]")
{
    CheckersBoard cb{};

    std::string serial = cb.serializeBoard();

    std::vector<std::vector<int>> deserial = cb.deserializeBoard(serial);

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x) 
        {
            REQUIRE( cb.m_Board[y][x].player == deserial[y][x] );
        }
    }
}


TEST_CASE("serializationTestThree", "[Required]")
{
    CheckersBoard cb{};
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            cb.m_Board[i][j].player = 0;
        }
    }

    cb.m_Board[0][1].player = COMP;
    cb.m_Board[6][7].player = PLAYER;

    cb.m_Board[0][7].player = COMP;
    cb.m_Board[7][0].player = PLAYER;


    std::string serial = cb.serializeBoard();

    std::vector<std::vector<int>> deserial = cb.deserializeBoard(serial);

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x) 
        {
            REQUIRE( cb.m_Board[y][x].player == deserial[y][x] );
        }
    }

}