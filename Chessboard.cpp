#include <cmath>
#include "Chessboard.h"
#include "definitions.h"

Bitboard Chessboard::attackRays[64][8];
inline void setPieceInBoard(uint64_t &piecePosition, Bitboard &board)
{
	Bitboard b = static_cast<uint64_t>(1) << piecePosition;
	board = board | b;
}

Chessboard::Chessboard()
{
	generateRays();
	mBoard.reserve(18);
	for (int i = 0; i < 18; i++) { mBoard.push_back(0); }
}

Bitboard Chessboard::checkIfenPassantPossibleOnNextMove(const Square a, const Square b)
{
	//Bitboard startSquare = SQUARE_TO_BITBOARD[a];
	//Bitboard endSquare = SQUARE_TO_BITBOARD[b];
	Bitboard startSquare = Bitboard(1)<<a;
	Bitboard endSquare = Bitboard(1)<<b;
	//auto x = startSquare & mBoard[WHITE_PAWNS_BOARD];
	//auto y = startSquare & RANK2;
	//auto z = endSquare & RANK4;
	if( (startSquare & mBoard[WHITE_PAWNS_BOARD]) && (startSquare & RANK2)
	&&  (endSquare & RANK4) )
	{
		return startSquare >> 8;
	}

	if( (startSquare & mBoard[BLACK_PAWNS_BOARD]) && (startSquare & RANK7)
	&&  (endSquare & RANK5) )
	{
		return startSquare << 8;
	}
	
	return 0x0;
}

Piece Chessboard::getPieceFromSquare(const Square square)
{
	Bitboard pieceBoard = uint64_t(1) << square;
	Bitboard p;
	int i = 0;
	for(i = 2; i<13; i+=2)
	{
		p = mBoard[i+mPlayerToMove];
		if(p & pieceBoard) return Piece(i);
	}
	return Piece::NO_PIECE;
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
		//auto n = mBoard[i+move.mSide] & moveToSquare;
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
	//std::cout << "  -----------------------\n";
	std::cout << "\n";
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
	std::cout << "  -----------------------\n   a  b  c  d  e  f  g  h\n";
	//printBitboads();
	std::string stm;
	if(mPlayerToMove == WHITE) stm="White"; else stm="Black";
	std::cout << "Side to move:" << stm;
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
	for(size_t n = 2; n< mBoard.size();n++)
	{
		//int count = 0;
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
	mPlayerToMove = Player::WHITE;
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
	//mPlayerToMove = Player(otherSide);
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
	
	//mPlayerToMove = Player(otherSide);
}

void Chessboard::setPositionValue()
{
	auto whitePawns = popCount(mBoard[WHITE_PAWNS_BOARD]) * PIECES_VALUES[PAWN];
	auto whiteKnights = popCount(mBoard[WHITE_KNIGHTS_BOARD]) * PIECES_VALUES[KNIGHT];
	auto whiteBishops = popCount(mBoard[WHITE_BISHOPS_BOARD]) * PIECES_VALUES[BISHOP];
	auto whiteRooks = popCount(mBoard[WHITE_ROOKS_BOARD]) * PIECES_VALUES[ROOK];
	auto whiteQueens = popCount(mBoard[WHITE_QUEEN_BOARD]) * PIECES_VALUES[QUEEN];
	auto white = whitePawns + whiteKnights + whiteBishops + whiteRooks + whiteQueens;

	auto blackPawns = popCount(mBoard[BLACK_PAWNS_BOARD]) * PIECES_VALUES[PAWN];
	auto blackKnights = popCount(mBoard[BLACK_KNIGHTS_BOARD]) * PIECES_VALUES[KNIGHT];
	auto blackBishops = popCount(mBoard[BLACK_BISHOPS_BOARD]) * PIECES_VALUES[BISHOP];
	auto blackRooks = popCount(mBoard[BLACK_ROOKS_BOARD]) * PIECES_VALUES[ROOK];
	auto blackQueens = popCount(mBoard[BLACK_QUEEN_BOARD]) * PIECES_VALUES[QUEEN];
	auto black = blackPawns + blackKnights + blackBishops + blackRooks + blackQueens;
	
	mPositionValue = white - black;
	//if(mPlayerToMove == BLACK) mPositionValue *= -1;
}

void Chessboard::whitePawnMoves()
{
	Bitboard p1, p2;//, pTotal;
	p1 = mBoard[WHITE_PAWNS_BOARD];
	p1 = p1 >> 8;
	p1 = p1 & (~(p1 & mBoard[ALL_PIECES_BOARD]));//all the panws that can advance one square
	p1 = p1 & (~RANK8); //don't advance to last rank, promotion handles this part
	//printBitboard(pTotal);
	auto movesOneSquare = getBitsPosition(p1);
	for (auto &i : movesOneSquare)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i));
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	p2 = (mBoard[WHITE_PAWNS_BOARD] & RANK2) >> 8;
	p2 = p2 & (~(p2 & mBoard[ALL_PIECES_BOARD]));
	p2 = p2 >> 8;
	p2 = p2 & (~(p2 & mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 16), Square(i),Piece::NO_PIECE, i << 8);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	//------------CAPTURE----------------------
	Bitboard captureLeft, captureRight;
	captureRight = mBoard[WHITE_PAWNS_BOARD];
	captureRight = captureRight >> 9;
	captureRight = captureRight & (~HFILE);
	captureRight = captureRight & (~RANK8); //don't advance to last rank, promotion handles this part
	captureRight = captureRight & mBoard[BLACK_PIECES_BOARD];
	auto captureRightBits = getBitsPosition(captureRight);
	for (auto &i : captureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	captureLeft = mBoard[WHITE_PAWNS_BOARD];
	captureLeft = captureLeft >> 7;
	captureLeft = captureLeft & (~AFILE);
	captureLeft = captureLeft & (~RANK8); //don't advance to last rank, promotion handles this part
	captureLeft = captureLeft & mBoard[BLACK_PIECES_BOARD];
	auto captureLeftBits = getBitsPosition(captureLeft);
	for (auto &i : captureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}
	//------------EN PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = mBoard[WHITE_PAWNS_BOARD];
	opCaptureRight = opCaptureRight >> 9;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i));
		setMoveCapture(m);
		m.enPassantCapture = true;
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	opCaptureLeft = mBoard[WHITE_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft >> 7;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & mBoard[ENPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i));
		setMoveCapture(m);
		m.enPassantCapture = true;
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	//------------PROMOTION----------------------
	Bitboard promotedPawn, capturePromotionL, capturePromotionR;
	promotedPawn = mBoard[WHITE_PAWNS_BOARD];
	promotedPawn = promotedPawn & RANK7;
	
	capturePromotionR = promotedPawn;
	capturePromotionL = promotedPawn;

	promotedPawn = promotedPawn >> 8;
	promotedPawn = promotedPawn & (~mBoard[ALL_PIECES_BOARD]);
	auto promotedPawnBits = getBitsPosition(promotedPawn);
	for (auto &i : promotedPawnBits)
	{
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::QUEEN);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::ROOK);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::KNIGHT);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		
		m = Move(Piece::PAWN, Player::WHITE, Square(i + 8), Square(i),Piece::BISHOP);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	capturePromotionR = capturePromotionR >> 9;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::QUEEN);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::ROOK);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::KNIGHT);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 9), Square(i),Piece::BISHOP);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL >> 7;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & mBoard[BLACK_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::QUEEN);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::ROOK);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::KNIGHT);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::WHITE, Square(i + 7), Square(i),Piece::BISHOP);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}
}

void Chessboard::blackPawnMoves()
{
	Bitboard p1, p2;//, pTotal;
	p1 = mBoard[BLACK_PAWNS_BOARD];
	p1 = p1 << 8;
	p1 = p1 & (~(p1 & mBoard[ALL_PIECES_BOARD]));//all the panws that can advance one square
	p1 = p1 & (~RANK1); //don't advance to last rank, promotion handles this part
	//printBitboard(pTotal);
	auto movesOneSquare = getBitsPosition(p1);
	for (auto &i : movesOneSquare)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i));		
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	p2 = (mBoard[BLACK_PAWNS_BOARD] & RANK7) << 8;
	p2 = p2 & (~(p2 & mBoard[ALL_PIECES_BOARD]));
	p2 = p2 << 8;
	p2 = p2 & (~(p2 & mBoard[ALL_PIECES_BOARD]));//all the pawns that can advance two squares	
	auto movesTwoSquares = getBitsPosition(p2);
	for (auto &i : movesTwoSquares)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 16), Square(i),Piece::NO_PIECE, i >> 8);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	//------------CAPTURE----------------------
	Bitboard captureLeft, captureRight;
	captureRight = mBoard[BLACK_PAWNS_BOARD];
	captureRight = captureRight << 7;
	captureRight = captureRight & (~HFILE);
	captureRight = captureRight & (~RANK1); //don't advance to last rank, promotion handles this part
	captureRight = captureRight & mBoard[WHITE_PIECES_BOARD];
	auto captureRightBits = getBitsPosition(captureRight);
	for (auto &i : captureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}

	captureLeft = mBoard[BLACK_PAWNS_BOARD];
	captureLeft = captureLeft << 9;
	captureLeft = captureLeft & (~AFILE);
	captureLeft = captureLeft & (~RANK1); //don't advance to last rank, promotion handles this part
	captureLeft = captureLeft & mBoard[WHITE_PIECES_BOARD];
	auto captureLeftBits = getBitsPosition(captureLeft);
	for (auto &i : captureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	//------------EN PASSANT CAPTURE---------------
	Bitboard opCaptureLeft, opCaptureRight;
	opCaptureRight = mBoard[BLACK_PAWNS_BOARD];
	opCaptureRight = opCaptureRight << 7;
	opCaptureRight = opCaptureRight & (~HFILE);
	opCaptureRight = opCaptureRight & mBoard[ENPASSANT_BOARD];
	auto opCaptureRightBits = getBitsPosition(opCaptureRight);
	for (auto &i : opCaptureRightBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i));
		m.enPassantCapture = true;
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}

	opCaptureLeft = mBoard[BLACK_PAWNS_BOARD];
	opCaptureLeft = opCaptureLeft << 9;
	opCaptureLeft = opCaptureLeft & (~AFILE);
	opCaptureLeft = opCaptureLeft & mBoard[ENPASSANT_BOARD];
	auto opCaptureLeftBits = getBitsPosition(opCaptureLeft);
	for (auto &i : opCaptureLeftBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i));
		m.enPassantCapture = true;
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}

	//------------PROMOTION----------------------
	Bitboard promotedPawn, capturePromotionL, capturePromotionR;
	promotedPawn = mBoard[BLACK_PAWNS_BOARD];
	promotedPawn = promotedPawn & RANK2;
	
	capturePromotionR = promotedPawn;
	capturePromotionL = promotedPawn;

	promotedPawn = promotedPawn << 8;
	promotedPawn = promotedPawn & (~mBoard[ALL_PIECES_BOARD]);
	auto promotedPawnBits = getBitsPosition(promotedPawn);
	for (auto &i : promotedPawnBits)
	{
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::QUEEN);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::ROOK);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::KNIGHT);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 8), Square(i),Piece::BISHOP);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	capturePromotionR = capturePromotionR << 7;
	capturePromotionR = capturePromotionR & (~HFILE);
	capturePromotionR = capturePromotionR & mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionRBits = getBitsPosition(capturePromotionR);
	for (auto &i : capturePromotionRBits)
	{	
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::QUEEN);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		
		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::ROOK);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::KNIGHT);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 7), Square(i),Piece::BISHOP);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}
	
	capturePromotionL = capturePromotionL << 9;
	capturePromotionL = capturePromotionL & (~AFILE);
	capturePromotionL = capturePromotionL & mBoard[WHITE_PIECES_BOARD];
	auto capturePromotionLBits = getBitsPosition(capturePromotionL);
	for (auto &i : capturePromotionLBits)
	{		
		auto m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::QUEEN);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::ROOK);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::KNIGHT);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();

		m = Move(Piece::PAWN, Player::BLACK, Square(i - 9), Square(i),Piece::BISHOP);
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		//mPossibleMoves.back().printMove();
	}
}

void Chessboard::whiteKnightMoves()
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
	oneOClockAttack = oneOClockAttack & (~(RANK1 | RANK2 | AFILE));	
	twoOClockAttack = twoOClockAttack & (~(AFILE | RANK1 | BFILE));
	fourOClockAttack = fourOClockAttack & (~(AFILE | RANK8 | BFILE));
	fiveOClockAttack = fiveOClockAttack & (~(AFILE | RANK8 | RANK7));
	sevenOClockAttack = sevenOClockAttack & (~(HFILE | RANK8 | RANK7));
	eightOClockAttack = eightOClockAttack & (~(HFILE | RANK8 | GFILE));
	tenOClockAttack = tenOClockAttack & (~(HFILE | RANK1 | GFILE));
	elevenOClockAttack = elevenOClockAttack & (~(HFILE | RANK1 | RANK2));
	
	auto oneOClockAttackBits = getBitsPosition(oneOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : oneOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 15), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 6), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();		
	}
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 10), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();		
	}

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 17), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 15), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}
	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i - 6), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 10), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~mBoard[WHITE_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::WHITE, Square(i + 17), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

}

void Chessboard::blackKnightMoves()
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
	oneOClockAttack = oneOClockAttack & (~(RANK1 | RANK2 | AFILE));	
	twoOClockAttack = twoOClockAttack & (~(AFILE | RANK1 | BFILE));
	fourOClockAttack = fourOClockAttack & (~(AFILE | RANK8 | BFILE));
	fiveOClockAttack = fiveOClockAttack & (~(AFILE | RANK8 | RANK7));
	sevenOClockAttack = sevenOClockAttack & (~(HFILE | RANK8 | RANK7));
	eightOClockAttack = eightOClockAttack & (~(HFILE | RANK8 | GFILE));
	tenOClockAttack = tenOClockAttack & (~(HFILE | RANK1 | GFILE));
	elevenOClockAttack = elevenOClockAttack & (~(HFILE | RANK1 | RANK2));
	
	auto oneOClockAttackBits = getBitsPosition(oneOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : oneOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 15), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}
	
	auto twoOClockAttackBits = getBitsPosition(twoOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : twoOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 6), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();		
	}
	
	auto fourOClockAttackBits = getBitsPosition(fourOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fourOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 10), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();				
	}

	auto fiveOClockAttackBits = getBitsPosition(fiveOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : fiveOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 17), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();						
	}

	auto sevenOClockAttackBits = getBitsPosition(sevenOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : sevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 15), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}

	auto eightOClockAttackBits = getBitsPosition(eightOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : eightOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i - 6), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();		
	}

	auto tenOClockAttackBits = getBitsPosition(tenOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : tenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 10), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();				
	}

	auto elevenOClockAttackBits = getBitsPosition(elevenOClockAttack & (~mBoard[BLACK_PIECES_BOARD]));
	for (auto &i : elevenOClockAttackBits)
	{
		auto m = Move(Piece::KNIGHT, Player::BLACK, Square(i + 17), Square(i));
		setMoveCapture(m);
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
	}
}

void Chessboard::whiteRookMoves()
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
		
		auto rookAttackBits = getBitsPosition(rookAttack);
		for (auto &i : rookAttackBits)
		{
			auto m = Move(Piece::ROOK, Player::WHITE, Square(rookBitPos), Square(i));
			setMoveCapture(m);
			auto a = mCastlingAvailable[1];
			auto c = mCastlingAvailable[3];
			makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
			mCastlingAvailable[1] = a;
			mCastlingAvailable[3] = c;
		}
	}
}

void Chessboard::blackRookMoves()
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
		
		auto rookAttackBits = getBitsPosition(rookAttack);
		for (auto &i : rookAttackBits)
		{
			auto m = Move(Piece::ROOK, Player::BLACK, Square(rookBitPos), Square(i));
			setMoveCapture(m);
			auto b = mCastlingAvailable[2];
			auto d = mCastlingAvailable[4];
			makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
			mCastlingAvailable[2] = b;
			mCastlingAvailable[4] = d;
		}
	}
}

void Chessboard::whiteBishopMoves()
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
		
		auto bishopAttackBits = getBitsPosition(bishopAttack);
		for (auto &i : bishopAttackBits)
		{
			auto m = Move(Piece::BISHOP, Player::WHITE, Square(bishopBitPos), Square(i));
			setMoveCapture(m);
			makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
		}			
	}
}

void Chessboard::blackBishopMoves()
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
		
		auto bishopAttackBits = getBitsPosition(bishopAttack);
		for (auto &i : bishopAttackBits)
		{
			auto m = Move(Piece::BISHOP, Player::BLACK, Square(bishopBitPos), Square(i));
			setMoveCapture(m);
			makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
		}
	}
}

void Chessboard::whiteQueenMoves()
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
		
		auto queenAttacksBits = getBitsPosition(queenAttacks);
		for (auto &i : queenAttacksBits)
		{
			auto m = Move(Piece::QUEEN, Player::WHITE, Square(queenBitsPos), Square(i));
			setMoveCapture(m);
			makeMove(m);
			if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
		}			
	}
}

void Chessboard::blackQueenMoves()
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
		
		auto queenAttacksBits = getBitsPosition(queenAttacks);
		for (auto &i : queenAttacksBits)
		{
			auto m = Move(Piece::QUEEN, Player::BLACK, Square(queenBitsPos), Square(i));
			setMoveCapture(m);
			makeMove(m);
			if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
			{
				mPossibleMoves.emplace_back(m);		
			}
			unmakeMove();
		}			
	}
}

void Chessboard::whiteKingMoves()
{
	Bitboard kingBoard = mBoard[WHITE_KING_BOARD];
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
	//printBitboard(kingAttacks);

	//can't capture own pieces
	Bitboard temp = kingAttacks & mBoard[WHITE_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits)
	{
		auto m = Move(Piece::KING, Player::WHITE, Square(kingBitPos[0]), Square(i));
		setMoveCapture(m);
		auto a = mCastlingAvailable[1];
		auto b = mCastlingAvailable[3];
		makeMove(m);
		if(!checkIfSquaresAreAttackedByBlack(mBoard[WHITE_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		mCastlingAvailable[1] = a;
		mCastlingAvailable[3] = b;
	}

	if(mCastlingAvailable[1])//kingside castling
	{
		Bitboard squares = 8070450532247928832;
		if(((squares & ~kingBoard) & mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByBlack(squares))
			{
				auto m = Move(Piece::KING, Player::WHITE, Square::E1, Square::G1);
				m.mCastle = Castling::KINGSIDE;
				mPossibleMoves.emplace_back(m);
			}
		}
	}
	if(mCastlingAvailable[3])//queenside castling
	{
		Bitboard squares = 2161727821137838080;
		if(((squares & ~kingBoard) & mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
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

void Chessboard::blackKingMoves()
{
	Bitboard kingBoard = mBoard[BLACK_KING_BOARD];
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
	//printBitboard(kingAttacks);
	
	//can't capture own pieces
	Bitboard temp = kingAttacks & mBoard[BLACK_PIECES_BOARD];
	kingAttacks = kingAttacks & ~temp;
	//printBitboard(kingAttacks);

	auto kingAttacksBits = getBitsPosition(kingAttacks);
	auto kingBitPos = getBitsPosition(kingBoard);
	for (auto &i : kingAttacksBits)
	{
		auto m = Move(Piece::KING, Player::BLACK, Square(kingBitPos[0]), Square(i));
		setMoveCapture(m);
		auto a = mCastlingAvailable[2];
		auto b = mCastlingAvailable[4];
		makeMove(m);
		if(!checkIfSquaresAreAttackedByWhite(mBoard[BLACK_KING_BOARD]))
		{
			mPossibleMoves.emplace_back(m);		
		}
		unmakeMove();
		mCastlingAvailable[2] = a;
		mCastlingAvailable[4] = b;
	}

	if(mCastlingAvailable[2])//kingside castling
	{
		Bitboard squares = 112;
		if(((squares & ~kingBoard) & mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
		{
			if(!checkIfSquaresAreAttackedByWhite(squares))
			{
				auto m = Move(Piece::KING, Player::BLACK, Square::E8, Square::G8);
				m.mCastle = Castling::KINGSIDE;
				mPossibleMoves.emplace_back(m);
			}
		}
	}
	if(mCastlingAvailable[4])//queenside castling
	{
		Bitboard squares = 14;
		if((squares & mBoard[ALL_PIECES_BOARD]) == 0)//if squares are not occupied
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

void Chessboard::generateMoves()
{
	mPossibleMoves.clear();
	if(mPlayerToMove == Player::WHITE)
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

	// for(auto m : mPossibleMoves) 
	// {
	// 	m.printMove();
	// 	std::cout << " ";
	// }	
	// std::cout << "\n";		
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

void Chessboard::generateRays()
{
	
	int bitCount = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			//NORTH
			Chessboard::attackRays[bitCount][NORTH] = 0;
			for(int k = 0; k < i; k++)
			{
				uint64_t x = uint64_t(1) << bitCount;
				Chessboard::attackRays[bitCount][NORTH] |= (x >> ( 8*(k+1)));
			}
			//NORTH-EAST
			Chessboard::attackRays[bitCount][NORTH_EAST] = 0;
			int minNE = i < (7-j) ? i : (7-j);
			for(int k = 0; k < minNE; k++)
			{
				Chessboard::attackRays[bitCount][NORTH_EAST] |= (uint64_t(1) << ( bitCount - ( 7*(k+1) ) ));
			}
			//EAST
			Chessboard::attackRays[bitCount][EAST] = 0;
			for(int k = 0; k < 7-j; k++)
			{
				Chessboard::attackRays[bitCount][EAST] |= (uint64_t(1) << ( bitCount + ( k+1 ) ));
			}
			//SOUTH-EAST
			Chessboard::attackRays[bitCount][SOUTH_EAST] = 0;
			int minSE = (7-i)<(7-j) ? 7-i : 7-j;
			for(int k = 0; k < minSE; k++)
			{
				Chessboard::attackRays[bitCount][SOUTH_EAST] |= (uint64_t(1) << ( bitCount + ( 9*(k+1) ) ));
			}
			//SOUTH
			Chessboard::attackRays[bitCount][SOUTH] = 0;
			for(int k = 0; k < 7-i; k++)
			{
				Chessboard::attackRays[bitCount][SOUTH] |= (uint64_t(1) << ( bitCount + ( 8*(k+1) ) ));
			}
			//SOUTH-WEST
			Chessboard::attackRays[bitCount][SOUTH_WEST] = 0;
			int minSW = (7-i) < j ? (7-i) : j;
			for(int k = 0; k < minSW; k++)
			{
				Chessboard::attackRays[bitCount][SOUTH_WEST] |= (uint64_t(1) << ( bitCount + ( 7*(k+1) ) ));
			}
			//WEST
			Chessboard::attackRays[bitCount][WEST] = 0;
			for(int k = 0; k < j; k++)
			{
				Chessboard::attackRays[bitCount][WEST] |= (uint64_t(1) << ( bitCount - ( k+1 ) ));
			}
			//NORTH-WEST
			Chessboard::attackRays[bitCount][NORTH_WEST] = 0;
			int minNW1 = j < i ? j : i;
			int minNW2 = i < j ? i : j;
			int minNW = minNW1 > minNW2 ? minNW1 : minNW2;
			if(i == 7) minNW = j;
			for(int k = 0; k < minNW; k++)
			{
				Chessboard::attackRays[bitCount][NORTH_WEST] |= (uint64_t(1) << ( bitCount - ( 9*(k+1) ) ));
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