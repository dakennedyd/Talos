#include <cmath>
#include "Chessboard.h"
#include "definitions.h"

inline void setPieceInBoard(uint64_t &piecePosition, Bitboard &board)
{
	//Bitboard b = 1 * std::powl(2, piecePosition);
	Bitboard b = static_cast<uint64_t>(1) << piecePosition;
	board = board | b;
}

Chessboard::Chessboard()
{
	mBoard.reserve(18);
	for (int i = 0; i < 18; i++) { mBoard.push_back(0); }
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

	//std::queue<Move> empty;
	//std::swap(mHistory, empty);	
	mHistory.clear();
}

void Chessboard::makeMove(const Move &move)
{
	Bitboard moveFromSquare = Bitboard(1) << move.mMoveFrom;
	Bitboard moveToSquare = Bitboard(1) << move.mMoveTo;
	auto otherSide = (move.mSide + 1) % 2;

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

	mPlayerToMove = Player(otherSide);
}
