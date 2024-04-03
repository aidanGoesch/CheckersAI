#ifndef CHECKERS_BOARD_H
#define CHECKERS_BOARD_H

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <string>

constexpr int COMP = 1;
constexpr int PLAYER = 2;

constexpr unsigned EXPLORATION_PARAMETER = 3;
constexpr double EULER = 2.71828182845904523536;

constexpr bool DEBUG = false;

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
    double calculateValue();
    Node(const std::string& key, Node* p, const bool& kinged);

    unsigned m_TotalSimulations;
    unsigned m_WinningSimulations;

    Node* m_ParentNode;
    std::vector<Node*> m_ChildNodes;
    std::string m_Key;   // ensure that there are no duplicates
    int m_Turn;          // Indicates who's turn it is (e.i. who is making the move / who can win)
    double m_Score;
    bool m_KingedPiece;   // Indicates whether a piece was kinged to get to this new state
}; 


class CheckersBoard
{
public:
    CheckersBoard();
    ~CheckersBoard();

    void Draw();

    std::vector<std::pair<int, int>> highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& modify);
    
    void Move();

    void Play();

private:
    bool SelectSquare(const std::string& prompt, bool selectingMove);
    void GetPlayerMove(const bool chaining);
    bool isChain(const unsigned& y, const unsigned& x, const unsigned& oppositePlayer);
    bool checkKings();
    void movePiece();
    int winner();

    void getCompMove();
    std::vector<CompSquare> compileCompPieces(const int& p);


    bool makeRandomMove(const int& player, const int& i);
    void simulateRandomGame();
    std::string serializeBoard() const; // actually no fucking clue how to do this
    std::vector<std::vector<int>> deserializeBoard(const std::string& serial) const;
    void makeBestCompMove();
    void updateRootNode();


    std::vector<std::vector<Square>> m_Board;
    std::pair<unsigned, unsigned> m_Selected;  
    std::pair<int, int> m_ToMove;
    int m_Turn;

    Node* m_RootNode;
    std::unordered_map<std::string, Node*> m_GameStates;  
};





#endif