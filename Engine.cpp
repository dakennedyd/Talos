#include <string>
#include "Engine.h"
#include "Move.h"
#include <algorithm>

Engine::Engine()
{
	//mPossibleMoves.reserve(100);
	printInfo();
	//generateRays();
}

Engine::~Engine()
{
}

void Engine::init()
{
	chessboard.generateRays();
}

void Engine::uci()
{
	std::string commandString;

	chessboard.reset();
	chessboard.setState(START_POS_FEN_STRING);
	bool validcommand = false;
	while (true)
	{
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
			if (command[0] == "d" || command[0] == "draw" || command[0] == "drawboard" || command[0] == "print")
			{
				chessboard.printBoard();
				validcommand = true;
			}
			if (command[0] == "pbb" || command[0] == "printbitboards")
			{
				//checkIfSquaresAreAttackedByWhite(Bitboard(0));
				//checkIfSquaresAreAttackedByBlack(Bitboard(0));
				chessboard.printBitboards();
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
						if (command[2] == "moves")
							for (size_t i = 3; i < command.size(); ++i)
							{
								//todo: check for input errors!
								auto moveFromSquare = STR_TO_SQUARE[command[i].substr(0, 2)];
								auto moveToSquare = STR_TO_SQUARE[command[i].substr(2, 2)];
								auto promotion = command[i].substr(4);
								Piece promoted = Piece::NO_PIECE;
								if (promotion == "q")
									promoted = Piece::QUEEN;
								if (promotion == "r")
									promoted = Piece::ROOK;
								if (promotion == "b")
									promoted = Piece::BISHOP;
								if (promotion == "n")
									promoted = Piece::KNIGHT;
								// auto m = Move(a, b);

								auto op = chessboard.checkIfenPassantPossibleOnNextMove(moveFromSquare, moveToSquare);
								auto piece = chessboard.getPieceFromSquare(moveFromSquare);

								auto m = Move(piece, chessboard.mPlayerToMove, moveFromSquare, moveToSquare, promoted, op);
								if (moveFromSquare == Square::E1 && moveToSquare == Square::G1 && piece == Piece::KING)
									m.mCastle = Castling::KINGSIDE;
								if (moveFromSquare == Square::E1 && moveToSquare == Square::C1 && piece == Piece::KING)
									m.mCastle = Castling::QUEENSIDE;
								if (moveFromSquare == Square::E8 && moveToSquare == Square::G8 && piece == Piece::KING)
									m.mCastle = Castling::KINGSIDE;
								if (moveFromSquare == Square::E8 && moveToSquare == Square::C8 && piece == Piece::KING)
									m.mCastle = Castling::QUEENSIDE;

								chessboard.setMoveCapture(m);
								chessboard.makeMove(m);
								if (chessboard.mPlayerToMove == WHITE)
									chessboard.mPlayerToMove = BLACK;
								else
									chessboard.mPlayerToMove = WHITE;
							}
					}
					validcommand = true;
				}
			}
		}
		if (command[0] == "go")
		{
			//chessboard.generateMoves();
			search(chessboard, 2, chessboard.mPlayerToMove);
			if (chessboard.mPlayerToMove == BLACK)
			{
				for (auto &m : chessboard.mPossibleMoves)
				{
					m.mScore *= -1;
				}
			}
			std::sort(chessboard.mPossibleMoves.begin(), chessboard.mPossibleMoves.end(),
					  [](Move const &a, Move const &b)
					  { return a.mScore > b.mScore; });
			//auto move = chessboard.mPossibleMoves[getRandomNumber(0, mPossibleMoves.size()-1)];
			//auto move = chessboard.mPossibleMoves[0];
			
			std::cout << "info multipv 1 depth 3 score cp " << chessboard.mPossibleMoves[0].mScore << "\n";
			std::cout << "bestmove ";
			//move.printMove();
			chessboard.mPossibleMoves[0].printMove();
			std::cout << "\n";

			validcommand = true;
		}
		if (command[0] == "perft")
		{
			if (command.size() > 1)
			{
				auto depth = std::stoi(command[1]);
				chessboard.reset();
				chessboard.setState(START_POS_FEN_STRING);
				std::cout << "depth   nodes\n";
				//mPerftNodes = 0;
				//mNodesCounter = 0;
				chessboard.mPossibleMoves.clear();
				std::cout << depth << "       " << perft(depth) << "\n";
			}else{
				//perft();
			}
			validcommand = true;
		}
		if (!validcommand)
		{
			std::cout << "info invalid command:" << commandString << "\n";
		}
		validcommand = false;
	}
}

uint64_t Engine::perft(const int depth)
{
	if (depth < 1)
    	return uint64_t(1);

	auto moveList = chessboard.generateMovesPerft();
	int perftNodes = 0;
	for(auto i : moveList)
	{
		chessboard.makeMove(i);
		perftNodes += perft(depth - 1);
		chessboard.unmakeMove();
  	}
	return perftNodes;
}

void Engine::printInfo()
{
	std::cout << NAME << " by " << AUTHOR << "\n";
}

int Engine::search(Chessboard &node, int depth, Player maxPlayer)
{
	node.generateMoves();
	if(node.mPossibleMoves.size() == 0)
	{
		if(maxPlayer == WHITE)
			return INT_MAX_VALUE;
		else
			return INT_MIN_VALUE;
	}
	if (depth == 0)
	{
		node.evaluation();
		return node.mPositionValue;
	}
	if (maxPlayer == WHITE)
	{
		node.mPositionValue = INT_MIN_VALUE;
		for (auto &move : node.mPossibleMoves)
		{
			auto child = node;
			child.makeMove(move);
			child.mPlayerToMove = BLACK;
			node.mPositionValue = search(child, depth - 1, BLACK);
			move.mScore = std::max(node.mPositionValue, move.mScore);
		}
		return node.mPositionValue;
	}
	else
	{
		node.mPositionValue = INT_MAX_VALUE;
		for (auto &move : node.mPossibleMoves)
		{
			auto child = node;
			child.makeMove(move);
			child.mPlayerToMove = WHITE;
			node.mPositionValue = search(child, depth - 1, WHITE);
			move.mScore = std::min(node.mPositionValue, move.mScore);
		}
		return node.mPositionValue;
	}
}
