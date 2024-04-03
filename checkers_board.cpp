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
    m_Board[0][5].player = 0;
    m_Board[2][7].player = 0;
    m_Board[4][5].player = 0;
    m_Board[5][4].player = 0;
    m_Board[5][6].player = 0;
    m_Board[2][3].player = 0;
    

    std::string rootKey = serializeBoard();
    m_RootNode = new Node{rootKey, nullptr, false};
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


std::vector<std::pair<int, int>> CheckersBoard::highlightPossibleMoves(const int& piece, const unsigned& y, const unsigned& x, const bool& modify)
{
    std::vector<std::pair<int, int>> ret;
    // highlight possible moves for that square

    bool forceMove = false;  // forces players to take a piece if they can
    int opp = (piece == PLAYER) ? 1 : 2;

    // capture moves
    if ((m_Board[y][x].kinged && piece == COMP) | piece == PLAYER)
    {
        if (y > 1)  // highlights up moves - capture
        {
            if (x > 1 && m_Board[y-1][x-1].player == opp && m_Board[y-2][x-2].player == 0)
            {
                if (modify) m_Board[y-2][x-2].highlighted = true;

                forceMove = true;
                ret.push_back({y-2, x-2});
            }

            if (x < 7 && m_Board[y-1][x+1].player == opp && m_Board[y-2][x+2].player == 0)
            {
                if (modify) m_Board[y-2][x+2].highlighted = true;

                forceMove = true;
                ret.push_back({y-2, x+2});
            }
        }
    }

    if ((m_Board[y][x].kinged && piece == PLAYER) | piece == COMP)  
    {
        if (m_Board[y][x].kinged && y < 6) // highlights down moves - capture
        {
            if (x > 1 && m_Board[y+1][x-1].player == opp && m_Board[y+2][x-2].player == 0)
            {
                if (modify) m_Board[y+2][x-2].highlighted = true;

                forceMove = true;
                ret.push_back({y+2, x-2});
            }

            if (x < 7 && m_Board[y+1][x+1].player == opp && m_Board[y+2][x+2].player == 0)
            {
                if (modify) m_Board[y+2][x+2].highlighted = true;

                forceMove = true;
                ret.push_back({y+2, x+2});
            }
        }
    }

    if (forceMove) return ret;  // returns if the move is forced
    // non-capture moves
    if ((m_Board[y][x].kinged && piece == COMP) | piece == PLAYER)
    {
        if (y > 0)  // highlights up moves - non capture
        {
            // Forwards
            if (x > 1 && m_Board[y-1][x-1].player == 0)
            {
                if (modify) m_Board[y-1][x-1].highlighted = true;
                ret.push_back({y-1, x-1});
            }

            if (x < 7 && m_Board[y-1][x+1].player == 0)
            {
                if (modify) m_Board[y-1][x+1].highlighted = true;
                ret.push_back({y-1, x+1});
            }
        }
    }
    

    if ((m_Board[y][x].kinged && piece == PLAYER) | piece == COMP)  
    {
        if (y < 7)  // highlights down moves - non capture
        {
            // Backwards
            if (x > 1 && m_Board[y+1][x-1].player == 0)
            {
                if (modify) m_Board[y+1][x-1].highlighted = true;
                ret.push_back({y+1, x-1});
            }

            if (x < 7 && m_Board[y+1][x+1].player == 0)
            {
                if (modify) m_Board[y+1][x+1].highlighted = true;
                ret.push_back({y+1, x+1});
            }
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

        highlightPossibleMoves(PLAYER, y, x, true);

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
            if (m_Board[i][j].player == COMP)
                ++countOne;
            
            if (m_Board[i][j].player == PLAYER)
                ++countTwo;
        }
    }
    
    // std::cout << countOne << " " << countTwo;
    if (countOne == 0 && countTwo > 0)   // player wins
    {   
        std::cout << 1 << std::endl;
        return PLAYER;
    }
    
    if (countOne > 0 && countTwo == 0)   // COMP wins
    {
        std::cout << 2 << std::endl;
        return COMP;
    }

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

                // std::cout << "comp moves: " << std::endl;
                // for (auto e : tmp) {
                //     std::cout << e.first << " " << e.second << std::endl;
                // }

                compPieces.push_back(CompSquare{std::make_pair(i, j), tmp});
            }
            else if (m_Board[i][j].player == PLAYER)
            {
                std::vector<std::pair<int, int>> tmp = highlightPossibleMoves(PLAYER, i, j, false);

                playerPieces.push_back(CompSquare{std::make_pair(i, j), tmp});
            }
        }
    }

    auto func = [](const CompSquare& c) { return c.possibleMoves.size() > 0; };

    if (!std::any_of(playerPieces.begin(), playerPieces.end(), func))
    {
        std::cout << 3 << std::endl;
        return COMP;
    }

    if (!std::any_of(compPieces.begin(), compPieces.end(), func))
    {   
        std::cout << 4 << std::endl;
        return PLAYER;
    }

    return 0;                            // game not over
}


bool CheckersBoard::checkKings()
{
    bool kinged = false;
    for (int i = 0; i < 8; ++i)
    {
        if (m_Board[0][i].player == PLAYER)
        {
            if (!m_Board[0][i].kinged) kinged = true;

            m_Board[0][i].kinged = true;
        }

        if (m_Board[7][i].player == COMP)
        {
            if (!m_Board[7][i].kinged) kinged = true;

            m_Board[7][i].kinged = true;
        }
    }
    return kinged;
}


void CheckersBoard::Move()
{
    if (m_Turn == PLAYER)
    {
        GetPlayerMove(false);

        // std::string newKey = serializeBoard();

        // if (m_GameStates.find(newKey) != m_GameStates.end())
        // {
        //     // TODO: prune impossible branches
        //     m_GameStates[newKey]->m_ParentNode = nullptr;
        //     m_RootNode = m_GameStates[newKey];  // update the root node so that the current game state is now the root
        // }
        // else 
        // {
        //     std::cout << "ERROR" << std::endl;
        // }

        m_Turn = COMP;
    }
    else if (m_Turn == COMP)
    {
        for (int i = 0; i < 40 ; ++i)   // simulate 25 random games (probable can and should be more)
        {
            simulateRandomGame();
        }

        makeBestCompMove();

        m_Turn = PLAYER;
    }

    updateRootNode();
}


void CheckersBoard::updateRootNode()
{
    std::string tmpKey = serializeBoard();

    if (m_GameStates.find(tmpKey) != m_GameStates.end())  // Check to see if the node exists
    {
        m_RootNode = m_GameStates[tmpKey];
        m_RootNode->m_ParentNode = nullptr;
    }
    else
    {
        m_RootNode = new Node{tmpKey, nullptr, false};  // change this so that it points at the parent
        m_GameStates[tmpKey] = m_RootNode;      // add to dictionary
    }
}


void CheckersBoard::makeBestCompMove()
{
    std::cout << "number of children: " << m_RootNode->m_ChildNodes.size() << std::endl;
    std::cout << "Tree Size: " << m_GameStates.size() << std::endl;
    for (Node* childNode : m_RootNode->m_ChildNodes)
    {
        if (childNode != nullptr)
        {
            childNode->calculateValue();

            std::cout << "key: " << childNode->m_Key << std::endl;
        }
    }
    

    std::vector<Node*> children = m_RootNode->m_ChildNodes;
    Node* bestNode = *std::max_element(children.begin(), children.end(), [](Node* a, Node* b){ return a->m_Score < b->m_Score; });
    std::cout << "best move: " << bestNode->m_Key << std::endl;

    // de-serialize
    std::vector<std::vector<int>> optimalBoard = deserializeBoard(bestNode->m_Key);

    // iterate over the deserialized board and the actual board
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            int newVal = optimalBoard[y][x];
            int oldVal = m_Board[y][x].player;

            // update the kinged status of the pieces
            if (oldVal == 0 && newVal == COMP)  // updates king value for the piece moved
            {
                m_Board[y][x].kinged = bestNode->m_KingedPiece;
            } else if (oldVal == PLAYER && newVal == 0) // player piece gets taken
            {
                m_Board[y][x].kinged = false;
            }
            m_Board[y][x].player = newVal; // update the value
        }
    }
}


void CheckersBoard::Play()
{

    ClearConsole();
    // Move();
    
    // std::cout << m_Turn << std::endl;

    // while (winner() == 0) 
    // {
    //     // std::cout << 1;w
    //     Move();
    // }
    

    // Draw();
    // Move();
    // Draw();
    // Move();
    // Draw();
    // Move();
    // Draw();

    Draw();
    while (winner() == 0)
    {
        Move();
        Draw();
    }
    Draw();

    // auto tmp = serializeBoard();
    // auto res = deserializeBoard(tmp);

    // for (auto row : res)
    // {
    //     for (auto e : row)
    //     {
    //         std::cout << e << ' ';
    //     }
    //     std::cout << std::endl;
    // }
}



Node::Node(const std::string& key, Node* p, const bool& kinged) 
: m_TotalSimulations(0), m_WinningSimulations(0), m_ParentNode(p), m_ChildNodes(std::vector<Node*>{}), m_Key(key), m_Score(-1.0), m_KingedPiece(kinged) 
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


bool CheckersBoard::makeRandomMove(const int& player, const int& i = -1)
{
    // get pieces that can move
    std::vector<CompSquare> pieces = compileCompPieces(player);   // gets a list of pieces and their possible moves
    std::vector<CompSquare> moveablePieces;
    size_t bound;
    CompSquare pieceToMove;

    if (DEBUG) {
        std::cout << "piece's possible moves: " << std::endl;
        for (auto e : pieces)
        {
            std::cout << "piece" << std::endl;
            for (auto f : e.possibleMoves)
            {
                std::cout << "\t" << f.first << " " << f.second << std::endl;
            }
        }
    }

    if (i == -1)
    {
        // filter out all of the pieces that can't be moved
        std::copy_if (pieces.begin(), pieces.end(), std::back_inserter(moveablePieces), [](CompSquare i){return i.possibleMoves.size()>0;} );

        // randomly pick a piece
        bound = moveablePieces.size();
        
        std::srand(std::time(0));  
        pieceToMove = moveablePieces[std::rand() % bound];
    }
    else
    {
        pieceToMove = moveablePieces[i];
    }

    // randomly pick somewhere to move
    bound = pieceToMove.possibleMoves.size();
    std::pair<int, int> move = pieceToMove.possibleMoves[std::rand() % bound];

    int y = pieceToMove.coordinate.first;
    int x = pieceToMove.coordinate.second;

    int ny = move.first;
    int nx = move.second;

    m_Board[y][x].player = 0;
    m_Board[ny][nx].player = player;

    // transfer king status
    m_Board[ny][nx].kinged = m_Board[y][x].kinged;
    m_Board[y][x].kinged = false;
    
    bool chainStarted = false;
    if (abs(nx - x) > 1)  // check if it is taking another piece
    {
        chainStarted = true;

        int z = ((y - ny) / 2) + ny;
        int w = ((x - nx) / 2) + nx;

        m_Board[z][w].player = 0;

        // check for moves where you take another piece, otherwise stop
        std::vector<std::pair<int, int>> possibleMovesFromNewPos = highlightPossibleMoves(COMP, ny, nx, false);
        if (possibleMovesFromNewPos.size() > 0)
        {
            if (abs(possibleMovesFromNewPos[0].second - nx) > 1)
                makeRandomMove(COMP, z + w);  // should make it so that it will only move the piece that is chaining
        }
    }

    if (m_Turn == PLAYER) m_Turn = COMP;
    else m_Turn = PLAYER;

    // return a pair with the piece and the move -- change return type
    return chainStarted;   
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
    boardString += std::to_string(m_Turn);
    return boardString;
}

std::vector<std::vector<int>> CheckersBoard::deserializeBoard(const std::string& serial) const
{
    std::vector<std::vector<int>> ret;
    int cursor = 0;
    bool s = true;
    for (int i = 0; i < 8; ++i)
    {
        std::vector<int> row;

        if (s) row.push_back(0);// push/pop back every other row
        for (int j = 0; j < 4; ++j, ++cursor)  
        {
            row.push_back(int(serial[cursor]) - 48);
            row.push_back(0);
        }

        if (s) row.pop_back();

        ret.push_back(row);
        s = !s;  // alternate the push/pop operation every other row
    }

    return ret;
}


void CheckersBoard::simulateRandomGame()
{
    // CheckersBoard tmp = *this;  // copy the board so that you can reset after the simulation
    // things that need to be reset after every game:
    const int tmpTurn = m_Turn;
    // Node* tmpRoot = m_RootNode;   maybe not necessary?
    // const std::pair<int, int> tmpToMove = m_S;
    const std::vector<std::vector<Square>> tmpBoard = m_Board;   // this might need to be a copy
        // turn
        // rootnode
        // selected
        // board


    int currentPlayer = m_Turn;
    Node* prevNode = nullptr;
    Node* currentNode = m_RootNode;
    bool chaining = false;

    while (true)
    {
        // std::cout << "AAAAAAAAAAAAAAA" << currentPlayer << std::endl;
        prevNode = currentNode;
        chaining = makeRandomMove(currentPlayer);

        bool requiredKing = checkKings();

        if (chaining) continue; // makes it so that if you are forced to chain takes then the parent child relationship between the nodes stays intact

        if (!chaining)
            currentPlayer = (currentPlayer == PLAYER) ? COMP : PLAYER;   // flip the current player

        std::string tmpKey = serializeBoard();

        if (DEBUG) {
            std::cout << "currentBoard (turn = " << currentPlayer << ")"  << std::endl;
            for (auto r : m_Board)
            {
                for (auto e : r)
                    std::cout << e.player;
                
                std::cout << std::endl;
            }
        }

        auto cursor = std::find_if(prevNode->m_ChildNodes.begin(), prevNode->m_ChildNodes.end(), [&tmpKey](Node* n){ return n->m_Key == tmpKey; });

        if (cursor == prevNode->m_ChildNodes.end())  // Check to see if the node exists in the children of the 
        // {
        //     currentNode = m_GameStates[tmpKey];
        // }
        // else
        {
            currentNode = new Node{tmpKey, prevNode, false};  // change this so that it points at the parent
            m_GameStates[tmpKey] = currentNode;      // add to dictionary
            prevNode->m_ChildNodes.push_back(currentNode);
        }
        else
        {
            currentNode = *cursor;
        }

        if (winner() != 0)
            break;
    }

    int simWinner = winner();

    if (DEBUG) std::cout << "winner: " << simWinner << "  board: " << currentNode->m_Key << std::endl;
    std::cout << "number of nodes explored: "  << m_GameStates.size() << std::endl;

    // back propogate
    for ( ; currentNode != nullptr ; currentNode = currentNode->m_ParentNode)
    {
        // std::cout << "bad" << std::endl;
        if (currentNode->m_Turn != simWinner)
        {
            currentNode->m_WinningSimulations ++;
        }
        currentNode->m_TotalSimulations ++;  // update visit count
    }

    // restore to previous game state
    m_Board = tmpBoard;
    m_Turn = tmpTurn;
    
    if (DEBUG) std::cout << "Reset test: " << serializeBoard() << std::endl;
}



double Node::calculateValue() 
{
    /* calculates the score for node that it is called on */

    float tmp;
    if (m_ParentNode->m_WinningSimulations == 0)
        tmp = -1000000000.0;
    else
        tmp = std::log((float) m_ParentNode->m_WinningSimulations);


    std::cerr << "MATH:" << std::endl;
    std::cerr << m_WinningSimulations << " / " << m_TotalSimulations << " child key:  " << m_Key << std::endl;
    std::cerr << m_ParentNode->m_WinningSimulations << " / " << m_ParentNode->m_TotalSimulations << " parent key: " << m_ParentNode->m_Key << std::endl;

    // std::cout << (m_WinningSimulations / (double) m_TotalSimulations) << std::endl;
    // std::cout << EXPLORATION_PARAMETER * std::sqrt(
    //         (tmp / std::log(EULER) )   // replace base 10 with e
    //                         / (float) m_ParentNode->m_TotalSimulations) << std::endl;

    m_Score = (m_WinningSimulations / (double) m_TotalSimulations) + 
        EXPLORATION_PARAMETER * std::sqrt(
            (tmp / std::log(EULER) )   // replace base 10 with e
                            / (float) m_ParentNode->m_TotalSimulations);

    return m_Score;
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
