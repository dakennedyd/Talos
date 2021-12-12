#pragma once
//#include "definitions.h"
#include "Chessboard.h"
#include "Move.h"

class Engine
{
public:
	Engine();
	~Engine();
	void uci();
	void printInfo();
	void init();		
	Chessboard & getChessBoard(){return chessboard;};
	int search(Chessboard &node, int depth, Player maxPlayer);
	uint64_t perft(const int depth);
private:		
	Chessboard chessboard;
	//uint64_t mPerftNodes = 0;
	//uint64_t mNodesCounter = 0;
};

