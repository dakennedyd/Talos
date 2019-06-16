#pragma once
#include <string>
#include <queue>
#include <unordered_map>

#include "definitions.h"
#include "Move.h"

class Chessboard
{
public:
	Chessboard();
	~Chessboard() = default;
	void setState(const std::string &FENstring);	
	void printBoard();
	void reset();

	//changes state on the internal board does not check if it is legal or not
	void makeMove(const Move &move);
	void unmakeMove();

//private:		
	int64_t getPositionValue();
	void printBitboard(const Bitboard bitboard);
	void printBitboards();
	/**
	 * @brief checks if any of the squares in the squares bitboard
	 * are under attack by player if it is it returns true else false
	 */
	bool checkIfSquaresAreAttackedByWhite(const Bitboard squares);
	bool checkIfSquaresAreAttackedByBlack(const Bitboard squares);
	
	enum Player mPlayerToMove = Player::WHITE;
	
	//checks if move is a capture and set mCaptured to the captured piece
	void setMoveCapture(Move &move);
	void setIfEnPassantCapture(Move &move);

	std::vector<Bitboard> mBoard;	
	std::vector<Move> mHistory;	
	int mCastlingAvailable[5] = {0,1,1,1,1};//garbage kingSideW kingSideB QueenSideW QueenSideB
	int mKingNotMoved[3] = {0,1,1};//garbage, wking, bking - 0 means it moved
	bool mCanCastle = true;
};

