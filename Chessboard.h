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
	void printBitboard(const Bitboard bitboard);
	void printBitboards();
	enum Player mPlayerToMove = Player::WHITE;
	//unsigned int mPlayerToMove = Player::WHITE;

	std::vector<Bitboard> mBoard;	
	std::vector<Move> mHistory;
};

