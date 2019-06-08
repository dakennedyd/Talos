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
	//void debugPrint(const Bitboard n);
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
	 * @brief checks if any of the squares in the squares bitboard
	 * is under attack by player if it is it returns true else false
	 */
	bool checkIfSquaresAreAttackedByWhite(const Bitboard squares);
	bool checkIfSquaresAreAttackedByBlack(const Bitboard squares);

	/**
	 * @brief precalculates rays for sliding pieces for each square on the board
	 */
	void generateRays();
	
	Bitboard checkIfenPassantPossibleOnNextMove(const Square a, const Square b);
	Piece getPieceFromSquare(const Square square);
	//checks if move is a capture and set mCaptured to the captured piece
	void setMoveCapture(Move &move);
	Chessboard chessboard;
	std::chrono::time_point<std::chrono::system_clock> now{ std::chrono::system_clock::now() };
	std::chrono::system_clock::duration epoch{ now.time_since_epoch() };
	typedef std::chrono::duration<unsigned int, std::milli> ms;

	std::mt19937 randomEngine{ std::chrono::duration_cast<ms>(epoch).count() };
	//std::default_random_engine re{ std::chrono::duration_cast<ms>(epoch).count() };
	std::vector<Move> mPossibleMoves;
};

