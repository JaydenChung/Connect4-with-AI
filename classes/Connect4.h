#pragma once
#include "Game.h"
#include "Square.h"

//
// the classic game of tic tac toe
//

//
// this is the AI class
// we use a small clone here so we can recursively call minimax
//
class Connect4AI
{
public:
    int   _grid[6][7];
    bool isBoardFull() const;
    int evaluateBoard();
    int evaluatePosition(int y, int x, int player);
    int minimax(Connect4AI* state, int depth, bool isMaximizingPlayer);
    int ownerAt(int index ) const;
    int AICheckForWinner();
};

//
// the main game class
//
class Connect4 : public Game
{
public:
    Connect4();
    ~Connect4();

    // set up the board
    void        setUpBoard() override;
    void        drawFrame() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() const override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder *holder) override;
    bool        canBitMoveFrom(Bit*bit, BitHolder *src) override;
    bool        canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) override;
    void        stopGame() override;

    Connect4AI* clone();
	void        updateAI() override;
    bool        gameHasAI() override { return true; }
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;

    void        scanForMouse();
    Square      _grid[6][7];
};

