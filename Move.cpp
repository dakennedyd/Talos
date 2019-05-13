#include <iostream>
#include "Move.h"
#include "definitions.h"
#include "BoardState.h"

void Move::printMove()
{
	
	auto a = SQUARE_NUM_TO_STR.at(mMoveFrom);
	std::cout << SQUARE_NUM_TO_STR[mMoveFrom] << SQUARE_NUM_TO_STR[mMoveTo] << "\n";
}
