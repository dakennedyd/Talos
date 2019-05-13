#pragma once
#include <string>
#include <queue>
#include <unordered_map>

#include "definitions.h"
#include "Move.h"


typedef uint64_t Bitboard;



static Bitboard AFILE = 0x0101010101010101;
static Bitboard BFILE = AFILE << 1;
static Bitboard CFILE = BFILE << 1;
static Bitboard DFILE = CFILE << 1;
static Bitboard EFILE = DFILE << 1;
static Bitboard FFILE = EFILE << 1;
static Bitboard GFILE = FFILE << 1;
static Bitboard HFILE = GFILE << 1;

static Bitboard RANK1 = 0xFF00000000000000;
static Bitboard RANK2 = RANK1 >> 8;
static Bitboard RANK3 = RANK2 >> 8;
static Bitboard RANK4 = RANK3 >> 8;
static Bitboard RANK5 = RANK4 >> 8;
static Bitboard RANK6 = RANK5 >> 8;
static Bitboard RANK7 = RANK6 >> 8;
static Bitboard RANK8 = RANK7 >> 8;

class BoardState
{
public:
	BoardState();
	~BoardState() = default;
	void setState(const std::string &FENstring);	
	void printBoard();
	void reset();

	//changes state on the internal board does not check if it is legal or not
	void makeMove(Move && move);
//private:	
	//enum Player mPlayerToMove = Player::WHITE;
	unsigned int mPlayerToMove = Player::WHITE;

	std::vector<Bitboard> mBoard;	
	std::queue<Move> mHistory;
};

