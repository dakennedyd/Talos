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
	void generateMoves();
	// int getRandomNumber(int min, int max)
	// {
	// 	std::uniform_int_distribution<> intDist(min, max);
	// 	return intDist(randomEngine);
	// }

	//private:
	/**
	 * @brief precalculates rays for sliding pieces for each square on the board
	 */
	void generateRays();
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
	 * are under attack by player if it is it returns true else false
	 */
	bool checkIfSquaresAreAttackedByWhite(const Bitboard squares);
	bool checkIfSquaresAreAttackedByBlack(const Bitboard squares);
	Bitboard checkIfenPassantPossibleOnNextMove(const Square a, const Square b);
	Piece getPieceFromSquare(const Square square);

	void evaluation();
	void printBitboard(const Bitboard bitboard);
	void printBitboards();

	//determines if move is a capture and set mCaptured to the captured piece
	void setMoveCapture(Move &move);
	void setIfEnPassantCapture(Move &move);

	std::vector<Move> mPossibleMoves;
	int mPositionValue;
	enum Player mPlayerToMove = Player::WHITE;
	std::vector<Bitboard> mBoard;
	//std::vector<Move> mHistory;
	Move mLastMove;
	int mCastlingAvailable[5] = {0, 1, 1, 1, 1}; //garbage kingSideW kingSideB QueenSideW QueenSideB
	int mKingNotMoved[3] = {0, 1, 1};			 //garbage, wking, bking - 0 means it moved
	bool mCanCastle = true;
	static Bitboard attackRays[64][8]; //precalculated rays first dim is square pos second dim is direction

	// std::chrono::time_point<std::chrono::system_clock> now{ std::chrono::system_clock::now() };
	// std::chrono::system_clock::duration epoch{ now.time_since_epoch() };
	// std::mt19937 randomEngine{
	// 		std::chrono::duration_cast<std::chrono::duration<unsigned int,
	// 		std::milli>>(epoch).count() };
};
