#include <string>
#include "Engine.h"
#include "Move.h"

Engine::Engine()
{		
	printInfo();
}

void Engine::init()
{
	generateRays();
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
							// chessboard.makeMove(
							// 	Move(STR_TO_SQUARE[command[i].substr(0, 2)], STR_TO_SQUARE[command[i].substr(2)]));
						}
					}
					validcommand = true;
				}
			}
		}
		if (command[0] == "go")
		{
			chessboard.generatePossibleMoves();
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

int64_t Engine::search(const Chessboard position, const int depth, const Player maximizingPlayer)
{
	//if(depth == 0 || position.)
}

