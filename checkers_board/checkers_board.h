#ifndef CHECKERS_BOARD_H
#define CHECKERS_BOARD_H

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <random>
#include <utility>
#include <functional>
#include <fstream>
#include <ncurses.h>


constexpr int COMP = 1;
constexpr int PLAYER = 2;

constexpr double EXPLORATION_PARAMETER = 1.4142;
constexpr double EULER = 2.71828182845904523536;

constexpr bool DEBUG = false;

#define PRINT_BOARD(board) \
    do { \
        for (auto& row : board) { \
            for (auto& e : row) { \
                file << e.player; \
            } \
            file << std::endl; \
        } \
    } while(0)

// bool S = true;

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;  // Combine the two hash values.
    }
};

struct Square {
    int player;
    bool selected;
    bool highlighted;
    bool kinged;
};


struct CompSquare {
    std::pair<int, int> coordinate;
    std::vector<std::pair<int, int> > possibleMoves;
    // void opterator=(const CompSquare& c);
};


class Node 
{
public:
    double calculateValue();
    Node(const std::string& key, Node* p, const bool& kinged, const int& turn);

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

    std::vector<std::pair<int, int> > highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& modify, const bool& forceTake);
    
    void Move();

    void Play();

    std::string serializeBoard() const; // actually no fucking clue how to do this
    std::vector<std::vector<int> > deserializeBoard(const std::string& serial) const;
    std::vector<std::vector<Square> > m_Board;
    bool makeRandomMove(std::unordered_map<int, std::unordered_map<std::pair<int, int>, CompSquare*, pair_hash> >& pieceMap, const int& y0, const int& x0);

private:
    bool SelectSquare(const std::string& prompt, bool selectingMove);
    void GetPlayerMove(const bool chaining);
    bool isChain(const unsigned& y, const unsigned& x, const unsigned& oppositePlayer);
    bool checkKings();
    void movePiece();
    int winner();

    void getCompMove();
    std::unordered_map<std::pair<int, int>, CompSquare*, pair_hash> compileCompPieces(const int& p, const bool& chaining);


    // bool makeRandomMove(const int& player,  bool& S, const int& i);
    void simulateRandomGame();

    // std::string serializeBoard() const; // actually no fucking clue how to do this
    // std::vector<std::vector<int> > deserializeBoard(const std::string& serial) const;

    void makeBestCompMove();
    void updateRootNode();


    // std::vector<std::vector<Square> > m_Board;
    std::pair<unsigned, unsigned> m_Selected;  
    std::pair<int, int> m_ToMove;
    int m_Turn;
    std::mt19937 m_Gen;
    std::uniform_int_distribution<> distrib; // Uniform distribution

    Node* m_RootNode;
    std::ofstream file;
};





#endif