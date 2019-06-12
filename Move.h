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
	const Bitboard enPassant = 0x0 )
	: mPiece(piece), mSide(side),
	mMoveFrom(moveFrom), mMoveTo(moveTo), mPromoteTo(piecePromotion),
	mEnPassantBoard(enPassant) {}
	~Move() = default;
	void printMove();

	Piece mPiece;
	Player mSide;
	Square mMoveFrom;
	Square mMoveTo;
	Piece mPromoteTo;
	double mScore = 0;
	Bitboard mEnPassantBoard = 0; //sinals en passant possible on next move
	bool enPassantCapture = false; //is this move an en passant capture?
	Piece mCaptured = Piece::NO_PIECE;
	Castling mCastle = Castling::NONE;
};

