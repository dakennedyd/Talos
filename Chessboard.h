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
	void printBitboard(const Bitboard &bitboard);
	void reset();

	//changes state on the internal board does not check if it is legal or not
	void makeMove(Move move);

	void makeMove();
//private:	
	void printBitboads();
	//enum Player mPlayerToMove = Player::WHITE;
	unsigned int mPlayerToMove = Player::WHITE;

	std::vector<Bitboard> mBoard;	
	std::queue<Move> mHistory;
};

