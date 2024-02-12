#include "checkers_board.h"

#include <windows.h>


void SetConsoleColors(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, textColor | (bgColor << 4));
}


void ClearConsole() {
    COORD topLeft = {0, 0};
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}


CheckersBoard::CheckersBoard() : m_Selected({6, 3}), m_ToMove({-1, -1})
{
    unsigned offset = 1;
    for (int y = 0; y < 8; ++y)
    {
        std::vector<Square> tmp;
        for (int x = 0; x < 8; ++x)
        {
            if ( y <= 2 && offset % 2 == 0 ) { tmp.push_back(Square{COMP, false, false, false}); }
            else if ( y >= 5 && offset % 2 == 0 ) { tmp.push_back(Square{PLAYER, false, false, false}); }
            else { tmp.push_back(Square{0, false, false, false}); }

            ++offset;
        }
        ++offset;
        m_Board.push_back(tmp);
    }

    m_Board[6][3].selected = true;
    m_Board[3][6].player = 1;
    m_Board[0][5].player = 0;
    m_Board[2][7].player = 0;
    m_Board[4][5].player = 2;
    m_Board[5][4].player = 0;
    m_Board[5][6].player = 0;
    m_Board[2][3].player = 0;
    
}

CheckersBoard::~CheckersBoard()
{
    return ;
}

void CheckersBoard::Draw()
{
    ClearConsole();
    std::cout << "+---+---+---+---+---+---+---+---+" << std::endl;
    for (auto row : m_Board)
    {
        std::cout << "|";
        for (auto e : row)
        {
            int bg = 0;
            if ( e.highlighted ) { bg = FOREGROUND_RED | FOREGROUND_GREEN ; }
            if ( e.selected ) { bg = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; }

            if ( e.player == 0 )
            {
                SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, bg);
                std::cout << "   ";         
            } else 
            {
                if ( e.player == 1 ) { SetConsoleColors(FOREGROUND_RED, bg); }
                else if (e.player == 2 ) { SetConsoleColors(FOREGROUND_BLUE, bg); }

                std::cout << " ● ";
            }
            
            SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
            std::cout << "|";
            
        }
        std::cout << std::endl;
        std::cout << "+---+---+---+---+---+---+---+---+" << std::endl;
    }
}


bool CheckersBoard::SelectSquare(const std::string& prompt, bool selectingMove)
{
    Draw();
    std::cout << prompt << std::endl;

    HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

    if (hConsoleInput == INVALID_HANDLE_VALUE) {
        std::cerr << "Error getting console input handle." << std::endl;
        return true;
    }

    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;

    SetConsoleMode(hConsoleInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

    while (true) {
        if (ReadConsoleInput(hConsoleInput, irInBuf, 128, &cNumRead)) {
            for (DWORD i = 0; i < cNumRead; ++i) {
                if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {
                    // Handle key press event
                    char pressedKey = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;

                    unsigned y = m_Selected.first; 
                    unsigned x = m_Selected.second;


                    if (pressedKey == '\r') {
                        if (selectingMove && m_Board[y][x].highlighted) return false;
                        if (! selectingMove && m_Board[y][x].player == 2)
                        {
                            m_ToMove = {y, x};
                            return false;  // this doesn't matter
                        }
                        if (selectingMove) return true;
                    }

                    if (pressedKey == 'w')
                    {
                        if ( y >= 1) {
                            m_Selected = {y-1, x};

                            m_Board[y][x].selected = false;
                            m_Board[y-1][x].selected = true;
                        }
                    } else if (pressedKey == 's')
                    {
                        if ( y + 1 < 8) {
                            m_Selected = {y+1, x};
                            
                            m_Board[y][x].selected = false;
                            m_Board[y+1][x].selected = true;
                        }
                    } else if (pressedKey == 'a')
                    {
                        if ( x >= 1) {
                            m_Selected = {y, x-1};

                            m_Board[y][x].selected = false;
                            m_Board[y][x-1].selected = true;
                        }
                    } else if (pressedKey == 'd')
                    {
                        if ( x + 1 < 8) {
                            m_Selected = {y, x+1};
                            
                            m_Board[y][x].selected = false;
                            m_Board[y][x+1].selected = true;
                        }
                    }

                    Draw();
                    std::cout << prompt << std::endl;
                }
            }
        }
    }
}

bool CheckersBoard::isChain(const unsigned& oppositePlayer)
{
    unsigned y = m_Selected.first;
    unsigned x = m_Selected.second;
    

    if (y > 1) 
    { // Forward
        if (x > 1 && m_Board[y-1][x-1].player == oppositePlayer && m_Board[y-2][x-2].player == 0)
            return true;
        if (x < 6 && m_Board[y-1][x+1].player == oppositePlayer && m_Board[y-2][x+2].player == 0)
            return true;
    }

    if (m_Board[y][x].kinged && y < 6)
    std::cout << 1;
    {  // Backward
        if (x > 1 && m_Board[y+1][x-1].player == 1 && m_Board[y+2][x-2].player == 0)
            return true;
        std::cout << 2;
        if (x < 7 && m_Board[y+1][x+1].player == 1 && m_Board[y+2][x+2].player == 0)
            return true;
        std::cout << 3;
    }
    return false;
}


std::vector<std::pair<int, int>> CheckersBoard::highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& chaining)
{
    std::vector<std::pair<int, int>> ret;
    // highlight possible moves for that square

    bool forceMove = false;  // forces players to take a piece if they can
    int opp = (piece == PLAYER) ? 1 : 2;

    if (y > 1)
    {
        if (x > 1 && m_Board[y-1][x-1].player == opp && m_Board[y-2][x-2].player == 0)
        {
            m_Board[y-2][x-2].highlighted = true;
            forceMove = true;
            ret.push_back({y-2, x-2});
        }

        if (x < 7 && m_Board[y-1][x+1].player == opp && m_Board[y-2][x+2].player == 0)
        {
            m_Board[y-2][x+2].highlighted = true;
            forceMove = true;
            ret.push_back({y-2, x+2});
        }
    }

    if (y > 0 && !chaining  && !forceMove)  // highlights all possible moves
    {
        // Forwards
        if (x > 1 && m_Board[y-1][x-1].player == 0)
            m_Board[y-1][x-1].highlighted = true;
            ret.push_back({y-1, x-1});

        if (x < 7 && m_Board[y-1][x+1].player == 0)
            m_Board[y-1][x+1].highlighted = true;
            ret.push_back({y-1, x+1});
    }
    

    if (m_Board[y][x].kinged)
    {
        forceMove = false;
        if (m_Board[y][x].kinged && y < 6) 
        {
            if (x > 1 && m_Board[y+1][x-1].player == opp && m_Board[y+2][x-2].player == 0)
            {
                m_Board[y+2][x-2].highlighted = true;
                forceMove = true;
                ret.push_back({y+2, x-2});
            }

            if (x < 7 && m_Board[y+1][x+1].player == opp && m_Board[y+2][x+2].player == 0)
            {
                m_Board[y+2][x+2].highlighted = true;
                forceMove = true;
                ret.push_back({y+2, x+2});
            }
        }

        if (y < 7 && !chaining && !forceMove)
        {
            // Backwards
            if (x > 1 && m_Board[y+1][x-1].player == 0)
                m_Board[y+1][x-1].highlighted = true;
                ret.push_back({y+1, x-1});

            if (x < 7 && m_Board[y+1][x+1].player == 0)
                m_Board[y+1][x+1].highlighted = true;
                ret.push_back({y+1, x+1});
        }        
    }
    return ret;
}


void CheckersBoard::movePiece()
{
    // update the piece's position
    int y = m_ToMove.first;
    int x = m_ToMove.second;

    int ny = (int)m_Selected.first;
    int nx = (int)m_Selected.second;

    m_Board[y][x].player = 0;
    m_Board[ny][nx].player = 2;

    // transfer king status
    m_Board[ny][nx].kinged = m_Board[y][x].kinged;
    m_Board[y][x].kinged = false;

    m_ToMove = {-1, -1};

    // std::cout << "nx: " << nx << "  x: " << x << "  ny: " << ny << "  y: " << y << std::endl;

    bool chainStarted = false;
    if (abs(nx - x) > 1)  // check if it is taking another piece
    {
        chainStarted = true;
        int z = ((y - ny) / 2) + ny;
        int w = ((x - nx) / 2) + nx;

        m_Board[z][w].player = 0;
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_Board[i][j].highlighted = false;
        }
    }

    // if there is another move in the chain recurse
    if (chainStarted && isChain(1))
    {
        std::cout << "chain" << std::endl;
        GetPlayerMove(true);
    }
}


void CheckersBoard::GetPlayerMove(const bool chaining)
{
    checkKings();
    if (chaining) 
        m_ToMove = m_Selected;

    bool tmp = true;
    while (tmp) 
    {
        if (!chaining)
        {
            SelectSquare("Select a Piece!", false);
        } 

        unsigned y = m_ToMove.first;
        unsigned x = m_ToMove.second;

        highlightPossibleMoves(PLAYER, y, x, chaining);
        std::cout << 1 << std::endl;

        // get where the palyer wants to move the piece
        tmp = SelectSquare("Which Square Would You Like to Move This Piece to?", true);
        if (tmp && !chaining)  // if the player doesn't select a valid move they get to reselect the piece they want to move
        {
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                m_Board[i][j].highlighted = false;
            }
        }
        }
    }

    movePiece();
}


void getCompMove()
{
    ;
}


int CheckersBoard::winner() const
{
    int countOne = 0;
    int countTwo = 0;

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (m_Board[i][j].player == 1)
                ++countOne;
            
            if (m_Board[i][j].player == 2)
                ++countTwo;
        }
    }
    
    // std::cout << countOne << " " << countTwo;
    if (countOne == 0 && countTwo > 0)   // player wins
        return 2;
    
    if (countOne > 0 && countTwo == 0)   // CPU wins
        return 1;

    // check for not being able to make any other moves

    return 0;                            // game not over
}


void CheckersBoard::checkKings()
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_Board[0][i].player == 2)
            m_Board[0][i].kinged = true;

        if (m_Board[7][i].player == 1)
            m_Board[7][i].kinged = true;
    }
}


void CheckersBoard::Play()
{

    ClearConsole();
    // GetPlayerMove(false);
    
    while (winner() == 0) 
    {
        // std::cout << 1;w
        GetPlayerMove(false);
    }
    

    Draw();
}



Node::Node(Node* p) 
: totalSimulations(0), winningSimulations(0), parentNode(p), childNodes(std::vector<Node*>{nullptr}) 
{
}


std::vector<CompSquare> CheckersBoard::compileCompPieces(const int& p)
{
    std::vector<CompSquare> pieces;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (m_Board[i][j].player == p)
                {
                    std::vector<std::pair<int, int>> tmp = highlightPossibleMoves(p, i, j, false);
                    pieces.push_back(CompSquare{std::make_pair(i, j), tmp});
                }
            
        }
    }
    return pieces;
}


std::vector<std::vector<Square>> CheckersBoard::makeRandomMove(const int& player)
{
    // get pieces that can move
    std::vector<CompSquare> pieces = compileCompPieces(player);   // gets a list of pieces and their possible moves
    
    // iterate over the vector - if there is a possible move, add it to a dif vector

    // randomly pick a piece

    // get all of the possible moves for that piece

    // randomly pick somewhere to move
}



double Node::calculateValue() const 
{
    return (winningSimulations / (double) totalSimulations) + 
        EXPLORATION_PARAMETER * std::sqrt(
            (std::log((float) parentNode->winningSimulations) / std::log(EULER) )   // replace base 10 with e
                            / (float) parentNode->totalSimulations);
}


bool CompSquare::operator==(const CompSquare& other)
{
    return (other.coordinate == coordinate) && (other.possibleMoves == possibleMoves);
}
bool CompSquare::operator< (const CompSquare& other)
{
    return (other.coordinate == coordinate) && (other.possibleMoves.size() > possibleMoves.size());
}
bool CompSquare::operator> (const CompSquare& other)
{
    return (other.coordinate == coordinate)  && (other.possibleMoves.size() < possibleMoves.size());
}