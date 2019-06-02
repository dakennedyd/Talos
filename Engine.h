#pragma once
//#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include "Chessboard.h"
#include "Move.h"

class Engine
{
public:
	Engine();
	~Engine();
	void uci();
	void printInfo();
	void generateMoves();
	void init();
	
	
	int getRandomNumber(int min, int max)
	{
		std::uniform_int_distribution<> intDist(min, max);
		return intDist(randomEngine);
	}

	// Move stringToMove(const std::string &str)
	// {
	// 	return Move(STR_TO_SQUARE[str.substr(0, 2)], STR_TO_SQUARE[str.substr(2)]);
	// }

	//enum Player mPlayerToMove = Player::WHITE;

	/** 
	gets a bitboard an outputs a vector containing the indices of
	every bit set.
	taken from:
	https://stackoverflow.com/questions/14086854/get-an-array-of-the-bit-positions-within-a-64-bit-integer
	*/
	std::vector<uint64_t> getBitsPosition(Bitboard board);

	Chessboard & getChessBoard(){return chessboard;};
private:
	void whitePawnMoves();
	void whiteKnightMoves();
	void whiteRookMoves();
	void whiteBishopMoves();
	void whiteQueenMoves();
	void whiteKingMoves();
	void blackPawnMoves();
	void blackKnightMoves();
	void blackRookMoves();
	void blackBishopMoves();
	void blackQueenMoves();
	void blackKingMoves();
	/**
	 * @brief precalculates rays for sliding pieces for each square on the board
	 * 
	 */
	void generateRays();
	
	void debugPrint(const Bitboard n);
	Bitboard checkIfOnPassantPossibleOnNextMove(const Square a, const Square b);
	Piece getPieceFromSquare(const Square square);
	Chessboard chessboard;
	std::chrono::time_point<std::chrono::system_clock> now{ std::chrono::system_clock::now() };
	std::chrono::system_clock::duration epoch{ now.time_since_epoch() };
	typedef std::chrono::duration<unsigned int, std::milli> ms;

	std::mt19937 randomEngine{ std::chrono::duration_cast<ms>(epoch).count() };
	//std::default_random_engine re{ std::chrono::duration_cast<ms>(epoch).count() };
	std::vector<Move> mPossibleMoves;
};

