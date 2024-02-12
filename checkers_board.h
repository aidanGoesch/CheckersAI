#ifndef CHECKERS_BOARD_H
#define CHECKERS_BOARD_H

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

constexpr int COMP = 1;
constexpr int PLAYER = 2;

const unsigned EXPLORATION_PARAMETER = 3;
const double EULER = 2.71828182845904523536;

struct Square {
    int player;
    bool selected;
    bool highlighted;
    bool kinged;
};


struct CompSquare {
    std::pair<int, int> coordinate;
    std::vector<std::pair<int, int>> possibleMoves;

    bool operator==(const CompSquare& other);
    bool operator< (const CompSquare& other);
    bool operator> (const CompSquare& other);
};


class CheckersBoard
{
public:
    CheckersBoard();
    ~CheckersBoard();

    void Draw();

    std::vector<std::pair<int, int>> highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& chaining);
    
    void Play();

private:
    bool SelectSquare(const std::string& prompt, bool selectingMove);
    void GetPlayerMove(const bool chaining);
    bool isChain(const unsigned& oppositePlayer);
    void checkKings();
    void movePiece();
    int winner() const;

    void getCompMove();
    std::vector<CompSquare> compileCompPieces(const int& p);


    std::vector<std::vector<Square>> makeRandomMove(const int& player);


    std::vector<std::vector<Square>> m_Board;
    std::pair<unsigned, unsigned> m_Selected;  
    std::pair<int, int> m_ToMove;
};


class Node 
{
public:
    double calculateValue() const;
    Node(Node* p);

private:
    unsigned totalSimulations;
    unsigned winningSimulations;

    Node* parentNode;
    std::vector<Node*> childNodes;
};


#endif