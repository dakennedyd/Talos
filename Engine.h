#pragma once
//#include <cstdlib>
#include "Chessboard.h"
#include "Move.h"

class Engine
{
public:
	Engine();
	~Engine() = default;
	void uci();
	void printInfo();
	void init();
			
	Chessboard & getChessBoard(){return chessboard;};
	int64_t search(const Chessboard position, const int depth, const Player maximizingPlayer);
private:		
	Chessboard chessboard;
};

