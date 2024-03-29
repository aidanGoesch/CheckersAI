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


CheckersBoard::CheckersBoard() : m_Selected({6, 3}), m_ToMove({-1, -1}), m_Turn(PLAYER), m_GameStates{std::unordered_map<std::string, Node*>()}
{
    unsigned offset = 1;
    for (int y = 0; y < 8; ++y)
    {
        std::vector<Square> tmp;
        for (int x = 0; x < 8; ++x)
        {
            if ( y <= 2 && offset % 2 == 0 ) { tmp.push_back(Square{COMP, false, false, false}); }
            else if ( y >= 5 && offset % 2 == 0 ) { tmp.push_back(Square{0, false, false, false}); }
            else { tmp.push_back(Square{0, false, false, false}); }

            ++offset;
        }
        ++offset;
        m_Board.push_back(tmp);
    }

    m_Board[6][3].selected = true;
    m_Board[3][6].player = PLAYER;
    // m_Board[0][5].player = 0;
    m_Board[2][7].player = 0;
    m_Board[4][5].player = 0;
    m_Board[5][4].player = 0;
    m_Board[5][6].player = 0;
    m_Board[2][3].player = 0;
    

    std::string rootKey = serializeBoard();
    m_RootNode = new Node{rootKey, nullptr};
    m_GameStates[rootKey] = m_RootNode;
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
                        if (! selectingMove && m_Board[y][x].player == PLAYER)
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

bool CheckersBoard::isChain(const unsigned& y, const unsigned& x, const unsigned& oppositePlayer)
{
    if (y > 1) 
    { // Forward
        if (x > 1 && m_Board[y-1][x-1].player == oppositePlayer && m_Board[y-2][x-2].player == 0)
            return true;
        if (x < 5 && m_Board[y-1][x+1].player == oppositePlayer && m_Board[y-2][x+2].player == 0)
            return true;
    }

    std::cout << "kinged = " << m_Board[y][x].kinged << std::endl;
    if (m_Board[y][x].kinged && y < 6)
    // std::cout << 1;
    {  // Backward
        std::cout << "BBBBBBB" << std::endl;
        if (x > 1 && m_Board[y+1][x-1].player == oppositePlayer && m_Board[y+2][x-2].player == 0)
            return true;
        if (x < 5 && m_Board[y+1][x+1].player == oppositePlayer && m_Board[y+2][x+2].player == 0)
            return true;
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

    if (ny == 0)
        m_Board[ny][nx].kinged = true;

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
    if (chainStarted && isChain(m_Selected.first, m_Selected.second, COMP))
    {
        // std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
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


int CheckersBoard::winner()
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
        return PLAYER;
    
    if (countOne > 0 && countTwo == 0)   // COMP wins
        return COMP;


    // check for not being able to make any other moves
    std::vector<CompSquare> playerPieces;
    std::vector<CompSquare> compPieces;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (m_Board[i][j].player == COMP)
            {
                std::vector<std::pair<int, int>> tmp = highlightPossibleMoves(COMP, i, j, false);
                compPieces.push_back(CompSquare{std::make_pair(i, j), tmp});
            }
            else if (m_Board[i][j].player == PLAYER)
            {
                std::vector<std::pair<int, int>> tmp = highlightPossibleMoves(PLAYER, i, j, false);
                playerPieces.push_back(CompSquare{std::make_pair(i, j), tmp});
            }
        }
    }

    auto func = [](const CompSquare& c) { return c.possibleMoves.size() == 0; };

    if (!std::any_of(playerPieces.begin(), playerPieces.end(), func))
    {
        return COMP;
    }

    if (!std::any_of(compPieces.begin(), compPieces.end(), func))
    {
        return PLAYER;
    }

    return 0;                            // game not over
}


void CheckersBoard::checkKings()
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_Board[0][i].player == PLAYER)
        {
            m_Board[0][i].kinged = true;
            // std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA:    " << i << std::endl;
        }

        if (m_Board[7][i].player == COMP)
            m_Board[7][i].kinged = true;
    }
}


void CheckersBoard::Move()
{
    if (m_Turn == PLAYER)
    {
        GetPlayerMove(false);

        std::string newKey = serializeBoard();

        if (m_GameStates.find(newKey) != m_GameStates.end())
        {
            // TODO: prune impossible branches
            m_GameStates[newKey]->m_ParentNode = nullptr;
            m_RootNode = m_GameStates[newKey];  // update the root node so that the current game state is now the root
        }
        else 
        {
            std::cout << "ERROR" << std::endl;
        }

        m_Turn = COMP;
    }
    else if (m_Turn == COMP)
    {
        
        for (int i = 0; i < 25 ; ++i)   // simulate 25 random games (probable can and should be more)
        {
            simulateRandomGame();
        }

        makeBestCompMove();

        m_Turn = PLAYER;
    }
}

void CheckersBoard::makeBestCompMove()
{
    ;
}


void CheckersBoard::Play()
{

    ClearConsole();
    Move();
    
    std::cout << m_Turn << std::endl;

    // while (winner() == 0) 
    // {
    //     // std::cout << 1;w
    //     Move();
    // }
    

    Draw();
    std::string s = serializeBoard();
    auto t = (winner() == PLAYER) ? "PLAYER" : "COMP";
    std::cout << "winner: " << t << std::endl;
}



Node::Node(const std::string& key, Node* p) 
: m_TotalSimulations(0), m_WinningSimulations(0), m_ParentNode(p), m_ChildNodes(std::vector<Node*>{}), m_Key(key) 
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


std::pair<CompSquare, std::pair<int, int>> CheckersBoard::makeRandomMove(const int& player)
{
    // get pieces that can move
    std::vector<CompSquare> pieces = compileCompPieces(player);   // gets a list of pieces and their possible moves
    std::vector<CompSquare> moveablePieces;

    // filter out all of the pieces that can't be moved
    std::copy_if (pieces.begin(), pieces.end(), std::back_inserter(moveablePieces), [](CompSquare i){return i.possibleMoves.size()>0;} );

    // randomly pick a piece
    size_t bound = moveablePieces.size();

    std::srand(std::time(0));  
    CompSquare pieceToMove = moveablePieces[std::rand() % bound];

    // randomly pick somewhere to move
    bound = pieceToMove.possibleMoves.size();
    std::pair<int, int> move = pieceToMove.possibleMoves[std::rand() % bound];


    // return a pair with the piece and the move -- change return type
}


std::string CheckersBoard::serializeBoard() const
{
    /* function that gets the keys for the map that keeps track of what states
     have been visited before */
    std::vector<Square> boardList;

    bool toggle = true;

    for ( std::vector<Square> row : m_Board )   // put the 2d board into one very long list
    {
        boardList.insert( boardList.end(), row.begin(), row.end() );

        if (toggle) 
            boardList.push_back(Square{0, false, false, false});
        else
            boardList.pop_back();

        toggle = !toggle;
    }

    std::string boardString = "";
    for (int i = 1 ; i < boardList.size() ; i += 2)   // get every other element of the list
    {
        boardString += std::to_string(boardList[i].player);
    }
    return boardString;
}


void CheckersBoard::simulateRandomGame()
{
    CheckersBoard tmp = *this;  // copy the board so that you can reset after the simulation
    int currentPlayer = m_Turn;
    Node* prevNode = nullptr;
    Node* currentNode = m_RootNode;

    while (winner() == 0)
    {
        prevNode = currentNode;
        makeRandomMove(currentPlayer);

        currentPlayer = (currentPlayer == PLAYER) ? COMP : PLAYER;   // flip the current player

        std::string tmpKey = serializeBoard();

        if (m_GameStates.find(tmpKey) != m_GameStates.end())  // Check to see if the node exists
        {
            currentNode = m_GameStates[tmpKey];
        }
        else
        {
            currentNode = new Node{tmpKey, nullptr};  // change this so that it points at the parent
            m_GameStates[tmpKey] = currentNode;      // add to dictionary
        }

        currentNode->m_TotalSimulations ++;  // update visit count
        currentNode->m_ChildNodes.push_back(prevNode);
    }

    int simWinner = winner();

    // back propogate
    for ( ; currentNode != nullptr ; currentNode = currentNode->m_ParentNode)
    {
        if (currentNode->m_Turn == simWinner)
        {
            currentNode->m_WinningSimulations ++;
        }
    }

    // restore to previous game state
    *this = tmp;
}



double Node::calculateValue() const 
{
    return (m_WinningSimulations / (double) m_TotalSimulations) + 
        EXPLORATION_PARAMETER * std::sqrt(
            (std::log((float) m_ParentNode->m_WinningSimulations) / std::log(EULER) )   // replace base 10 with e
                            / (float) m_ParentNode->m_TotalSimulations);
}

// simulate game
// keep track of root node and tree with pointers, keep track with unorderedmap to update stats
// update root node
// while there is no winner and moves are able to be made
//      Randomly pick a move
//      make the move
//      update stats for new node in unordered_map?
//      \_/(._.)\_/

// update the root node of the tree whenever the player moves
