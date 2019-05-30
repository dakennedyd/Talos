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
	mPossibleMoves.reserve(100);
	printInfo();
}


Engine::~Engine()
{
}

void Engine::init()
{
	generateRays();
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

void Engine::blackPawnMoves()
{
	Bitboard p1, p2;//, pTotal;
	p1 = chessboard.mBoard[BLACK_PAWNS_BOARD];
	p1 = p1 << 8;
	p1 = p1 & (~(p1 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the panws that can advance one square
	p1 = p1 & (~RANK1); //don't advance to last rank, promotion handles this part
	//printBitboard(pTotal);
	auto movesOneSquare = getBitsPosition(p1);
	for (auto &i : movesOneSquare)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 8), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	p2 = (chessboard.mBoard[BLACK_PAWNS_BOARD] & RANK7) << 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));
	p2 = p2 << 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 16), Square(i),Piece::NO_PIECE, i >> 8));
	}

	//------------CAPTURE----------------------
	Bitboard captureLeft, captureRight;
	captureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
	captureRight = captureRight << 7;
	captureRight = captureRight & (~HFILE);
	captureRight = captureRight & (~RANK1); //don't advance to last rank, promotion handles this part
	captureRight = captureRight & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto captureRightBits = getBitsPosition(captureRight);
	for (auto &i : captureRightBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	captureLeft = chessboard.mBoard[BLACK_PAWNS_BOARD];
	captureLeft = captureLeft << 9;
	captureLeft = captureLeft & (~AFILE);
	captureLeft = captureLeft & (~RANK1); //don't advance to last rank, promotion handles this part
	captureLeft = captureLeft & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto captureLeftBits = getBitsPosition(captureLeft);
	for (auto &i : captureLeftBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i)));
		//mPossibleMoves.back().printMove();
	}
	//------------ON PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
	opCaptureRight = opCaptureRight << 7;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ONPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	opCaptureLeft = chessboard.mBoard[BLACK_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft << 9;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & chessboard.mBoard[ONPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i)));
		//mPossibleMoves.back().printMove();
	}

	//------------PROMOTION----------------------
	Bitboard promotedPawn, capturePromotionL, capturePromotionR;
	promotedPawn = chessboard.mBoard[BLACK_PAWNS_BOARD];
	promotedPawn = promotedPawn & RANK2;
	
	capturePromotionR = promotedPawn;
	capturePromotionL = promotedPawn;

	promotedPawn = promotedPawn << 8;
	promotedPawn = promotedPawn & (~chessboard.mBoard[ALL_PIECES_BOARD]);
	auto promotedPawnBits = getBitsPosition(promotedPawn);
	for (auto &i : promotedPawnBits)
	{
		mPossibleMoves.emplace_back(Move(Square(i - 8), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i - 8), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i - 8), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i - 8), Square(i),Piece::BISHOP));
	}

	capturePromotionR = capturePromotionR << 7;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{		
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i - 7), Square(i),Piece::BISHOP));
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL << 9;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i),Piece::QUEEN));
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i),Piece::ROOK));
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i),Piece::KNIGHT));
		mPossibleMoves.emplace_back(Move(Square(i - 9), Square(i),Piece::BISHOP));
		//mPossibleMoves.back().printMove();
	}
}

void Engine::whiteKnightMoves()
{
	Bitboard knightsBoard = chessboard.mBoard[WHITE_KNIGHTS_BOARD];
	Bitboard elevenOClockAttack = knightsBoard >> 17;
	Bitboard tenOClockAttack = knightsBoard >> 10;
	Bitboard eightOClockAttack = knightsBoard << 6;
	Bitboard sevenOClockAttack = knightsBoard << 15;
	Bitboard fiveOClockAttack = knightsBoard << 17;
	Bitboard fourOClockAttack = knightsBoard << 10;
	Bitboard twoOClockAttack = knightsBoard >> 6;
	Bitboard oneOClockAttack = knightsBoard >> 15;

	//oneOClockAttack can't land on RANK1 or RANK2 or AFILE
	oneOClockAttack = oneOClockAttack & (~(RANK1 | RANK2 | AFILE));	
	twoOClockAttack = twoOClockAttack & (~(AFILE | RANK1 | BFILE));
	fourOClockAttack = fourOClockAttack & (~(AFILE | RANK8 | BFILE));
	fiveOClockAttack = fiveOClockAttack & (~(AFILE | RANK8 | RANK7));
	sevenOClockAttack = sevenOClockAttack & (~(HFILE | RANK8 | RANK7));
	eightOClockAttack = eightOClockAttack & (~(HFILE | RANK8 | GFILE));
	tenOClockAttack = tenOClockAttack & (~(HFILE | RANK1 | GFILE));
	elevenOClockAttack = elevenOClockAttack & (~(HFILE | RANK1 | RANK2));
	
	auto oneOClockAttackBits = getBitsPosition(oneOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : oneOClockAttackBits)	mPossibleMoves.emplace_back(Move(Square(i + 15), Square(i)));
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)	mPossibleMoves.emplace_back(Move(Square(i + 6), Square(i)));
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 10), Square(i)));

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 17), Square(i)));

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 15), Square(i)));

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 6), Square(i)));

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i + 10), Square(i)));

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i + 17), Square(i)));

}

void Engine::blackKnightMoves()
{
	Bitboard knightsBoard = chessboard.mBoard[BLACK_KNIGHTS_BOARD];
	Bitboard elevenOClockAttack = knightsBoard >> 17;
	Bitboard tenOClockAttack = knightsBoard >> 10;
	Bitboard eightOClockAttack = knightsBoard << 6;
	Bitboard sevenOClockAttack = knightsBoard << 15;
	Bitboard fiveOClockAttack = knightsBoard << 17;
	Bitboard fourOClockAttack = knightsBoard << 10;
	Bitboard twoOClockAttack = knightsBoard >> 6;
	Bitboard oneOClockAttack = knightsBoard >> 15;

	//oneOClockAttack can't land on RANK1 or RANK2 or AFILE
	oneOClockAttack = oneOClockAttack & (~(RANK1 | RANK2 | AFILE));	
	twoOClockAttack = twoOClockAttack & (~(AFILE | RANK1 | BFILE));
	fourOClockAttack = fourOClockAttack & (~(AFILE | RANK8 | BFILE));
	fiveOClockAttack = fiveOClockAttack & (~(AFILE | RANK8 | RANK7));
	sevenOClockAttack = sevenOClockAttack & (~(HFILE | RANK8 | RANK7));
	eightOClockAttack = eightOClockAttack & (~(HFILE | RANK8 | GFILE));
	tenOClockAttack = tenOClockAttack & (~(HFILE | RANK1 | GFILE));
	elevenOClockAttack = elevenOClockAttack & (~(HFILE | RANK1 | RANK2));
	
	auto oneOClockAttackBits = getBitsPosition(oneOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : oneOClockAttackBits)	mPossibleMoves.emplace_back(Move(Square(i + 15), Square(i)));
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)	mPossibleMoves.emplace_back(Move(Square(i + 6), Square(i)));
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 10), Square(i)));

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 17), Square(i)));

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 15), Square(i)));

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i - 6), Square(i)));

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i + 10), Square(i)));

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Square(i + 17), Square(i)));
}

void Engine::whiteRookMoves()
{
	Bitboard rook = chessboard.mBoard[WHITE_ROOKS_BOARD];
	int rookBitPos = 0;
	while(rookBitPos < 64)
	{		
		if(rook & uint64_t(1))
		{
			rookBitPos++;
			rook = rook >> 1;
			if(rookBitPos > 63) return; //no rook on the board
		}else{
			while((rook & uint64_t(1)) == 0x0)	
			{
				rookBitPos++;
				rook = rook >> 1;
				if(rookBitPos > 63) return; //no rook on the board
			}
		}
		Bitboard currentRook = uint64_t(1) << rookBitPos;
			
		//up
		{
			Bitboard rookAttack = 0;
			int counter = 0;
			while(currentRook & ~RANK8)
			{
				if(rookAttack & RANK8) break;
				counter++;
				rookAttack = rookAttack | (currentRook >> (counter*8));
				if (rookAttack & chessboard.mBoard[WHITE_PIECES_BOARD]) //can't capture own pieces
				{
					rookAttack = rookAttack & ~(currentRook >> (counter*8));
					break;
				}
				if (rookAttack & chessboard.mBoard[BLACK_PIECES_BOARD]) break;
			}
			auto rookAttackBits = getBitsPosition(rookAttack);
			for (auto &i : rookAttackBits)
			{
				mPossibleMoves.emplace_back(Move(Square(rookBitPos), Square(i)));
			}
		}
		//right
		{
			Bitboard rookAttack = 0;
			int counter = 0;
			while(currentRook & ~HFILE)
			{
				if(rookAttack & HFILE) break;
				counter++;
				rookAttack = rookAttack | (currentRook << counter);
				if (rookAttack & chessboard.mBoard[WHITE_PIECES_BOARD]) //can't capture own pieces
				{
					rookAttack = rookAttack & ~(currentRook << counter);
					break;
				}
				if (rookAttack & chessboard.mBoard[BLACK_PIECES_BOARD]) break;
			}
			auto rookAttackBits = getBitsPosition(rookAttack);
			for (auto &i : rookAttackBits)
			{
				mPossibleMoves.emplace_back(Move(Square(rookBitPos), Square(i)));
			}
		}
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

void Engine::generateMoves()
{
	mPossibleMoves.clear();
	if(chessboard.mPlayerToMove == Player::WHITE)
	{
		whitePawnMoves();
		//whiteKnightMoves();
		whiteRookMoves();
	}else{
		blackPawnMoves();
		//blackKnightMoves();
	}
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

void Engine::generateRays()
{
	
	int bitCount = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			//NORTH
			mRays[bitCount][NORTH] = 0;
			for(int k = 0; k < i; k++)
			{
				uint64_t x = uint64_t(1) << bitCount;
				mRays[bitCount][NORTH] |= (x >> ( 8*(k+1)));
			}
			//NORTH-EAST
			mRays[bitCount][NORTH_EAST] = 0;
			int minNE = i < (7-j) ? i : (7-j);
			for(int k = 0; k < minNE; k++)
			{
				mRays[bitCount][NORTH_EAST] |= (uint64_t(1) << ( bitCount - ( 7*(k+1) ) ));
			}
			//EAST
			mRays[bitCount][EAST] = 0;
			for(int k = 0; k < 7-j; k++)
			{
				mRays[bitCount][EAST] |= (uint64_t(1) << ( bitCount + ( k+1 ) ));
			}
			//SOUTH-EAST
			mRays[bitCount][SOUTH_EAST] = 0;
			int minSE = (7-i)<(7-j) ? 7-i : 7-j;
			for(int k = 0; k < minSE; k++)
			{
				mRays[bitCount][SOUTH_EAST] |= (uint64_t(1) << ( bitCount + ( 9*(k+1) ) ));
			}
			//SOUTH
			mRays[bitCount][SOUTH] = 0;
			for(int k = 0; k < 7-i; k++)
			{
				mRays[bitCount][SOUTH] |= (uint64_t(1) << ( bitCount + ( 8*(k+1) ) ));
			}
			//SOUTH-WEST
			mRays[bitCount][SOUTH_WEST] = 0;
			int minSW = (7-i) < j ? (7-i) : j;
			for(int k = 0; k < minSW; k++)
			{
				mRays[bitCount][SOUTH_WEST] |= (uint64_t(1) << ( bitCount + ( 7*(k+1) ) ));
			}
			//WEST
			mRays[bitCount][WEST] = 0;
			for(int k = 0; k < j; k++)
			{
				mRays[bitCount][WEST] |= (uint64_t(1) << ( bitCount - ( k+1 ) ));
			}
			//NORTH-WEST
			mRays[bitCount][NORTH_WEST] = 0;
			int minNW1 = j < i ? j : i;
			int minNW2 = i < j ? i : j;
			int minNW = minNW1 > minNW2 ? minNW1 : minNW2;
			if(i == 7) minNW = j;
			for(int k = 0; k < minNW; k++)
			{
				mRays[bitCount][NORTH_WEST] |= (uint64_t(1) << ( bitCount - ( 9*(k+1) ) ));
			}
			bitCount++;
		}
	}
	Bitboard b = 0;
	for(int i = 0; i < 64; i++)
	{
		for(int i2 = 0; i2 < 8; i2++)
		{
			for(int j2 = 0; j2 < 8; j2++)
			{
				b = (static_cast<uint64_t>(1) << (i2 * 8 + j2)) & mRays[i][NORTH_WEST];
				if (b != 0) std::cout << " X "; else std::cout << " - ";
			}
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}	
}

