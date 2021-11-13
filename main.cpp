// #include "definitions.h"
//#include "BoardState.h"
#include "Engine.h"

int main()
{

	Engine engine;

	//std::string input;
	//engine.getChessBoard().setState("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	//chessboard.setState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//engine.chessboard.printBoard();
	//std::cout << "\n";
	//engine.whitePawnMoves();
	//printBitboard(cFile);
	//std::getline(std::cin, input);
	engine.init();
	engine.uci();
    return 0;
}

