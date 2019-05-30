#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
#include "definitions.h"

struct Move
{
	//Move() = delete;
	Move(const enum Square moveFrom, const enum Square moveTo, const enum Piece piecePromotion = Piece::NO_PIECE,
	const Bitboard onPassant = 0x0 )
	: mMoveFrom(moveFrom), mMoveTo(moveTo), mPromoteTo(piecePromotion),
	mOnPassantBoard(onPassant) {
		//SQUARE_NUM_TO_STR.emplace(std::pair<Square, std::string>{ A1,"a1" });
	}
	~Move() = default;
	void printMove();

	Square mMoveFrom;
	Square mMoveTo;
	Piece mPromoteTo;
	double mScore = 0;
	Bitboard mOnPassantBoard;
	Player mSideToMove;
	Piece mPiece;
};

