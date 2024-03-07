#ifndef CHECKERS_BOARD_H
#define CHECKERS_BOARD_H

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_map>

constexpr int COMP = 1;
constexpr int PLAYER = 2;

constexpr unsigned EXPLORATION_PARAMETER = 3;
constexpr double EULER = 2.71828182845904523536;

struct Square {
    int player;
    bool selected;
    bool highlighted;
    bool kinged;
};


struct CompSquare {
    std::pair<int, int> coordinate;
    std::vector<std::pair<int, int>> possibleMoves;
};


class Node 
{
public:
    double calculateValue() const;
    Node(Node* p);

    unsigned m_TotalSimulations;
    unsigned m_WinningSimulations;

    Node* m_ParentNode;
    std::vector<Node*> m_ChildNodes;
    std::string m_Key;   // ensure that there are no duplicates
};


class CheckersBoard
{
public:
    CheckersBoard();
    ~CheckersBoard();

    void Draw();

    std::vector<std::pair<int, int>> highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& chaining);
    
    void Move();

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


    std::pair<CompSquare, std::pair<int, int>> makeRandomMove(const int& player);
    void simulateRandomGame();
    std::string serializeBoard() const; // actually no fucking clue how to do this


    std::vector<std::vector<Square>> m_Board;
    std::pair<unsigned, unsigned> m_Selected;  
    std::pair<int, int> m_ToMove;
    int m_Turn;

    Node* m_RootNode;
    std::unordered_map<std::string, Node*> m_GameStates;  
};





#endif