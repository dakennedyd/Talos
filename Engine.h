#pragma once
//#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include "BoardState.h"
#include "Move.h"
class Engine
{
public:
	Engine();
	~Engine();
	void whitePawnMoves();
	void blackPawnMoves();
	void whiteKnightMoves();
	void blackKnightMoves();
	void uci();
	void printInfo();
	void printBitboard(const Bitboard &bitboard);
	void generateMoves();
	
	int getRandomNumber(int min, int max)
	{
		std::uniform_int_distribution<> intDist(min, max);
		return intDist(randomEngine);
	}

	Move stringToMove(const std::string &str)
	{
		return Move(SQUARE_STR_TO_NUM[str.substr(0, 2)], SQUARE_STR_TO_NUM[str.substr(2)]);
	}

	//enum Player mPlayerToMove = Player::WHITE;

	/** 
	gets a bitboard an outputs a vector containing the indices of
	every bit set.
	taken from:
	https://stackoverflow.com/questions/14086854/get-an-array-of-the-bit-positions-within-a-64-bit-integer
	*/
	std::vector<uint64_t> getBitsPosition(Bitboard board);

	BoardState & getChessBoard(){return chessboard;};
private:
	Bitboard checkIfOnPassantPossibleOnNextMove(const Square a, const Square b);
	BoardState chessboard;
	std::chrono::time_point<std::chrono::system_clock> now{ std::chrono::system_clock::now() };
	std::chrono::system_clock::duration epoch{ now.time_since_epoch() };
	typedef std::chrono::duration<unsigned int, std::milli> ms;

	std::mt19937 randomEngine{ std::chrono::duration_cast<ms>(epoch).count() };
	//std::default_random_engine re{ std::chrono::duration_cast<ms>(epoch).count() };
	std::vector<Move> mPossibleMoves;
};

