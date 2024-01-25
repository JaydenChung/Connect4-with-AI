// Jayden Chung
// CMPM123
// #1893540
//I focused on creating the minimax function. The A B pruning was messing up the score my AI was keeping
//track of so I had to remove it and my updateAI was being called multiple times due to errors in clone
//Tried implimenting a different version of score when minimax wasn't working... 3 in a row 100 score player -100, 2 in a row 10 score, etc...
//Bahar Boateni helped me debug my code through printing out the board and stayed after section to help me understand great TA!!!

#include <algorithm>
#include <vector>
#include <random>
#include<iostream>
#include "Connect4.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = 0;

Connect4::Connect4()
{
    
}

Connect4::~Connect4()
{
}

//
// make an X or an O
//
Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == 1 ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].initHolder(ImVec2(100*x + 100, 100*y + 100), "square.png", x, y);
        }
    }
    
    startGame();
    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}


//
// scan for mouse is temporarily in the actual game class
// this will be moved to a higher up class when the squares have a heirarchy
// we want the event loop to be elsewhere and calling this class, not called by this class
// but this is fine for tic-tac-toe
//
void Connect4::scanForMouse()
{
    if (gameHasAI() && getCurrentPlayer()->isAIPlayer()) 
    {
        updateAI();
        return;
    }
    ImVec2 mousePos = ImGui::GetMousePos();
    mousePos.x -= ImGui::GetWindowPos().x;
    mousePos.y -= ImGui::GetWindowPos().y;

    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            if (_grid[y][x].isMouseOver(mousePos)) {
                if (ImGui::IsMouseClicked(0)) {
                    if (actionForEmptyHolder(&_grid[y][x])) {
                        endTurn();
                    }
                } else {
                    _grid[y][x].setHighlighted(true);
                }
            } else {
                _grid[y][x].setHighlighted(false);
            }
        }
    }    
}

//
// draw the board and then the pieces
// this will also go somewhere else when the heirarchy is set up
//
void Connect4::drawFrame() 
{
    scanForMouse();
    //scanForInput 

    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].paintSprite();
            if (_grid[y][x].bit()) {
                _grid[y][x].bit()->paintSprite();
            }
        }
    }
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Connect4::actionForEmptyHolder(BitHolder *holder)
{
    if (holder->bit()) {
        return false;  // The holder contains a bit
    }

    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());

    int column = int((holder->getPosition().x - _grid[0][0].getPosition().x) / 100.0);  // Calculate the column based on position.

    for (int row = 5; row >= 0; row--) {
        BitHolder *targetHolder = &_grid[row][column];

        if (!targetHolder->bit()) {
            //Get the position for the bit
            ImVec2 position = targetHolder->getPosition();

            //set position of bit
            bit->setPosition(position);

            targetHolder->setBit(bit);

            return true;  
        }
    }

    return false;  
}

bool Connect4::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool Connect4::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void Connect4::stopGame()
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* Connect4::ownerAt(int index ) const
{
    int row = index / 7;  
    int col = index % 7;  

    if (row >= 0 && row < 6 && col >= 0 && col < 7 && _grid[row][col].bit()) {
        return _grid[row][col].bit()->getOwner();
    }

    return nullptr;
}

Player* Connect4::checkForWinner()
{
    //horizontal win
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            Player *p = ownerAt(row * 7 + col);
            if (p != nullptr && p == ownerAt(row * 7 + col + 1) &&
                p == ownerAt(row * 7 + col + 2) && p == ownerAt(row * 7 + col + 3)) {
                return p;
            }
        }
    }

    //vertical win
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            Player *p = ownerAt(row * 7 + col);
            if (p != nullptr && p == ownerAt((row + 1) * 7 + col) &&
                p == ownerAt((row + 2) * 7 + col) && p == ownerAt((row + 3) * 7 + col)) {
                return p;
            }
        }
    }

    //diagonal win
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            Player *p = ownerAt(row * 7 + col);
            if (p != nullptr && p == ownerAt((row + 1) * 7 + col + 1) &&
                p == ownerAt((row + 2) * 7 + col + 2) && p == ownerAt((row + 3) * 7 + col + 3)) {
                return p;
            }
        }
    }
    for (int row = 0; row < 3; row++) {
        for (int col = 3; col < 7; col++) {
            Player *p = ownerAt(row * 7 + col);
            if (p != nullptr && p == ownerAt((row + 1) * 7 + col - 1) &&
                p == ownerAt((row + 2) * 7 + col - 2) && p == ownerAt((row + 3) * 7 + col - 3)) {
                return p;
            }
        }
    }

    return nullptr;
}

bool Connect4::checkForDraw()
{
    // check to see if the board is full
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            if (!_grid[y][x].bit()) {
                return false;
            }
        }
    }
    return true;
}

//
// state strings
//
std::string Connect4::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Connect4::stateString() const
{
    std::string s;
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 7; x++) {
            Bit *bit = _grid[y][x].bit();
            if (bit) {
                // Add 1 to the player number to convert from 0-based to 1-based indexing
                s += std::to_string(bit->getOwner()->playerNumber() + 1);
            } else {
                s += "0";
            }
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Connect4::setStateString(const std::string &s)
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            int index = y*7 + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit( PieceForPlayer(playerNumber-1) );
            } else {
                _grid[y][x].setBit( nullptr );
            }
        }
    }
}


//
// this is the function that will be called by the AI
//
void Connect4::updateAI() 
{
    int bestVal = 0;
    Square* bestMove = nullptr;

    Connect4AI* newState = this->clone();
    std::vector<int> columns = {3, 2, 4, 1, 5, 0, 6};   //priority order

    
    // std::cout << "Initial" << std::endl;
    // for (int y = -1; y < 6; y++) {
    //     std::cout << (y>0?y:0) << " - ";
    //     for (int x = 0; x < 7; x++) {
    //         if (y == -1)
    //             std::cout << x << " ";
    //         else
    //             std::cout << newState->_grid[y][x] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // Traverse all cells, evaluate minimax function for all empty cells
    for (int i = 0; i < 7; i++) {
        int x = columns[i];
        int y;
        for (y = 5; y >= 0; y--) { // Start from the bottom row
            // Check if cell is empty
            if (!_grid[y][x].bit()) {
                break;
            }
        }
        // Make the move
        _grid[y][x].setBit(PieceForPlayer(AI_PLAYER));
        Connect4AI* newState = this->clone();
        // std::cout << " - " << y << " " << x << std::endl;
        // for (int j = -1; j < 6; j++) {
        //     std::cout << (j>0?j:0) << " - ";
        //     for (int i = 0; i < 7; i++) {
        //         if (j == -1)
        //             std::cout << i << " ";
        //         else
        //             std::cout << newState->_grid[j][i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        int moveVal = newState->minimax(newState, 0, false); // It's the minimizing player's turn next
        delete newState; // Don't forget to delete the cloned state!
        // Undo the move
        _grid[y][x].setBit(nullptr);

        // If the value of the current move is more than the best value, update best
        if (moveVal > bestVal || !bestMove) {
            bestMove = &_grid[y][x];
            bestVal = moveVal;
        }
    }

    // Make the best move
    if(bestMove) {
        if (actionForEmptyHolder(bestMove)) {
            endTurn();
        }
    }
}

//
// AI class
// this is a small class that just has a bunch of ints in it to allow us to recursively call minimax
//

Connect4AI* Connect4::clone() 
{
    Connect4AI* newGame = new Connect4AI();
    std::string gamestate = stateString();
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            int index = y*7 + x;
            int playerNumber = gamestate[index] - '0';
            newGame->_grid[y][x] = playerNumber;
        }
    }
    return newGame;
}

//
// helper function for the winner check
//
int Connect4AI::ownerAt(int index ) const
{
    return _grid[index/7][index%7];}

int Connect4AI::AICheckForWinner()
{
    //horizontal win
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            int playerInt = ownerAt(row * 7 + col);
            if( playerInt != 0 && playerInt == ownerAt(row * 7 + col + 1) &&
                playerInt == ownerAt(row * 7 + col + 2) && playerInt == ownerAt(row * 7 + col + 3)) {
                return playerInt;
            }
        }
    }

    //vertical win
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            int playerInt = ownerAt(row * 7 + col);
            if( playerInt != 0 && playerInt == ownerAt((row + 1) * 7 + col) &&
                playerInt == ownerAt((row + 2) * 7 + col) && playerInt == ownerAt((row + 3) * 7 + col)) {
                return playerInt;
            }
        }
    }

    //diagonal win
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int playerInt = ownerAt(row * 7 + col);
            if( playerInt != 0 && playerInt == ownerAt((row + 1) * 7 + col + 1) &&
                playerInt == ownerAt((row + 2) * 7 + col + 2) && playerInt == ownerAt((row + 3) * 7 + col + 3)) {
                return playerInt;
            }
        }
    }
    for (int row = 0; row < 3; row++) {
        for (int col = 3; col < 7; col++) {
            int playerInt = ownerAt(row * 7 + col);
            if( playerInt != 0 && playerInt == ownerAt((row + 1) * 7 + col -1) &&
                playerInt == ownerAt((row +2 ) *7+col-2)&&playerInt==ownerAt((row+3)*7+col-3)){
                return playerInt;
            }
        }
    }

    return -1;
}
//
// Returns: positive value if AI wins, negative if human player wins, 0 for draw or undecided
//
int Connect4AI::evaluateBoard() 
{
    int score = 0;
    int winner = AICheckForWinner();
    if (winner == -1){
        return 0;   //no winner yet
    }
    // Check for winner
    if(winner == (AI_PLAYER+1)) 
    {
        return 10000; // AI wins, high positive score
    }
    else {
        return -10000; 
    }
    //std::cout << " should not reach this!" << std::endl;
    
    // for(int y = 0; y < 6; y++) {
    //     for(int x = 0; x < 7; x++) {
    //         if(_grid[y][x] == (AI_PLAYER+1)) {
    //             score += evaluatePosition(y, x, AI_PLAYER);
    //         } else if(_grid[y][x] == (HUMAN_PLAYER+1)) {
    //             score -= evaluatePosition(y, x, HUMAN_PLAYER);
    //         }
    //     }
    // }

    return score;
}


//
// helper function for a draw
//
bool Connect4AI::isBoardFull() const
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            if (!_grid[y][x]) {
                return false;
            }
        }
    }
    return true;    
}

//
// player is the current player's number (AI or human)
//
const int MAX_DEPTH = 6; 
int alpha = -1000;
int beta = 1000; 

int Connect4AI::minimax(Connect4AI* state, int depth, bool isMaximizingPlayer) 
{
    // std::cout << depth << std::endl;
    // for (int y = 0; y < 6; y++) {
    //         for (int x = 0; x < 7; x++) {
    //             std::cout << state->_grid[y][x] << " ";
    //         }
    //     std::cout << std::endl;
    // }
    
    int score = state->evaluateBoard();

    //std::cout << score << std::endl;;

    // If AI wins, human wins, or draw
    if(score >= 10) return score - depth;
    if(score <= -10) return score + depth;

    if(state->isBoardFull() || depth == MAX_DEPTH) return 0; // Draw or depth limit reached

    if (isMaximizingPlayer) {
        int bestVal = -1000; // Min value
        for (int x = 0; x < 7; x++) {
            int y;
            for (y = 5; y >= 0; y--) {
                // Check if cell is empty
                if (!state->_grid[y][x]) {
                    break;
                }
            }
           // std::cout << y << " " << x << std::endl;
            // Make the move
            state->_grid[y][x] = (AI_PLAYER + 1);           // because 0 is no player, 1 is human, 2 is AI
            bestVal = std::max(bestVal, minimax(state, depth+1, !isMaximizingPlayer));
            // Undo the move for backtracking
            state->_grid[y][x] = 0;

            alpha = std::max(alpha, bestVal);
            // if(beta <= alpha)
            //     break;
        }
        //std::cout << bestVal << std::endl;
        return bestVal;
    } else {
        int bestVal = 1000; // Max value
        for (int x = 0; x < 7; x++) {
            int y;
            for (y = 5; y >= 0; y--) {
                // Check if cell is empty
                if (!state->_grid[y][x]) {
                    break;
                }
            }
            //std::cout << y << " " << x << std::endl;
            // Make the move
            state->_grid[y][x] = (HUMAN_PLAYER + 1);
            bestVal = std::min(bestVal, minimax(state, depth+1, !isMaximizingPlayer));
            // Undo the move for backtracking
            state->_grid[y][x] = 0;

            beta = std::min(beta, bestVal);
            // if(beta <= alpha)
            //     break;
        }
        //std::cout << bestVal << std::endl;
        return bestVal;
    }
}
