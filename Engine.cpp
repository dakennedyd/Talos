#include <string>
#include "Engine.h"
#include "Move.h"


Engine::Engine()
{	
	printInfo();
}


Engine::~Engine()
{
}

void Engine::whitePawnMoves()
{
	Bitboard p1, p2;//, pTotal;
	p1 = chessboard.mBoard[WHITE_PAWNS_BOARD];
	p1 = (p1 >> 8);
	p1 = p1 & (~(p1 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the panws that can advance one square

	p2 = (chessboard.mBoard[WHITE_PAWNS_BOARD] & RANK2) >> 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));
	p2 = (p2 >> 8);
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares
	//pTotal = p1 | p2;
	//printBitboard(pTotal);
	auto movesOneSquare = getBitsPosition(p1);
	for (auto &i : movesOneSquare)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i)));
		//mPossibleMoves.back().printMove();
	}
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 16), Square(i)));
		//mPossibleMoves.back().printMove();
	}
}

void Engine::whitePawnAttacks()
{
	Bitboard pAttackRight, pAttackLeft, pTotal;
	pAttackRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
}

void Engine::uci()
{
	std::string commandString;

	chessboard.reset();
	chessboard.setState(START_POS_FEN_STRING);
	bool validcommand = false;
	while (true) {
		std::getline(std::cin, commandString);
		auto command = split(commandString, " ");
		if (command.size() == 1)
		{
			if (command[0] == "uci")
			{
				std::cout << "id name " << NAME << "\n";
				std::cout << "id author " << AUTHOR << "\n";
				std::cout << "uciok\n";
				validcommand = true;
			}
			if (command[0] == "isready")
			{
				std::cout << "readyok\n";
				validcommand = true;
			}
			if (command[0] == "ucinewgame")
			{
				chessboard.reset();
				chessboard.setState(START_POS_FEN_STRING);
				validcommand = true;
			}
			if (command[0] == "d" || command[0] == "draw" || command[0] == "drawboard")
			{
				chessboard.printBoard();
				validcommand = true;
			}			
			if (command[0] == "quit")
			{
				exit(0);
				validcommand = true;
			}

		}
		if (command.size() > 1)
		{
			if (command[0] == "position")
			{
				if (command[1] == "startpos")
				{
					chessboard.setState(START_POS_FEN_STRING);
					if (command.size() > 3)
					{
						if(command[2] == "moves")
						for (int i = 3; i < command.size(); ++i)
						{
							//todo: check for input errors!
							auto a = SQUARE_STR_TO_NUM[command[i].substr(0, 2)];
							auto b = SQUARE_STR_TO_NUM[command[i].substr(2)];
							// auto m = Move(a, b);

							chessboard.makeMove(Move(a,b));
							// chessboard.makeMove(
							// 	Move(SQUARE_STR_TO_NUM[command[i].substr(0, 2)], SQUARE_STR_TO_NUM[command[i].substr(2)]));
						}
					}
					validcommand = true;
				}
			}
		}
		if (command[0] == "go")
		{
			generateMoves();
			validcommand = true;
		}
		if (!validcommand)
		{
			std::cout << "invalid command.\n";
		}
		validcommand = false;
	}
}

void Engine::printInfo()
{
	std::cout << NAME << " by " << AUTHOR << "\n";
}

void Engine::printBitboard(const Bitboard & bitboard)
{
	Bitboard b;
	for (uint64_t i = 0; i < 8; i++)
	{
		for (uint64_t j = 0; j < 8; j++)
		{
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & bitboard; if (b != 0) std::cout << " X ";
			else std::cout << " - ";
		}
		std::cout << "\n";
	}
}

void Engine::generateMoves()
{
	mPossibleMoves.clear();
	whitePawnMoves();
	std::cout << "bestmove ";
	auto move = mPossibleMoves[getRandomNumber(0, mPossibleMoves.size()-1)];
	move.printMove();
	//chessboard.makeMove(move);
}

std::vector<uint64_t> Engine::getBitsPosition(Bitboard board)
{
	uint64_t n;
	std::vector<uint64_t> setBits;
	setBits.reserve(64);
	for (n = 0; board != 0; n++, board &= (board - 1))
	{
		setBits.push_back(static_cast<uint64_t>(std::log2(board & ~(board - 1))));
		//std::cout << static_cast<uint64_t>(std::log2(board & ~(board - 1))) << " ";
	}
	
	return setBits;

}

