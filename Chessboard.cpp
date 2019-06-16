#include <cmath>
#include "Chessboard.h"
#include "definitions.h"

inline void setPieceInBoard(uint64_t &piecePosition, Bitboard &board)
{
	Bitboard b = static_cast<uint64_t>(1) << piecePosition;
	board = board | b;
}

Chessboard::Chessboard()
{
	mBoard.reserve(18);
	for (int i = 0; i < 18; i++) { mBoard.push_back(0); }
}

void Chessboard::setIfEnPassantCapture(Move &move)
{
	Bitboard moveToSquare = Bitboard(1) << move.mMoveTo;
	auto otherSide = (move.mSide + 1) % 2;
	if(!(moveToSquare & mBoard[WHITE_PIECES_BOARD+otherSide]))
		move.enPassantCapture = true;
}

void Chessboard::setMoveCapture(Move &move)
{
	//Bitboard moveFromSquare = Bitboard(1) << move.mMoveFrom;
	Bitboard moveToSquare = Bitboard(1) << move.mMoveTo;
	auto otherSide = (move.mSide + 1) % 2;

	int i;
	for(i = 2; i < 13; i+=2)
	{
		//auto n = chessboard.mBoard[i+move.mSide] & moveToSquare;
		if(mBoard[i+otherSide] & moveToSquare)
		{
			//auto x = getBitsPosition(n);
			//move.mCaptured = Piece(x[0]);
			move.mCaptured = Piece(i);
			move.mScore = PIECES_VALUES[i];
			return;
		}

	}
}

bool Chessboard::checkIfSquaresAreAttackedByWhite(const Bitboard squares)
{
	Bitboard attackedSquares = 0;

	//PAWN
	{		
		//------------CAPTURE----------------------
		Bitboard captureLeft, captureRight;
		captureRight = mBoard[WHITE_PAWNS_BOARD];
		captureRight = captureRight >> 9;
		captureRight &= ~HFILE;
		captureRight &= mBoard[BLACK_PIECES_BOARD];//can't capture own pieces
		attackedSquares |= captureRight;

		captureLeft = mBoard[WHITE_PAWNS_BOARD];
		captureLeft = captureLeft >> 7;
		captureLeft &= ~AFILE;
		captureLeft &= mBoard[BLACK_PIECES_BOARD];//can't capture own pieces
		attackedSquares |= captureLeft;

		//------------ON PASSANT CAPTURE---------------
		Bitboard opCaptureLeft, opCaptureRight;
		opCaptureRight = mBoard[WHITE_PAWNS_BOARD];
		opCaptureRight = opCaptureRight >> 9;
		opCaptureRight &= ~HFILE;
		opCaptureRight &= mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureRight;

		opCaptureLeft = mBoard[WHITE_PAWNS_BOARD];
		opCaptureLeft = opCaptureLeft >> 7;
		opCaptureLeft &= ~AFILE;
		opCaptureLeft &= mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureLeft;
		
	}

	//KNIGHT
	{
		Bitboard knightsBoard = mBoard[WHITE_KNIGHTS_BOARD];
		Bitboard elevenOClockAttack = knightsBoard >> 17;
		Bitboard tenOClockAttack = knightsBoard >> 10;
		Bitboard eightOClockAttack = knightsBoard << 6;
		Bitboard sevenOClockAttack = knightsBoard << 15;
		Bitboard fiveOClockAttack = knightsBoard << 17;
		Bitboard fourOClockAttack = knightsBoard << 10;
		Bitboard twoOClockAttack = knightsBoard >> 6;
		Bitboard oneOClockAttack = knightsBoard >> 15;

		//oneOClockAttack can't land on RANK1 or RANK2 or AFILE
		oneOClockAttack &= ~(RANK1 | RANK2 | AFILE);
		twoOClockAttack &= ~(AFILE | RANK1 | BFILE);
		fourOClockAttack &= ~(AFILE | RANK8 | BFILE);
		fiveOClockAttack &= ~(AFILE | RANK8 | RANK7);
		sevenOClockAttack &= ~(HFILE | RANK8 | RANK7);
		eightOClockAttack &= ~(HFILE | RANK8 | GFILE);
		tenOClockAttack &= ~(HFILE | RANK1 | GFILE);
		elevenOClockAttack &= ~(HFILE | RANK1 | RANK2);
							
		attackedSquares = attackedSquares | oneOClockAttack | twoOClockAttack | fourOClockAttack |
			fiveOClockAttack | sevenOClockAttack | eightOClockAttack | tenOClockAttack | elevenOClockAttack;
		attackedSquares = attackedSquares & ~mBoard[WHITE_PIECES_BOARD];
	}

	//ROOK
	{
		auto rookBits = getBitsPosition(mBoard[WHITE_ROOKS_BOARD]);
		for (auto rookBitPos : rookBits) //for every white rook on the board
		{
			Bitboard rookAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[rookBitPos][NORTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[rookBitPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//EAST ATTACK
				temp = attackRays[rookBitPos][EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[rookBitPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[rookBitPos][SOUTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[rookBitPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//WEST ATTACK
				temp = attackRays[rookBitPos][WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[rookBitPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}			
			attackedSquares |= rookAttack;
		}
	}

	//BISHOP
	{
		auto bishopBits = getBitsPosition(mBoard[WHITE_BISHOPS_BOARD]);
		for (auto bishopBitPos : bishopBits) //for every white rook on the board
		{
			Bitboard bishopAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH-EAST ATTACK
				temp = attackRays[bishopBitPos][NORTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[bishopBitPos][NORTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			
			attackedSquares |= bishopAttack;
		}
	}

	//QUEEN
	{
		auto queenBits = getBitsPosition(mBoard[WHITE_QUEEN_BOARD]);
		for (auto queenBitsPos : queenBits) //for every white rook on the board
		{
			Bitboard queenAttacks = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[queenBitsPos][NORTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[queenBitsPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//EAST ATTACK
				temp = attackRays[queenBitsPos][EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[queenBitsPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[queenBitsPos][SOUTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[queenBitsPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//WEST ATTACK
				temp = attackRays[queenBitsPos][WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[queenBitsPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//NORTH-EAST ATTACK
				temp = attackRays[queenBitsPos][NORTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[queenBitsPos][NORTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[WHITE_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			
			attackedSquares |= queenAttacks;
		}
	}

	//KING
	{
		Bitboard kingBoard = mBoard[WHITE_KING_BOARD];
		Bitboard kingAttacks = 0;

		kingAttacks |= ((kingBoard >> 7) & ~(RANK1 | AFILE));//north-east
		kingAttacks |= ((kingBoard >> 8) & ~(RANK1));//north
		kingAttacks |= ((kingBoard >> 9) & ~(RANK1 | HFILE));//north-west
		kingAttacks |= ((kingBoard >> 1) & ~(HFILE));//west
		kingAttacks |= ((kingBoard << 1) & ~(AFILE));//east
		kingAttacks |= ((kingBoard << 9) & ~(RANK8 | AFILE));//south-east
		kingAttacks |= ((kingBoard << 8) & ~(RANK8));//south
		kingAttacks |= ((kingBoard << 7) & ~(RANK8 | HFILE));//south-west
		//printBitboard(kingAttacks);

		//can't capture own pieces
		Bitboard temp = kingAttacks & mBoard[WHITE_PIECES_BOARD];
		kingAttacks &= ~temp;
		//printBitboard(kingAttacks);
		
		attackedSquares |= kingAttacks;
	}

	//printBitboard(attackedSquares);
	if(attackedSquares & squares) return true;
	else return false;
}

bool Chessboard::checkIfSquaresAreAttackedByBlack(const Bitboard squares)
{
	Bitboard attackedSquares = 0;

	//PAWN
	{
		//------------CAPTURE----------------------
		Bitboard captureLeft, captureRight;
		captureRight = mBoard[BLACK_PAWNS_BOARD];
		captureRight = captureRight << 7;
		captureRight &= ~HFILE;
		captureRight &= mBoard[WHITE_PIECES_BOARD];//can't capture own pieces
		attackedSquares |= captureRight;

		captureLeft = mBoard[BLACK_PAWNS_BOARD];
		captureLeft = captureLeft << 9;
		captureLeft &= ~AFILE;
		captureLeft &= mBoard[WHITE_PIECES_BOARD];//can't capture own pieces
		attackedSquares |= captureLeft;

		//------------ON PASSANT CAPTURE---------------
		Bitboard opCaptureLeft, opCaptureRight;
		opCaptureRight = mBoard[BLACK_PAWNS_BOARD];
		opCaptureRight = opCaptureRight << 7;
		opCaptureRight &= ~HFILE;
		opCaptureRight &= mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureRight;

		opCaptureLeft = mBoard[BLACK_PAWNS_BOARD];
		opCaptureLeft = opCaptureLeft << 9;
		opCaptureLeft &= ~AFILE;
		opCaptureLeft &= mBoard[ENPASSANT_BOARD];
		attackedSquares |= opCaptureLeft;
	}

	//KNIGHT
	{
		Bitboard knightsBoard = mBoard[BLACK_KNIGHTS_BOARD];
		Bitboard elevenOClockAttack = knightsBoard >> 17;
		Bitboard tenOClockAttack = knightsBoard >> 10;
		Bitboard eightOClockAttack = knightsBoard << 6;
		Bitboard sevenOClockAttack = knightsBoard << 15;
		Bitboard fiveOClockAttack = knightsBoard << 17;
		Bitboard fourOClockAttack = knightsBoard << 10;
		Bitboard twoOClockAttack = knightsBoard >> 6;
		Bitboard oneOClockAttack = knightsBoard >> 15;

		//oneOClockAttack can't land on RANK1 or RANK2 or AFILE
		oneOClockAttack &= ~(RANK1 | RANK2 | AFILE);
		twoOClockAttack &= ~(AFILE | RANK1 | BFILE);
		fourOClockAttack &= ~(AFILE | RANK8 | BFILE);
		fiveOClockAttack &= ~(AFILE | RANK8 | RANK7);
		sevenOClockAttack &= ~(HFILE | RANK8 | RANK7);
		eightOClockAttack &= ~(HFILE | RANK8 | GFILE);
		tenOClockAttack &= ~(HFILE | RANK1 | GFILE);
		elevenOClockAttack &= ~(HFILE | RANK1 | RANK2);
							
		attackedSquares = attackedSquares | oneOClockAttack | twoOClockAttack | fourOClockAttack |
			fiveOClockAttack | sevenOClockAttack | eightOClockAttack | tenOClockAttack | elevenOClockAttack;
		attackedSquares = attackedSquares & ~mBoard[BLACK_PIECES_BOARD];
	}

	//ROOK
	{
		auto rookBits = getBitsPosition(mBoard[BLACK_ROOKS_BOARD]);
		for (auto rookBitPos : rookBits) //for every white rook on the board
		{
			Bitboard rookAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[rookBitPos][NORTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[rookBitPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//EAST ATTACK
				temp = attackRays[rookBitPos][EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[rookBitPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[rookBitPos][SOUTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[rookBitPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			{
				//WEST ATTACK
				temp = attackRays[rookBitPos][WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[rookBitPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				rookAttack = rookAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(rookAttack);
			}
			attackedSquares |= rookAttack;
		}
	}

	//BISHOP
	{
		auto bishopBits = getBitsPosition(mBoard[BLACK_BISHOPS_BOARD]);
		for (auto bishopBitPos : bishopBits) //for every white rook on the board
		{
			Bitboard bishopAttack = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH-EAST ATTACK
				temp = attackRays[bishopBitPos][NORTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[bishopBitPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[bishopBitPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[bishopBitPos][SOUTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[bishopBitPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[bishopBitPos][NORTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[bishopBitPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				bishopAttack = bishopAttack | (temp & ~firstBlockerBitboard);
				//printBitboard(bishopAttack);
			}
			
			attackedSquares |= bishopAttack;
		}
	}

	//QUEEN
	{
		auto queenBits = getBitsPosition(mBoard[BLACK_QUEEN_BOARD]);
		for (auto queenBitsPos : queenBits) //for every white rook on the board
		{
			Bitboard queenAttacks = 0, temp;
			Square firstBlocker;
			
			{
				//NORTH ATTACK
				temp = attackRays[queenBitsPos][NORTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH];
				temp = attackRays[queenBitsPos][NORTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//EAST ATTACK
				temp = attackRays[queenBitsPos][EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][EAST];
				temp = attackRays[queenBitsPos][EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH ATTACK
				temp = attackRays[queenBitsPos][SOUTH] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH];
				temp = attackRays[queenBitsPos][SOUTH] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//WEST ATTACK
				temp = attackRays[queenBitsPos][WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][WEST];
				temp = attackRays[queenBitsPos][WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//NORTH-EAST ATTACK
				temp = attackRays[queenBitsPos][NORTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_EAST];
				temp = attackRays[queenBitsPos][NORTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH-EAST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_EAST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_EAST];
				temp = attackRays[queenBitsPos][SOUTH_EAST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//SOUTH-WEST ATTACK
				temp = attackRays[queenBitsPos][SOUTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getLowestSetBit(temp);
				temp = attackRays[firstBlocker][SOUTH_WEST];
				temp = attackRays[queenBitsPos][SOUTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			{
				//NORTH-WEST ATTACK
				temp = attackRays[queenBitsPos][NORTH_WEST] & mBoard[ALL_PIECES_BOARD];
				firstBlocker = getHighestSetBit(temp);
				temp = attackRays[firstBlocker][NORTH_WEST];
				temp = attackRays[queenBitsPos][NORTH_WEST] & ~temp;

				//can't capture own pieces
				Bitboard firstBlockerBitboard = uint64_t(1) << firstBlocker;
				firstBlockerBitboard &= mBoard[BLACK_PIECES_BOARD];
				queenAttacks = queenAttacks | (temp & ~firstBlockerBitboard);
				//printBitboard(queenAttacks);
			}
			
			attackedSquares |= queenAttacks;
		}
	}

	//KING
	{
		Bitboard kingBoard = mBoard[BLACK_KING_BOARD];
		Bitboard kingAttacks = 0;

		kingAttacks |= ((kingBoard >> 7) & ~(RANK1 | AFILE));//north-east
		kingAttacks |= ((kingBoard >> 8) & ~(RANK1));//north
		kingAttacks |= ((kingBoard >> 9) & ~(RANK1 | HFILE));//north-west
		kingAttacks |= ((kingBoard >> 1) & ~(HFILE));//west
		kingAttacks |= ((kingBoard << 1) & ~(AFILE));//east
		kingAttacks |= ((kingBoard << 9) & ~(RANK8 | AFILE));//south-east
		kingAttacks |= ((kingBoard << 8) & ~(RANK8));//south
		kingAttacks |= ((kingBoard << 7) & ~(RANK8 | HFILE));//south-west
		// kingAttacks = kingAttacks | ((kingBoard & ~RANK8) >> 8);//north
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~AFILE) >> 7);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK8) & ~HFILE) >> 9);
		// kingAttacks = kingAttacks | ((kingBoard & ~HFILE) >> 1);
		// kingAttacks = kingAttacks | ((kingBoard & ~AFILE) << 1);
		// kingAttacks = kingAttacks | ((kingBoard & ~RANK1) << 8);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~HFILE) << 9);
		// kingAttacks = kingAttacks | (((kingBoard & ~RANK1) & ~AFILE) << 7);		
		//printBitboard(kingAttacks);
		
		//can't capture own pieces
		Bitboard temp = kingAttacks & mBoard[BLACK_PIECES_BOARD];
		kingAttacks = kingAttacks & ~temp;
		//printBitboard(kingAttacks);

		attackedSquares |= kingAttacks;
	}

	//printBitboard(attackedSquares);
	if(attackedSquares & squares) return true;
	else return false;
}

void Chessboard::setState(const std::string & FENstring)
{
	reset();
	//note: check to see if its a valid FENstring!!!
	std::string state = FENstring;
	state.erase(state.find(' ')); //we only need the pieces locations for now
	uint64_t bitPosition = 0;
	for (unsigned char c : state)
	{
		if (bitPosition == 64) break;
		if (c == '/') continue;
		if (std::isdigit(c) == 0) //not numeric
		{
			switch (c) {
			case 'p': setPieceInBoard(bitPosition, mBoard[BLACK_PAWNS_BOARD]); break;
			case 'r': setPieceInBoard(bitPosition, mBoard[BLACK_ROOKS_BOARD]); break;
			case 'n': setPieceInBoard(bitPosition, mBoard[BLACK_KNIGHTS_BOARD]); break;
			case 'b': setPieceInBoard(bitPosition, mBoard[BLACK_BISHOPS_BOARD]); break;
			case 'q': setPieceInBoard(bitPosition, mBoard[BLACK_QUEEN_BOARD]); break;
			case 'k': setPieceInBoard(bitPosition, mBoard[BLACK_KING_BOARD]); break;
			case 'P': setPieceInBoard(bitPosition, mBoard[WHITE_PAWNS_BOARD]); break;
			case 'R': setPieceInBoard(bitPosition, mBoard[WHITE_ROOKS_BOARD]); break;
			case 'N': setPieceInBoard(bitPosition, mBoard[WHITE_KNIGHTS_BOARD]); break;
			case 'B': setPieceInBoard(bitPosition, mBoard[WHITE_BISHOPS_BOARD]); break;
			case 'Q': setPieceInBoard(bitPosition, mBoard[WHITE_QUEEN_BOARD]); break;
			case 'K': setPieceInBoard(bitPosition, mBoard[WHITE_KING_BOARD]); break;
			}
			bitPosition++;
		}
		else { //numeric
			int n = std::stoi(std::string{static_cast<char>(c)});
			//std::cout << n;
			bitPosition += n;
		}		
	}
	mBoard[WHITE_PIECES_BOARD] = mBoard[WHITE_PAWNS_BOARD] | mBoard[WHITE_ROOKS_BOARD] | mBoard[WHITE_KNIGHTS_BOARD] |
		mBoard[WHITE_BISHOPS_BOARD] | mBoard[WHITE_QUEEN_BOARD] | mBoard[WHITE_KING_BOARD];
	mBoard[BLACK_PIECES_BOARD] = mBoard[BLACK_PAWNS_BOARD] | mBoard[BLACK_ROOKS_BOARD] | mBoard[BLACK_KNIGHTS_BOARD] |
		mBoard[BLACK_BISHOPS_BOARD] | mBoard[BLACK_QUEEN_BOARD] | mBoard[BLACK_KING_BOARD];
	mBoard[ALL_PIECES_BOARD] = mBoard[WHITE_PIECES_BOARD] | mBoard[BLACK_PIECES_BOARD];
}

void Chessboard::printBitboard(const Bitboard bitboard)
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
	std::cout << "\n\n";
}

void Chessboard::printBoard()
{
	Bitboard b;
	for (uint64_t i = 0; i < 8; i++)
	{
		std::cout << 8-i << "|";
		for (uint64_t j = 0; j < 8; j++)
		{
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_PAWNS_BOARD]; if (b != 0) { std::cout << " p "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_ROOKS_BOARD]; if (b != 0) { std::cout << " r "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_KNIGHTS_BOARD]; if (b != 0) { std::cout << " n "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_BISHOPS_BOARD]; if (b != 0) { std::cout << " b "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_QUEEN_BOARD]; if (b != 0) { std::cout << " q "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[BLACK_KING_BOARD]; if (b != 0) { std::cout << " k "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_PAWNS_BOARD]; if (b != 0) { std::cout << " P "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_ROOKS_BOARD]; if (b != 0) { std::cout << " R "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_KNIGHTS_BOARD]; if (b != 0) { std::cout << " N "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_BISHOPS_BOARD]; if (b != 0) { std::cout << " B "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_QUEEN_BOARD]; if (b != 0) { std::cout << " Q "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[WHITE_KING_BOARD]; if (b != 0) { std::cout << " K "; continue; }
			b = (uint64_t(1) << (i * 8 + j)) & mBoard[ALL_PIECES_BOARD]; if (b == 0) std::cout << " - ";
			else 
			std::cout << " ? ";
		}
		std::cout << "\n";
	}
	std::cout << "  ------------------------\n   a  b  c  d  e  f  g  h\n";
	//printBitboads();
	std::cout << "\ncastling available K:" << mCastlingAvailable[1] << 
		"  Q:" << mCastlingAvailable[3] << "  k:" << mCastlingAvailable[2] <<
		"  q:" << mCastlingAvailable[4] << "\n";
}

void Chessboard::printBitboards()
{
	std::vector<std::string> names = {
		"WHITE_PAWNS_BOARD",
		"BLACK_PAWNS_BOARD",
		"WHITE_KNIGHTS_BOARD",
		"BLACK_KNIGHTS_BOARD",
		"WHITE_BISHOPS_BOARD",
		"BLACK_BISHOPS_BOARD",
		"WHITE_ROOKS_BOARD",
		"BLACK_ROOKS_BOARD",
		"WHITE_QUEEN_BOARD", 
		"BLACK_QUEEN_BOARD", 
		"WHITE_KING_BOARD",
		"BLACK_KING_BOARD",
		"WHITE_PIECES_BOARD",
		"BLACK_PIECES_BOARD",
		"ALL_PIECES_BOARD",
		"ENPASSANT_BOARD"
	};
	for(int n = 2; n< mBoard.size();n++)
	{
		int count = 0;
		Bitboard b;
		std::cout << names[n-2] << "\n";
		for (uint64_t i = 0; i < 8; i++)
		{
			std::cout << 8-i << "|";
			for (uint64_t j = 0; j < 8; j++)
			{
				b = (uint64_t(1) << (i * 8 + j)) & mBoard[n]; if (b != 0) { std::cout << " X "; continue; }
				std::cout << " - ";
			}
			std::cout << "\n";
		}
		std::cout << "  ------------------------\n   a  b  c  d  e  f  g  h\n\n";
	}
}

void Chessboard::reset()
{
	mPlayerToMove = WHITE;
	std::fill(mBoard.begin(), mBoard.end(), 0); //reset board
	mCastlingAvailable[1] = 1;
	mCastlingAvailable[2] = 1;
	mCastlingAvailable[3] = 1;
	mCastlingAvailable[4] = 1;
	mKingNotMoved[1] = 1;
	mKingNotMoved[2] = 1;
	//std::queue<Move> empty;
	//std::swap(mHistory, empty);	
	mHistory.clear();
}

void Chessboard::makeMove(const Move &move)
{
	Bitboard moveFromSquare = Bitboard(1) << move.mMoveFrom;
	Bitboard moveToSquare = Bitboard(1) << move.mMoveTo;
	auto otherSide = (move.mSide + 1) % 2;

	//determines which type of castling is available
	// auto rook = mBoard[WHITE_ROOKS_BOARD + move.mSide] & moveFromSquare;
	// auto king = mBoard[WHITE_KING_BOARD + move.mSide] & moveFromSquare;
	// mKingNotMoved[KING_CASTLE[king & EFILE & RANK1]] = 0;
	// mKingNotMoved[KING_CASTLE[king & EFILE & RANK8]] = 0;
	
	//determines if and which rook moved for the first time
	// mCastlingAvailable[ROOK_CASTLE[rook & RANK1 & AFILE]] = 0;
	// mCastlingAvailable[ROOK_CASTLE[rook & RANK1 & HFILE]] = 0;
	// mCastlingAvailable[ROOK_CASTLE[rook & RANK8 & HFILE]] = 0;
	// mCastlingAvailable[ROOK_CASTLE[rook & RANK8 & AFILE]] = 0;

	// mCastlingAvailable[1] &= mKingNotMoved[1];
	// mCastlingAvailable[2] &= mKingNotMoved[2];
	// mCastlingAvailable[3] &= mKingNotMoved[1];
	// mCastlingAvailable[4] &= mKingNotMoved[2];
	
	if(move.mPiece == Piece::ROOK && move.mMoveFrom == Square::H1) mCastlingAvailable[1] = 0;
	if(move.mPiece == Piece::ROOK && move.mMoveFrom == Square::A1) mCastlingAvailable[3] = 0;
	if(move.mPiece == Piece::ROOK && move.mMoveFrom == Square::H8) mCastlingAvailable[2] = 0;
	if(move.mPiece == Piece::ROOK && move.mMoveFrom == Square::A8) mCastlingAvailable[4] = 0;

	if(move.mPiece == Piece::KING && move.mSide == Player::WHITE && move.mMoveFrom == Square::E1)
	{
		mCastlingAvailable[1] = 0;
		mCastlingAvailable[3] = 0;
	}
	if(move.mPiece == Piece::KING && move.mSide == Player::BLACK && move.mMoveFrom == Square::E8)
	{
		mCastlingAvailable[2] = 0;
		mCastlingAvailable[4] = 0;
	}		

	if(move.mPromoteTo == Piece::NO_PIECE) //if move is not a promotion
	{
		//add the piece to dest square
		mBoard[move.mPiece + move.mSide] |= moveToSquare;
	}else
	{
		//add the promoted piece to dest square
		mBoard[move.mPromoteTo + move.mSide] |= moveToSquare;
	}
	//remove piece from starting square(from piece's bitboard)
	mBoard[move.mPiece + move.mSide] &= ~moveFromSquare;
	
	mBoard[ALL_PIECES_BOARD] |= moveToSquare; //add piece to ALL_PIECES_BOARD
	mBoard[ALL_PIECES_BOARD] &= ~moveFromSquare; //remove piece from ALL_PIECES_BOARD
	mBoard[WHITE_PIECES_BOARD + move.mSide] |= moveToSquare; //add piece to color bitboard
	mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~moveFromSquare; //remove piece from color bitboard

	//remove captured piece from the board
	//only meaningful is move is a capture
	mBoard[move.mCaptured + otherSide] &= ~mBoard[ENPASSANT_BOARD];	
	mBoard[move.mCaptured + otherSide] &= ~moveToSquare;
	mBoard[WHITE_PIECES_BOARD + otherSide] &= ~moveToSquare;

	if(move.mCastle == Castling::KINGSIDE)
	{
		Bitboard rookColor[2] = {RANK1, RANK8};
		Bitboard rook = mBoard[Piece::ROOK + move.mSide] & HFILE;
		rook &= rookColor[move.mSide];
		mBoard[Piece::ROOK + move.mSide] |= (rook >> 2);
		mBoard[Piece::ROOK + move.mSide] &= ~rook;
		mBoard[ALL_PIECES_BOARD] |= (rook >> 2);
		mBoard[ALL_PIECES_BOARD] &= ~rook;
		mBoard[WHITE_PIECES_BOARD + move.mSide] |= (rook >> 2);
		mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~rook;
		mCastlingAvailable[1] = 0;
		mCastlingAvailable[2] = 0;
		mCastlingAvailable[3] = 0;
		mCastlingAvailable[4] = 0;
	}
	if(move.mCastle == Castling::QUEENSIDE)
	{
		Bitboard rookColor[2] = {RANK1, RANK8};
		Bitboard rook = mBoard[Piece::ROOK + move.mSide] & AFILE;
		rook &= rookColor[move.mSide];
		mBoard[Piece::ROOK + move.mSide] |= (rook << 3);
		mBoard[Piece::ROOK + move.mSide] &= ~rook;
		mBoard[ALL_PIECES_BOARD] |= (rook << 3);
		mBoard[ALL_PIECES_BOARD] &= ~rook;
		mBoard[WHITE_PIECES_BOARD + move.mSide] |= (rook << 3);
		mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~rook;
		mCastlingAvailable[1] = 0;
		mCastlingAvailable[2] = 0;
		mCastlingAvailable[3] = 0;
		mCastlingAvailable[4] = 0;
	}
	
	mHistory.push_back(move);
	mPlayerToMove = Player(otherSide);
	mBoard[ENPASSANT_BOARD] = move.mEnPassantBoard;
}

void Chessboard::unmakeMove()
{
	auto move = mHistory.back();
	mHistory.pop_back();
	
	Bitboard moveToSquare = uint64_t(1) << move.mMoveTo;
	Bitboard moveFromSquare = uint64_t(1) << move.mMoveFrom;
	auto otherSide = (move.mSide + 1) % 2;
	
	mBoard[move.mPiece + move.mSide] |= moveFromSquare; //add the piece where it came from
	mBoard[move.mPiece + move.mSide] &= ~moveToSquare; //remove the piece where it moved to

	mBoard[ALL_PIECES_BOARD] |= moveFromSquare; //add piece to ALL_PIECES_BOARD

	if(move.mCaptured == Piece::NO_PIECE) //if move is not a capture
	{
		mBoard[ALL_PIECES_BOARD] &= ~moveToSquare; //remove piece from ALL_PIECES_BOARD
	}else
	{
		//if move is capture restore other side pieces to color bitboard
		mBoard[WHITE_PIECES_BOARD + otherSide] |= moveToSquare;
	}

	mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~moveToSquare; //remove piece from color bitboard	
	mBoard[WHITE_PIECES_BOARD + move.mSide] |= moveFromSquare; //add piece to color bitboard
	
	//only meaningful if move was a capture
	if(move.enPassantCapture)
	{
		Bitboard CapturedPawnPosition[2] = {moveToSquare << 8, 
											moveToSquare >> 8};
		mBoard[WHITE_PAWNS_BOARD + otherSide] |= CapturedPawnPosition[otherSide];
		mBoard[ENPASSANT_BOARD] = CapturedPawnPosition[otherSide];
	}else
	{
		//restore captured piece to the board
		mBoard[move.mCaptured + otherSide] |= moveToSquare;
	}
		
	//remove the promoted piece from its board
	//only meaningful if last move was a promotion
	mBoard[move.mPromoteTo + move.mSide] &= ~moveToSquare;

	// if(move.mCastle == Castling::KINGSIDE)
	// {
	// 	Bitboard rookColor[2] = {RANK1, RANK8};
	// 	Bitboard rook = mBoard[Piece::ROOK + move.mSide] & FFILE;
	// 	rook &= rookColor[move.mSide];
	// 	mBoard[Piece::ROOK + move.mSide] |= (rook << 2);
	// 	mBoard[Piece::ROOK + move.mSide] &= ~rook;
	// 	mBoard[ALL_PIECES_BOARD] |= (rook << 2);
	// 	mBoard[ALL_PIECES_BOARD] &= ~rook;
	// 	mBoard[WHITE_PIECES_BOARD + move.mSide] |= (rook << 2);
	// 	mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~rook;
	// }
	// if(move.mCastle == Castling::QUEENSIDE)
	// {
	// 	Bitboard rookColor[2] = {RANK1, RANK8};
	// 	Bitboard rook = mBoard[Piece::ROOK + move.mSide] & DFILE;
	// 	rook &= rookColor[move.mSide];
	// 	mBoard[Piece::ROOK + move.mSide] |= (rook >> 3);
	// 	mBoard[Piece::ROOK + move.mSide] &= ~rook;
	// 	mBoard[ALL_PIECES_BOARD] |= (rook >> 3);
	// 	mBoard[ALL_PIECES_BOARD] &= ~rook;
	// 	mBoard[WHITE_PIECES_BOARD + move.mSide] |= (rook >> 3);
	// 	mBoard[WHITE_PIECES_BOARD + move.mSide] &= ~rook;
	// }
	
	mPlayerToMove = Player(otherSide);
}

int64_t Chessboard::getPositionValue()
{
	auto whitePawns = popCount(mBoard[WHITE_PAWNS_BOARD]) * PIECES_VALUES[Piece::PAWN];
	auto whiteKnights = popCount(mBoard[WHITE_KNIGHTS_BOARD]) * PIECES_VALUES[Piece::KNIGHT];
	auto whiteBishops = popCount(mBoard[WHITE_BISHOPS_BOARD]) * PIECES_VALUES[Piece::BISHOP];
	auto whiteRooks = popCount(mBoard[WHITE_ROOKS_BOARD]) * PIECES_VALUES[Piece::ROOK];
	auto whiteQueens = popCount(mBoard[WHITE_QUEEN_BOARD]) * PIECES_VALUES[Piece::QUEEN];
	auto black = whitePawns + whiteKnights + whiteBishops + whiteRooks + whiteQueens;

	auto blackPawns = popCount(mBoard[BLACK_PAWNS_BOARD]) * PIECES_VALUES[Piece::PAWN];
	auto blackKnights = popCount(mBoard[BLACK_KNIGHTS_BOARD]) * PIECES_VALUES[Piece::KNIGHT];
	auto blackBishops = popCount(mBoard[BLACK_BISHOPS_BOARD]) * PIECES_VALUES[Piece::BISHOP];
	auto blackRooks = popCount(mBoard[BLACK_ROOKS_BOARD]) * PIECES_VALUES[Piece::ROOK];
	auto blackQueens = popCount(mBoard[BLACK_QUEEN_BOARD]) * PIECES_VALUES[Piece::QUEEN];
	auto white = blackPawns + blackKnights + blackBishops + blackRooks + blackQueens;
	
	return white - black;
}
