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
	for(i = 2; i<13; i+=2)
	{
		p = chessboard.mBoard[i+chessboard.mPlayerToMove];
		if(p & pieceBoard) return Piece(i);
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
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}
	//------------EN PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureRight = opCaptureRight >> 9;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i));
		chessboard.setMoveCapture(m);
		m.enPassantCapture = true;
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	opCaptureLeft = chessboard.mBoard[WHITE_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft >> 7;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i));
		chessboard.setMoveCapture(m);
		m.enPassantCapture = true;
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	capturePromotionR = capturePromotionR >> 9;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::QUEEN);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::ROOK);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::KNIGHT);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::BISHOP);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL >> 7;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::QUEEN);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::ROOK);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::KNIGHT);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::BISHOP);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	//------------EN PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = chessboard.mBoard[BLACK_PAWNS_BOARD];
	opCaptureRight = opCaptureRight << 7;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & chessboard.mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i));
		m.enPassantCapture = true;
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		m.enPassantCapture = true;
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::ROOK);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::KNIGHT);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::BISHOP);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	capturePromotionR = capturePromotionR << 7;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{	
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::QUEEN);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		
		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::ROOK);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::KNIGHT);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::BISHOP);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL << 9;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & chessboard.mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::QUEEN);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::ROOK);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::KNIGHT);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::BISHOP);
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
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
	for (auto &i : oneOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 15), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 6), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();		
	}
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 10), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();		
	}

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 17), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 15), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}
	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 6), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 10), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 17), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

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
	for (auto &i : oneOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 15), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 6), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();		
	}
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 10), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();				
	}

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 17), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();						
	}

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 15), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 6), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();		
	}

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 10), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();				
	}

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~chessboard.mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 17), Square(i));
		chessboard.setMoveCapture(m);
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
	}
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
		{
			auto m = Move(Piece::ROOK, Player::WHITE, Square(rookBitPos), Square(i));
			chessboard.setMoveCapture(m);
			auto a = chessboard.mCastlingAvailable[1];
			auto c = chessboard.mCastlingAvailable[3];
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
			chessboard.mCastlingAvailable[1] = a;
			chessboard.mCastlingAvailable[3] = c;
		}
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
		{
			auto m = Move(Piece::ROOK, Player::BLACK, Square(rookBitPos), Square(i));
			chessboard.setMoveCapture(m);
			auto b = chessboard.mCastlingAvailable[2];
			auto d = chessboard.mCastlingAvailable[4];
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
			chessboard.mCastlingAvailable[2] = b;
			chessboard.mCastlingAvailable[4] = d;
		}
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
		{
			auto m = Move(Piece::BISHOP, Player::WHITE, Square(bishopBitPos), Square(i));
			chessboard.setMoveCapture(m);
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
		}			
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
		{
			auto m = Move(Piece::BISHOP, Player::BLACK, Square(bishopBitPos), Square(i));
			chessboard.setMoveCapture(m);
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
		}
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
		{
			auto m = Move(Piece::QUEEN, Player::WHITE, Square(queenBitsPos), Square(i));
			chessboard.setMoveCapture(m);
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
		}			
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
		{
			auto m = Move(Piece::QUEEN, Player::BLACK, Square(queenBitsPos), Square(i));
			chessboard.setMoveCapture(m);
			chessboard.makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			chessboard.unmakeMove();
		}			
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
	
	kingAttacks |= ((kingBoard >> 7) & ~(RANK1 | AFILE));//north-east
	kingAttacks |= ((kingBoard >> 8) & ~(RANK1));//north
	kingAttacks |= ((kingBoard >> 9) & ~(RANK1 | HFILE));//north-west
	kingAttacks |= ((kingBoard >> 1) & ~(HFILE));//west
	kingAttacks |= ((kingBoard << 1) & ~(AFILE));//east
	kingAttacks |= ((kingBoard << 9) & ~(RANK8 | AFILE));//south-east
	kingAttacks |= ((kingBoard << 8) & ~(RANK8));//south
	kingAttacks |= ((kingBoard << 7) & ~(RANK8 | HFILE));//south-west
	//chessboard.printBitboard(kingAttacks);

	//can't capture own pieces
	Bitboard temp = kingAttacks & chessboard.mBoard[WHITE_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//chessboard.printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits)
	{
		auto m = Move(Piece::KING, Player::WHITE, Square(kingBitPos[0]), Square(i));
		chessboard.setMoveCapture(m);
		auto a = chessboard.mCastlingAvailable[1];
		auto b = chessboard.mCastlingAvailable[3];
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(chessboard.mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		chessboard.mCastlingAvailable[1] = a;
		chessboard.mCastlingAvailable[3] = b;
	}

	if(chessboard.mCastlingAvailable[1])//kingside castling
	{
		Bitboard squares = 8070450532247928832;
		if(((squares & ~kingBoard) & chessboard.mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByBlack(squares))
			{
				auto m = Move(Piece::KING, Player::WHITE, Square::E1, Square::G1);
				m.mCastle = Castling::KINGSIDE;
				mPossibleMoves.emplace_back(m);
			}
		}
	}
	if(chessboard.mCastlingAvailable[3])//queenside castling
	{
		Bitboard squares = 2161727821137838080;
		if(((squares & ~kingBoard) & chessboard.mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByBlack(squares & 18302628885633564671ULL))
			{
				auto m = Move(Piece::KING, Player::WHITE, Square::E1, Square::C1);
				m.mCastle = Castling::QUEENSIDE;
				mPossibleMoves.emplace_back(m);
			}
		}
	}
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
	kingAttacks |= ((kingBoard >> 7) & ~(RANK1 | AFILE));//north-east
	kingAttacks |= ((kingBoard >> 8) & ~(RANK1));//north
	kingAttacks |= ((kingBoard >> 9) & ~(RANK1 | HFILE));//north-west
	kingAttacks |= ((kingBoard >> 1) & ~(HFILE));//west
	kingAttacks |= ((kingBoard << 1) & ~(AFILE));//east
	kingAttacks |= ((kingBoard << 9) & ~(RANK8 | AFILE));//south-east
	kingAttacks |= ((kingBoard << 8) & ~(RANK8));//south
	kingAttacks |= ((kingBoard << 7) & ~(RANK8 | HFILE));//south-west
	//chessboard.printBitboard(kingAttacks);
	
	//can't capture own pieces
	Bitboard temp = kingAttacks & chessboard.mBoard[BLACK_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//chessboard.printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits)
	{
		auto m = Move(Piece::KING, Player::BLACK, Square(kingBitPos[0]), Square(i));
		chessboard.setMoveCapture(m);
		auto a = chessboard.mCastlingAvailable[2];
		auto b = chessboard.mCastlingAvailable[4];
		chessboard.makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(chessboard.mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		chessboard.unmakeMove();
		chessboard.mCastlingAvailable[2] = a;
		chessboard.mCastlingAvailable[4] = b;
	}

	if(chessboard.mCastlingAvailable[2])//kingside castling
	{
		Bitboard squares = 112;
		if(((squares & ~kingBoard) & chessboard.mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByWhite(squares))
			{
				auto m = Move(Piece::KING, Player::BLACK, Square::E8, Square::G8);
				m.mCastle = Castling::KINGSIDE;
				mPossibleMoves.emplace_back(m);
			}
		}
	}
	if(chessboard.mCastlingAvailable[4])//queenside castling
	{
		Bitboard squares = 14;
		if((squares & chessboard.mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByWhite(Bitboard(28)))
			{
				auto m = Move(Piece::KING, Player::BLACK, Square::E8, Square::C8);
				m.mCastle = Castling::QUEENSIDE;
				mPossibleMoves.emplace_back(m);
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
							

							auto op = checkIfenPassantPossibleOnNextMove(a,b);
							auto piece = getPieceFromSquare(a);

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
		whiteKnightMoves();
		whiteRookMoves();
		whiteBishopMoves();
		whiteQueenMoves();
		whiteKingMoves();
	}else{
		blackPawnMoves();
		blackKnightMoves();
		blackRookMoves();
		blackBishopMoves();
		blackQueenMoves();
		blackKingMoves();
	}
	std::cout << "bestmove ";
	auto move = mPossibleMoves[getRandomNumber(0, mPossibleMoves.size()-1)];
	move.printMove();
	//chessboard.makeMove(move);
}
