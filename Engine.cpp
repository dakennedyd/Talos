#include <string>
#include "Engine.h"
#include "Move.h"


Bitboard Engine::checkIfenPassantPossibleOnNextMove(const Square a, const Square b)
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

Piece Engine::getPieceFromSquare(const Square square)
{
	Bitboard pieceBoard = uint64_t(1) << square;
	Bitboard p;
	int i = 0;
	for(i = 0; i<7; i++)
	{
		p = chessboard.mBoard[i+chessboard.mPlayerToMove];
		if(p & pieceBoard) return Piece(i+chessboard.mPlayerToMove);
	}
	return Piece::NO_PIECE;
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
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		//mPossibleMoves.back().printMove();
	}

	p2 = (chessboard.mBoard[WHITE_PAWNS_BOARD] & RANK2) >> 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));
	p2 = p2 >> 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 16), Square(i),Piece::NO_PIECE, i << 8);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		//mPossibleMoves.back().printMove();
	}

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
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
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
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
		//mPossibleMoves.back().printMove();
	}
	//------------ON PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureRight = opCaptureRight >> 9;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
		//mPossibleMoves.back().printMove();
	}

	opCaptureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft >> 7;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
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
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
	}

	capturePromotionR = capturePromotionR >> 9;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL >> 7;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
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
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
	}

	p2 = (chessboard.mBoard[BLACK_PAWNS_BOARD] & RANK7) << 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));
	p2 = p2 << 8;
	p2 = p2 & (~(p2 & chessboard.mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 16), Square(i),Piece::NO_PIECE, i >> 8);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
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
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
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
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		//mPossibleMoves.back().printMove();
	}
	//------------ON PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
	opCaptureRight = opCaptureRight << 7;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		//mPossibleMoves.back().printMove();
	}

	opCaptureLeft = chessboard.mBoard[BLACK_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft << 9;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i));
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
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
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
	}

	capturePromotionR = capturePromotionR << 7;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{	
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
		
		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}		
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL << 9;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::QUEEN);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			chessboard.unmakeMove();
		}else{
			mPossibleMoves.emplace_back(m);
		}
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
	for (auto &i : oneOClockAttackBits)	mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i + 15), Square(i)));
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)	mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i + 6), Square(i)));
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i - 10), Square(i)));

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i - 17), Square(i)));

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i - 15), Square(i)));

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i - 6), Square(i)));

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i + 10), Square(i)));

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::WHITE, Square(i + 17), Square(i)));

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
	for (auto &i : oneOClockAttackBits)	mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i + 15), Square(i)));
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)	mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i + 6), Square(i)));
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i - 10), Square(i)));

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i - 17), Square(i)));

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i - 15), Square(i)));

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i - 6), Square(i)));

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i + 10), Square(i)));

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits) mPossibleMoves.emplace_back(Move(Piece::KNIGHT, Player::BLACK, Square(i + 17), Square(i)));
}

void Engine::whiteRookMoves()
{	
	auto rookBits = getBitsPosition(chessboard.mBoard[WHITE_ROOKS_BOARD]);
	for (auto rookBitPos : rookBits) //for every white rook on the board
	{
		Bitboard rookAttack = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH ATTACK
			temp = attackRays[rookBitPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH];
			temp = attackRays[rookBitPos][NORTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//EAST ATTACK
			temp = attackRays[rookBitPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][EAST];
			temp = attackRays[rookBitPos][EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//SOUTH ATTACK
			temp = attackRays[rookBitPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH];
			temp = attackRays[rookBitPos][SOUTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//WEST ATTACK
			temp = attackRays[rookBitPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][WEST];
			temp = attackRays[rookBitPos][WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		
		auto rookAttackBits = getBitsPosition(rookAttack);
		for (auto &i : rookAttackBits) 
			mPossibleMoves.emplace_back(Move(Piece::ROOK, Player::WHITE, Square(rookBitPos), Square(i)));
	}
}

void Engine::blackRookMoves()
{	
	auto rookBits = getBitsPosition(chessboard.mBoard[BLACK_ROOKS_BOARD]);
	for (auto rookBitPos : rookBits) //for every white rook on the board
	{
		Bitboard rookAttack = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH ATTACK
			temp = attackRays[rookBitPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH];
			temp = attackRays[rookBitPos][NORTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//EAST ATTACK
			temp = attackRays[rookBitPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][EAST];
			temp = attackRays[rookBitPos][EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//SOUTH ATTACK
			temp = attackRays[rookBitPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH];
			temp = attackRays[rookBitPos][SOUTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		{
			//WEST ATTACK
			temp = attackRays[rookBitPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][WEST];
			temp = attackRays[rookBitPos][WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(rookAttack);
		}
		
		auto rookAttackBits = getBitsPosition(rookAttack);
		for (auto &i : rookAttackBits) 
			mPossibleMoves.emplace_back(Move(Piece::ROOK, Player::BLACK, Square(rookBitPos), Square(i)));
	}
}

void Engine::whiteBishopMoves()
{
	auto bishopBits = getBitsPosition(chessboard.mBoard[WHITE_BISHOPS_BOARD]);
	for (auto bishopBitPos : bishopBits) //for every white rook on the board
	{
		Bitboard bishopAttack = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH-EAST ATTACK
			temp = attackRays[bishopBitPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_EAST];
			temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//SOUTH-EAST ATTACK
			temp = attackRays[bishopBitPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_EAST];
			temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//SOUTH-WEST ATTACK
			temp = attackRays[bishopBitPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_WEST];
			temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//NORTH-WEST ATTACK
			temp = attackRays[bishopBitPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_WEST];
			temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		
		auto bishopAttackBits = getBitsPosition(bishopAttack);
		for (auto &i : bishopAttackBits) 
			mPossibleMoves.emplace_back(Move(Piece::BISHOP, Player::WHITE, Square(bishopBitPos), Square(i)));
	}
}

void Engine::blackBishopMoves()
{
	auto bishopBits = getBitsPosition(chessboard.mBoard[BLACK_BISHOPS_BOARD]);
	for (auto bishopBitPos : bishopBits) //for every white rook on the board
	{
		Bitboard bishopAttack = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH-EAST ATTACK
			temp = attackRays[bishopBitPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_EAST];
			temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//SOUTH-EAST ATTACK
			temp = attackRays[bishopBitPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_EAST];
			temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//SOUTH-WEST ATTACK
			temp = attackRays[bishopBitPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_WEST];
			temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		{
			//NORTH-WEST ATTACK
			temp = attackRays[bishopBitPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_WEST];
			temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(bishopAttack);
		}
		
		auto bishopAttackBits = getBitsPosition(bishopAttack);
		for (auto &i : bishopAttackBits) 
			mPossibleMoves.emplace_back(Move(Piece::BISHOP, Player::BLACK, Square(bishopBitPos), Square(i)));
	}
}

void Engine::whiteQueenMoves()
{
	auto queenBits = getBitsPosition(chessboard.mBoard[WHITE_QUEEN_BOARD]);
	for (auto queenBitsPos : queenBits) //for every white rook on the board
	{
		Bitboard queenAttacks = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH ATTACK
			temp = attackRays[queenBitsPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH];
			temp = attackRays[queenBitsPos][NORTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//EAST ATTACK
			temp = attackRays[queenBitsPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][EAST];
			temp = attackRays[queenBitsPos][EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH ATTACK
			temp = attackRays[queenBitsPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH];
			temp = attackRays[queenBitsPos][SOUTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//WEST ATTACK
			temp = attackRays[queenBitsPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][WEST];
			temp = attackRays[queenBitsPos][WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//NORTH-EAST ATTACK
			temp = attackRays[queenBitsPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_EAST];
			temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH-EAST ATTACK
			temp = attackRays[queenBitsPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_EAST];
			temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH-WEST ATTACK
			temp = attackRays[queenBitsPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_WEST];
			temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//NORTH-WEST ATTACK
			temp = attackRays[queenBitsPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_WEST];
			temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		
		auto queenAttacksBits = getBitsPosition(queenAttacks);
		for (auto &i : queenAttacksBits) 
			mPossibleMoves.emplace_back(Move(Piece::QUEEN, Player::WHITE, Square(queenBitsPos), Square(i)));
	}
}

void Engine::blackQueenMoves()
{
	auto queenBits = getBitsPosition(chessboard.mBoard[BLACK_QUEEN_BOARD]);
	for (auto queenBitsPos : queenBits) //for every white rook on the board
	{
		Bitboard queenAttacks = 0, temp;
		Square firstBlocker;
		
		{
			//NORTH ATTACK
			temp = attackRays[queenBitsPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH];
			temp = attackRays[queenBitsPos][NORTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//EAST ATTACK
			temp = attackRays[queenBitsPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][EAST];
			temp = attackRays[queenBitsPos][EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH ATTACK
			temp = attackRays[queenBitsPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH];
			temp = attackRays[queenBitsPos][SOUTH] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//WEST ATTACK
			temp = attackRays[queenBitsPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][WEST];
			temp = attackRays[queenBitsPos][WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//NORTH-EAST ATTACK
			temp = attackRays[queenBitsPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_EAST];
			temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH-EAST ATTACK
			temp = attackRays[queenBitsPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_EAST];
			temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//SOUTH-WEST ATTACK
			temp = attackRays[queenBitsPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getLowestSetBit(temp);
			temp = attackRays[firstBlocker][SOUTH_WEST];
			temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		{
			//NORTH-WEST ATTACK
			temp = attackRays[queenBitsPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
			firstBlocker = getHighestSetBit(temp);
			temp = attackRays[firstBlocker][NORTH_WEST];
			temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

			//can't capture own pieces
			Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
			firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
			queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
			//chessboard.printBitboard(queenAttacks);
		}
		
		auto queenAttacksBits = getBitsPosition(queenAttacks);
		for (auto &i : queenAttacksBits) 
			mPossibleMoves.emplace_back(Move(Piece::QUEEN, Player::BLACK, Square(queenBitsPos), Square(i)));
	}
}

void Engine::whiteKingMoves()
{
	Bitboard kingBoard = chessboard.mBoard[WHITE_KING_BOARD];
	Bitboard kingAttacks = 0;

	// kingAttacks = kingAttacks | ((kingBoard & ~RANK8) >> 8);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~AFILE) >> 7);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~HFILE) >> 9);
	// kingAttacks = kingAttacks | ((kingBoard & ~HFILE) >> 1);
	// kingAttacks = kingAttacks | ((kingBoard & ~AFILE) << 1);
	// kingAttacks = kingAttacks | ((kingBoard & ~RANK1) << 8);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~HFILE) << 9);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~AFILE) << 7);	
	kingAttacks = kingAttacks | (kingBoard >> 7);
	kingAttacks = kingAttacks | (kingBoard >> 8);
	kingAttacks = kingAttacks | (kingBoard >> 9);
	kingAttacks = kingAttacks | (kingBoard >> 1);
	kingAttacks = kingAttacks | (kingBoard << 1);
	kingAttacks = kingAttacks | (kingBoard << 9);
	kingAttacks = kingAttacks | (kingBoard << 8);
	kingAttacks = kingAttacks | (kingBoard << 7);
	//chessboard.printBitboard(kingAttacks);

	//can't capture own pieces
	Bitboard temp = kingAttacks & chessboard.mBoard[WHITE_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//chessboard.printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits) 
		mPossibleMoves.emplace_back(Move(Piece::KING, Player::WHITE, Square(kingBitPos[0]), Square(i)));
}

void Engine::blackKingMoves()
{
	Bitboard kingBoard = chessboard.mBoard[BLACK_KING_BOARD];
	Bitboard kingAttacks = 0;

	// kingAttacks = kingAttacks | ((kingBoard & ~RANK8) >> 8);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~AFILE) >> 7);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~HFILE) >> 9);
	// kingAttacks = kingAttacks | ((kingBoard & ~HFILE) >> 1);
	// kingAttacks = kingAttacks | ((kingBoard & ~AFILE) << 1);
	// kingAttacks = kingAttacks | ((kingBoard & ~RANK1) << 8);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~HFILE) << 9);
	// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~AFILE) << 7);
	kingAttacks = kingAttacks | (kingBoard >> 7);
	kingAttacks = kingAttacks | (kingBoard >> 8);
	kingAttacks = kingAttacks | (kingBoard >> 9);
	kingAttacks = kingAttacks | (kingBoard >> 1);
	kingAttacks = kingAttacks | (kingBoard << 1);
	kingAttacks = kingAttacks | (kingBoard << 9);
	kingAttacks = kingAttacks | (kingBoard << 8);
	kingAttacks = kingAttacks | (kingBoard << 7);
	//chessboard.printBitboard(kingAttacks);
	
	//can't capture own pieces
	Bitboard temp = kingAttacks & chessboard.mBoard[BLACK_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//chessboard.printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits) 
		mPossibleMoves.emplace_back(Move(Piece::KING, Player::BLACK, Square(kingBitPos[0]), Square(i)));
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
				//chessboard.printBitboards();
				checkIfSquaresAreAttackedByWhite(Bitboard(0));
				checkIfSquaresAreAttackedByBlack(Bitboard(0));
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
							

							auto op = checkIfenPassantPossibleOnNextMove(a,b);
							auto piece = getPieceFromSquare(a);
							chessboard.makeMove(Move(piece, chessboard.mPlayerToMove, a, b, promoted, op));
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
		// whiteKnightMoves();
		// whiteRookMoves();
		// whiteBishopMoves();
		// whiteQueenMoves();
		// whiteKingMoves();
	}else{
		blackPawnMoves();
		// blackKnightMoves();
		// blackRookMoves();
		// blackBishopMoves();
		// blackQueenMoves();
		// blackKingMoves();
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
			attackRays[bitCount][NORTH] = 0;
			for(int k = 0; k < i; k++)
			{
				uint64_t x = uint64_t(1) << bitCount;
				attackRays[bitCount][NORTH] |= (x >> ( 8*(k+1)));
			}
			//NORTH-EAST
			attackRays[bitCount][NORTH_EAST] = 0;
			int minNE = i < (7-j) ? i : (7-j);
			for(int k = 0; k < minNE; k++)
			{
				attackRays[bitCount][NORTH_EAST] |= (uint64_t(1) << ( bitCount - ( 7*(k+1) ) ));
			}
			//EAST
			attackRays[bitCount][EAST] = 0;
			for(int k = 0; k < 7-j; k++)
			{
				attackRays[bitCount][EAST] |= (uint64_t(1) << ( bitCount + ( k+1 ) ));
			}
			//SOUTH-EAST
			attackRays[bitCount][SOUTH_EAST] = 0;
			int minSE = (7-i)<(7-j) ? 7-i : 7-j;
			for(int k = 0; k < minSE; k++)
			{
				attackRays[bitCount][SOUTH_EAST] |= (uint64_t(1) << ( bitCount + ( 9*(k+1) ) ));
			}
			//SOUTH
			attackRays[bitCount][SOUTH] = 0;
			for(int k = 0; k < 7-i; k++)
			{
				attackRays[bitCount][SOUTH] |= (uint64_t(1) << ( bitCount + ( 8*(k+1) ) ));
			}
			//SOUTH-WEST
			attackRays[bitCount][SOUTH_WEST] = 0;
			int minSW = (7-i) < j ? (7-i) : j;
			for(int k = 0; k < minSW; k++)
			{
				attackRays[bitCount][SOUTH_WEST] |= (uint64_t(1) << ( bitCount + ( 7*(k+1) ) ));
			}
			//WEST
			attackRays[bitCount][WEST] = 0;
			for(int k = 0; k < j; k++)
			{
				attackRays[bitCount][WEST] |= (uint64_t(1) << ( bitCount - ( k+1 ) ));
			}
			//NORTH-WEST
			attackRays[bitCount][NORTH_WEST] = 0;
			int minNW1 = j < i ? j : i;
			int minNW2 = i < j ? i : j;
			int minNW = minNW1 > minNW2 ? minNW1 : minNW2;
			if(i == 7) minNW = j;
			for(int k = 0; k < minNW; k++)
			{
				attackRays[bitCount][NORTH_WEST] |= (uint64_t(1) << ( bitCount - ( 9*(k+1) ) ));
			}
			bitCount++;
		}
	}
	//debug print
	// Bitboard b = 0;
	// for(int i = 0; i < 64; i++)
	// {
	// 	for(int i2 = 0; i2 < 8; i2++)
	// 	{
	// 		for(int j2 = 0; j2 < 8; j2++)
	// 		{
	// 			b = (static_cast<uint64_t>(1) << (i2 * 8 + j2)) & attackRays[i][NORTH_WEST];
	// 			if (b != 0) std::cout << " X "; else std::cout << " - ";
	// 		}
	// 		std::cout << "\n";
	// 	}
	// 	std::cout << "\n\n";
	// }	
}

bool Engine::checkIfSquaresAreAttackedByWhite(const Bitboard squares)
{
	Bitboard attackedSquares = 0;

	//PAWN
	{		
		//------------CAPTURE----------------------
		Bitboard captureLeft, captureRight;
		captureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
		captureRight = captureRight >> 9;
		captureRight = captureRight & (~HFILE);
		attackedSquares |= captureRight;

		captureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
		captureLeft = captureLeft >> 7;
		captureLeft = captureLeft & (~AFILE);
		attackedSquares |= captureLeft;

		//------------ON PASSANT CAPTURE---------------
		Bitboard opCaptureLeft, opCaptureRight;
		opCaptureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
		opCaptureRight = opCaptureRight >> 9;
		opCaptureRight = opCaptureRight & (~HFILE);
		opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureRight;

		opCaptureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
		opCaptureLeft = opCaptureLeft >> 7;
		opCaptureLeft = opCaptureLeft & (~AFILE);
		opCaptureLeft = opCaptureLeft & chessboard.mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureLeft;
		
	}

	//KNIGHT
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
		
		// oneOClockAttack = oneOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// twoOClockAttack = twoOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// fourOClockAttack = fourOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// fiveOClockAttack = fiveOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// sevenOClockAttack = sevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// eightOClockAttack = eightOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// tenOClockAttack = tenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
		// elevenOClockAttack = elevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]);
				
		attackedSquares = attackedSquares | oneOClockAttack | twoOClockAttack | fourOClockAttack |
			fiveOClockAttack | sevenOClockAttack | eightOClockAttack | tenOClockAttack | elevenOClockAttack;
	}

	//ROOK
	{
		auto rookBits = getBitsPosition(chessboard.mBoard[WHITE_ROOKS_BOARD]);
		for (auto rookBitPos : rookBits) //for every white rook on the board
		{
			Bitboard rookAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[rookBitPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[rookBitPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//EAST ATTACK
				temp = attackRays[rookBitPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[rookBitPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[rookBitPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[rookBitPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//WEST ATTACK
				temp = attackRays[rookBitPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[rookBitPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			
			attackedSquares |= rookAttack;
		}
	}

	//BISHOP
	{
		auto bishopBits = getBitsPosition(chessboard.mBoard[WHITE_BISHOPS_BOARD]);
		for (auto bishopBitPos : bishopBits) //for every white rook on the board
		{
			Bitboard bishopAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH-EAST ATTACK
				temp = attackRays[bishopBitPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[bishopBitPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			
			attackedSquares |= bishopAttack;
		}
	}

	//QUEEN
	{
		auto queenBits = getBitsPosition(chessboard.mBoard[WHITE_QUEEN_BOARD]);
		for (auto queenBitsPos : queenBits) //for every white rook on the board
		{
			Bitboard queenAttacks = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[queenBitsPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[queenBitsPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//EAST ATTACK
				temp = attackRays[queenBitsPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[queenBitsPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[queenBitsPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[queenBitsPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//WEST ATTACK
				temp = attackRays[queenBitsPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[queenBitsPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//NORTH-EAST ATTACK
				temp = attackRays[queenBitsPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[queenBitsPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			
			attackedSquares |= queenAttacks;
		}
	}

	//KING
	{
		Bitboard kingBoard = chessboard.mBoard[WHITE_KING_BOARD];
		Bitboard kingAttacks = 0;

		kingAttacks = kingAttacks | (kingBoard >> 7);
		kingAttacks = kingAttacks | (kingBoard >> 8);
		kingAttacks = kingAttacks | (kingBoard >> 9);
		kingAttacks = kingAttacks | (kingBoard >> 1);
		kingAttacks = kingAttacks | (kingBoard << 1);
		kingAttacks = kingAttacks | (kingBoard << 9);
		kingAttacks = kingAttacks | (kingBoard << 8);
		kingAttacks = kingAttacks | (kingBoard << 7);
		//chessboard.printBitboard(kingAttacks);

		//can't capture own pieces
		Bitboard temp = kingAttacks & chessboard.mBoard[WHITE_PIECES_BOARD];
		kingAttacks = kingAttacks & ~temp;
		//chessboard.printBitboard(kingAttacks);
		
		attackedSquares |= kingAttacks;
	}

	//chessboard.printBitboard(attackedSquares);
	if(attackedSquares & squares) return true;
	else return false;
}

bool Engine::checkIfSquaresAreAttackedByBlack(const Bitboard squares)
{
	Bitboard attackedSquares = 0;

	//PAWN
	{
		//------------CAPTURE----------------------
		Bitboard captureLeft, captureRight;
		captureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
		captureRight = captureRight << 7;
		captureRight = captureRight & (~HFILE);
		attackedSquares |= captureRight;

		captureLeft = chessboard.mBoard[BLACK_PAWNS_BOARD];
		captureLeft = captureLeft << 9;
		captureLeft = captureLeft & (~AFILE);
		attackedSquares |= captureLeft;

		//------------ON PASSANT CAPTURE---------------
		Bitboard opCaptureLeft, opCaptureRight;
		opCaptureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
		opCaptureRight = opCaptureRight << 7;
		opCaptureRight = opCaptureRight & (~HFILE);
		opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureRight;

		opCaptureLeft = chessboard.mBoard[BLACK_PAWNS_BOARD];
		opCaptureLeft = opCaptureLeft << 9;
		opCaptureLeft = opCaptureLeft & (~AFILE);
		opCaptureLeft = opCaptureLeft & chessboard.mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureLeft;
	}

	//KNIGHT
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

		attackedSquares = attackedSquares | oneOClockAttack | twoOClockAttack | fourOClockAttack |
			fiveOClockAttack | sevenOClockAttack | eightOClockAttack | tenOClockAttack | elevenOClockAttack;
	}

	//ROOK
	{
		auto rookBits = getBitsPosition(chessboard.mBoard[BLACK_ROOKS_BOARD]);
		for (auto rookBitPos : rookBits) //for every white rook on the board
		{
			Bitboard rookAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[rookBitPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[rookBitPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//EAST ATTACK
				temp = attackRays[rookBitPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[rookBitPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[rookBitPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[rookBitPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			{
				//WEST ATTACK
				temp = attackRays[rookBitPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[rookBitPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(rookAttack);
			}
			attackedSquares |= rookAttack;
		}
	}

	//BISHOP
	{
		auto bishopBits = getBitsPosition(chessboard.mBoard[BLACK_BISHOPS_BOARD]);
		for (auto bishopBitPos : bishopBits) //for every white rook on the board
		{
			Bitboard bishopAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH-EAST ATTACK
				temp = attackRays[bishopBitPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[bishopBitPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(bishopAttack);
			}
			
			attackedSquares |= bishopAttack;
		}
	}

	//QUEEN
	{
		auto queenBits = getBitsPosition(chessboard.mBoard[BLACK_QUEEN_BOARD]);
		for (auto queenBitsPos : queenBits) //for every white rook on the board
		{
			Bitboard queenAttacks = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[queenBitsPos][NORTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[queenBitsPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//EAST ATTACK
				temp = attackRays[queenBitsPos][EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[queenBitsPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[queenBitsPos][SOUTH] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[queenBitsPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//WEST ATTACK
				temp = attackRays[queenBitsPos][WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[queenBitsPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//NORTH-EAST ATTACK
				temp = attackRays[queenBitsPos][NORTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_EAST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[queenBitsPos][NORTH_WEST] & chessboard.mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= chessboard.mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//chessboard.printBitboard(queenAttacks);
			}
			
			attackedSquares |= queenAttacks;
		}
	}

	//KING
	{
		Bitboard kingBoard = chessboard.mBoard[BLACK_KING_BOARD];
		Bitboard kingAttacks = 0;

		// kingAttacks = kingAttacks | ((kingBoard & ~RANK8) >> 8);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~AFILE) >> 7);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~HFILE) >> 9);
		// kingAttacks = kingAttacks | ((kingBoard & ~HFILE) >> 1);
		// kingAttacks = kingAttacks | ((kingBoard & ~AFILE) << 1);
		// kingAttacks = kingAttacks | ((kingBoard & ~RANK1) << 8);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~HFILE) << 9);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~AFILE) << 7);
		kingAttacks = kingAttacks | (kingBoard >> 7);
		kingAttacks = kingAttacks | (kingBoard >> 8);
		kingAttacks = kingAttacks | (kingBoard >> 9);
		kingAttacks = kingAttacks | (kingBoard >> 1);
		kingAttacks = kingAttacks | (kingBoard << 1);
		kingAttacks = kingAttacks | (kingBoard << 9);
		kingAttacks = kingAttacks | (kingBoard << 8);
		kingAttacks = kingAttacks | (kingBoard << 7);
		//chessboard.printBitboard(kingAttacks);
		
		//can't capture own pieces
		Bitboard temp = kingAttacks & chessboard.mBoard[BLACK_PIECES_BOARD];
		kingAttacks = kingAttacks & ~temp;
		//chessboard.printBitboard(kingAttacks);

		attackedSquares |= kingAttacks;
	}

	//chessboard.printBitboard(attackedSquares);
	if(attackedSquares & squares) return true;
	else return false;
}

