#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
#include "definitions.h"

struct Move
{
	//Move() = delete;
	Move(const enum Piece piece, const enum Player side,
	const enum Square moveFrom, const enum Square moveTo,
	const enum Piece piecePromotion = Piece::NO_PIECE,
	const Bitboard onPassant = 0x0 )
	: mMoveFrom(moveFrom), mMoveTo(moveTo), mPromoteTo(piecePromotion),
	mOnPassantBoard(onPassant) {
		//SQUARE_TO_STR.emplace(std::pair<Square, std::string>{ A1,"a1" });
	}
	~Move() = default;
	void printMove();

	Piece mPiece;
	Player mSideToMove;
	Square mMoveFrom;
	Square mMoveTo;
	Piece mPromoteTo;
	double mScore = 0;
	Bitboard mOnPassantBoard = 0;
	Piece mCaptured;
};

