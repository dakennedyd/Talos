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
						if(command[2] == "moves")
						for (int i = 3; i < command.size(); ++i)
						{
							//todo: check for input errors!
							auto a = STR_TO_SQUARE[command[i].substr(0, 2)];
							auto b = STR_TO_SQUARE[command[i].substr(2,2)];
							auto p = command[i].substr(4);
							Piece promoted = Piece::NO_PIECE;
							if(p == "q") promoted = Piece::QUEEN;
							if(p == "r") promoted = Piece::ROOK;
							if(p == "b") promoted = Piece::BISHOP;
							if(p == "n") promoted = Piece::KNIGHT;
							// auto m = Move(a, b);
							

							auto op = chessboard.checkIfenPassantPossibleOnNextMove(a,b);
							auto piece = chessboard.getPieceFromSquare(a);

							auto m = Move(piece, chessboard.mPlayerToMove, a, b, promoted, op);
							if(a == Square::E1 && b == Square::G1 && piece == Piece::KING
								&& chessboard.mPlayerToMove == Player::WHITE)
								m.mCastle = Castling::KINGSIDE;
							if(a == Square::E1 && b == Square::C1 && piece == Piece::KING
								&& chessboard.mPlayerToMove == Player::WHITE)
								m.mCastle = Castling::QUEENSIDE;
							if(a == Square::E8 && b == Square::G8 && piece == Piece::KING
								&& chessboard.mPlayerToMove == Player::BLACK)
								m.mCastle = Castling::KINGSIDE;
							if(a == Square::E8 && b == Square::C8 && piece == Piece::KING
								&& chessboard.mPlayerToMove == Player::BLACK)
								m.mCastle = Castling::QUEENSIDE;

							chessboard.setMoveCapture(m);
							chessboard.setIfEnPassantCapture(m);
							chessboard.makeMove(m);
							if(chessboard.mPlayerToMove == WHITE) chessboard.mPlayerToMove = BLACK;
							else chessboard.mPlayerToMove = WHITE;
						}
					}
					validcommand = true;
				}
			}
		}
		if (command[0] == "go")
		{
			//chessboard.generateMoves();
			search(chessboard, 3, chessboard.mPlayerToMove);			
			std::sort(chessboard.mPossibleMoves.begin(), chessboard.mPossibleMoves.end(),
          		[] (Move const& a, Move const& b) { return a.mScore < b.mScore; });
			//auto move = chessboard.mPossibleMoves[getRandomNumber(0, mPossibleMoves.size()-1)];
			//auto move = chessboard.mPossibleMoves[0];
			std::cout << "bestmove ";
			//move.printMove();
			chessboard.mPossibleMoves[0].printMove();
			std::cout << "\n";

			validcommand = true;
		}
		if (!validcommand)
		{
			std::cout << "invalid command:" << commandString << "\n";
		}
		validcommand = false;
	}
}

void Engine::printInfo()
{
	std::cout << NAME << " by " << AUTHOR << "\n";
}

int Engine::search(Chessboard &node, int depth, Player maxPlayer)
{
	node.generateMoves();
	if(depth == 0 || node.mPossibleMoves.size() == 0)
	{
		node.setPositionValue();
		return node.mPositionValue;
	}
	auto child = node;
	if(maxPlayer == WHITE)
	{
		node.mPositionValue = INT_MIN_VALUE;
		for(auto n : node.mPossibleMoves)
		{
			child.makeMove(n);
			node.mPositionValue = std::max(node.mPositionValue, search(child,depth-1, BLACK));
			n.mScore = node.mPositionValue;
		}
		return child.mPositionValue;
	}else
	{
		child.mPositionValue = INT_MAX_VALUE;
		for(auto n : node.mPossibleMoves)
		{
			child.makeMove(n);
			node.mPositionValue = std::min(node.mPositionValue, search(child,depth-1, WHITE));
			n.mScore = node.mPositionValue;
		}
		return child.mPositionValue;
	}
	
}
