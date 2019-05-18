#include <iostream>
#include "Move.h"
#include "definitions.h"
#include "BoardState.h"

void Move::printMove()
{
	//fix this
	auto a = SQUARE_NUM_TO_STR.at(mMoveFrom);
	
	std::string p = "";
	if(mPromoteTo == Piece::QUEEN) p = "q";
	if(mPromoteTo == Piece::ROOK) p = "r";
	if(mPromoteTo == Piece::KNIGHT) p = "n";
	if(mPromoteTo == Piece::BISHOP) p = "b";
		
	std::cout << SQUARE_NUM_TO_STR[mMoveFrom] << SQUARE_NUM_TO_STR[mMoveTo] << p <<"\n";	
}
