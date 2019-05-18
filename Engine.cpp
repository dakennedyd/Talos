#include <string>
#include "Engine.h"
#include "Move.h"


Bitboard Engine::checkIfOnPassantPossibleOnNextMove(const Square a, const Square b)
{
	//Bitboard startSquare = SQUARE_TO_BITBOARD[a];
	//Bitboard endSquare = SQUARE_TO_BITBOARD[b];
	Bitboard startSquare = Bitboard(1)<<a;
	Bitboard endSquare = Bitboard(1)<<b;
	//auto x = startSquare & chessboard.mBoard[WHITE_PAWNS_BOARD];
	//auto y = startSquare & RANK2;
	//auto z = endSquare & RANK4;
	if( (startSquare & chessboard.mBoard[WHITE_PAWNS_BOARD]) && (startSquare & RANK2)
	&&  (endSquare & RANK4) )
	{
		return startSquare >> 8;
	}

	if( (startSquare & chessboard.mBoard[BLACK_PAWNS_BOARD]) && (startSquare & RANK7)
	&&  (endSquare & RANK5) )
	{
		return startSquare << 8;
	}
	
	return 0x0;
}
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
	p1 = p1 >> 8;
	p1 = p1 & (~(p1 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the panws that can advance one square
	p1 = p1 & (~RANK8); //don't advance to last rank, promotion handles this part
	//printBitboard(pTotal);
	auto movesOneSquare = getBitsPosition(p1);
	for (auto &i : movesOneSquare)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	p2 = (chessboard.mBoard[WHITE_PAWNS_BOARD] & RANK2) >> 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));
	p2 = p2 >> 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 16), Square(i),Piece::NO_PIECE, i << 8));
		//mPossibleMoves.back().printMove();
	}

	//chessboard.mBoard[ONPASSANT_BOARD] = p2 << 8;

	//------------CAPTURE----------------------
	Bitboard captureLeft, captureRight;
	captureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
	captureRight = captureRight >> 9;
	captureRight = captureRight & (~HFILE);
	captureRight = captureRight & (~RANK8); //don't advance to last rank, promotion handles this part
	captureRight = captureRight & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto captureRightBits = getBitsPosition(captureRight);
	for (auto &i : captureRightBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	captureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
	captureLeft = captureLeft >> 7;
	captureLeft = captureLeft & (~AFILE);
	captureLeft = captureLeft & (~RANK8); //don't advance to last rank, promotion handles this part
	captureLeft = captureLeft & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto captureLeftBits = getBitsPosition(captureLeft);
	for (auto &i : captureLeftBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i)));
		//mPossibleMoves.back().printMove();
	}
	//------------ON PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureRight = opCaptureRight >> 9;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ONPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	opCaptureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft >> 7;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & chessboard.mBoard[ONPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	//------------PROMOTION----------------------
	Bitboard promotedPawn, capturePromotionL, capturePromotionR;
	promotedPawn = chessboard.mBoard[WHITE_PAWNS_BOARD];
	promotedPawn = promotedPawn & RANK7;
	
	capturePromotionR = promotedPawn;
	capturePromotionL = promotedPawn;

	promotedPawn = promotedPawn >> 8;
	promotedPawn = promotedPawn & (~chessboard.mBoard[ALL_PIECES_BOARD]);
	auto promotedPawnBits = getBitsPosition(promotedPawn);
	for (auto &i : promotedPawnBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i + 8), Square(i),Piece::BISHOP));
	}

	capturePromotionR = capturePromotionR >> 9;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{		
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i + 9), Square(i),Piece::BISHOP));
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL >> 7;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i + 7), Square(i),Piece::BISHOP));
		//mPossibleMoves.back().printMove();
	}
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
							auto b = SQUARE_STR_TO_NUM[command[i].substr(2,2)];
							auto p = command[i].substr(4);
							Piece promoted = Piece::NO_PIECE;
							if(p == "q") promoted = Piece::QUEEN;
							if(p == "r") promoted = Piece::ROOK;
							if(p == "b") promoted = Piece::BISHOP;
							if(p == "n") promoted = Piece::KNIGHT;
							// auto m = Move(a, b);

							auto op = checkIfOnPassantPossibleOnNextMove(a,b);
							chessboard.makeMove(Move(a, b, promoted, op));
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
			std::cout << "invalid command:" << commandString << "\n";
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

