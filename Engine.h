#pragma once
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
			
	Bitboard checkIfenPassantPossibleOnNextMove(const Square a, const Square b);
	Piece getPieceFromSquare(const Square square);	
	Chessboard chessboard;	
	std::vector<Move> mPossibleMoves;
};

