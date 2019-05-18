#include <cmath>
#include "BoardState.h"
#include "definitions.h"

inline void setPieceInBoard(uint64_t &piecePosition, Bitboard &board)
{
	//Bitboard b = 1 * std::powl(2, piecePosition);
	Bitboard b = static_cast<uint64_t>(1) << piecePosition;
	board = board | b;
}

BoardState::BoardState()
{
	mBoard.reserve(16);
	for (int i = 0; i < 16; i++) { mBoard.push_back(0); }
}

void BoardState::setState(const std::string & FENstring)
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

void BoardState::printBoard()
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
	printBitboads();
}

void BoardState::printBitboads()
{
	std::vector<std::string> names = {
		"WHITE_PAWNS_BOARD",
		"WHITE_ROOKS_BOARD",
		"WHITE_KNIGHTS_BOARD",
		"WHITE_BISHOPS_BOARD",
		"WHITE_KING_BOARD",
		"WHITE_QUEEN_BOARD", 
		"BLACK_PAWNS_BOARD",
		"BLACK_ROOKS_BOARD",
		"BLACK_KNIGHTS_BOARD",
		"BLACK_BISHOPS_BOARD",
		"BLACK_KING_BOARD",
		"BLACK_QUEEN_BOARD", 
		"BLACK_PIECES_BOARD",
		"WHITE_PIECES_BOARD",
		"ALL_PIECES_BOARD",
		"ONPASSANT_BOARD"
	};
	for(int n = 0; n< mBoard.size();n++)
	{
		int count = 0;
		Bitboard b;
		std::cout << names[n] << "\n";
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

void BoardState::reset()
{
	mPlayerToMove = WHITE;
	std::fill(mBoard.begin(), mBoard.end(), 0); //reset board

	std::queue<Move> empty;
	std::swap(mHistory, empty);	
}

void BoardState::makeMove(Move move)
{
	Bitboard moveFromSquare = Bitboard(1) << move.mMoveFrom;
	Bitboard moveToSquare = Bitboard(1) << move.mMoveTo;

	//if move is a capture then remove opponent's piece from color bitboard
	if (mPlayerToMove == Player::WHITE)
	{
		mBoard[BLACK_PIECES_BOARD] = (~moveToSquare) & mBoard[BLACK_PIECES_BOARD];		
		
		//if move is onPassant capture
		if( (moveFromSquare & mBoard[WHITE_PAWNS_BOARD]) && (moveToSquare & mBoard[ONPASSANT_BOARD]) )
		{
			mBoard[BLACK_PIECES_BOARD] = mBoard[BLACK_PIECES_BOARD] & (~(moveToSquare << 8));
			mBoard[BLACK_PAWNS_BOARD] = mBoard[BLACK_PAWNS_BOARD] & (~(moveToSquare << 8));
			mBoard[ALL_PIECES_BOARD] = mBoard[ALL_PIECES_BOARD] & (~(moveToSquare << 8));
		}
	}
	else
	{
		mBoard[WHITE_PIECES_BOARD] = (~moveToSquare) & mBoard[WHITE_PIECES_BOARD];		

		//if move is onPassant capture
		//auto x = moveFromSquare & mBoard[BLACK_PAWNS_BOARD];
		//auto y = moveToSquare & mBoard[ONPASSANT_BOARD];
		if( (moveFromSquare & mBoard[BLACK_PAWNS_BOARD]) && (moveToSquare & mBoard[ONPASSANT_BOARD] ) )
		{
			mBoard[WHITE_PIECES_BOARD] = mBoard[WHITE_PIECES_BOARD] & (~(moveToSquare >> 8));
			mBoard[WHITE_PAWNS_BOARD] = mBoard[WHITE_PAWNS_BOARD] & (~(moveToSquare >> 8));
			mBoard[ALL_PIECES_BOARD] = mBoard[ALL_PIECES_BOARD] & (~(moveToSquare >> 8));
		}
	}
	for (int i = 0; i <= 11; i++)
	{
		if ((mBoard[i] & moveFromSquare) != 0)//only modify the correct bitboard
		{
			//place piece in target square(in selected bitboard)
			if(move.mPromoteTo == Piece::NO_PIECE) mBoard[i] = mBoard[i] | moveToSquare;
			mBoard[i] = mBoard[i] & (~moveFromSquare); //remove piece from starting square(in selected bitboard)

			//place piece in target square in ALL_PIECES_BOARD
			mBoard[ALL_PIECES_BOARD] |= moveToSquare;
			//remove piece from starting square in ALL_PIECES_BOARD
			mBoard[ALL_PIECES_BOARD] = mBoard[ALL_PIECES_BOARD] & (~moveFromSquare);

			//move piece in selected color bitboard
			if (mPlayerToMove == Player::WHITE)
			{
				mBoard[WHITE_PIECES_BOARD] = mBoard[WHITE_PIECES_BOARD] | moveToSquare;
				mBoard[WHITE_PIECES_BOARD] = mBoard[WHITE_PIECES_BOARD] & (~moveFromSquare);
			}
			else
			{
				mBoard[BLACK_PIECES_BOARD] = mBoard[BLACK_PIECES_BOARD] | moveToSquare;
				mBoard[BLACK_PIECES_BOARD] = mBoard[BLACK_PIECES_BOARD] & (~moveFromSquare);
			}
			
			//if move is a capture remove opponet's piece from its bitboard
			if (mPlayerToMove == Player::WHITE)
			{
				for (int j = 6; j <= 11; j++)
				{
					//if(i == j) continue;
					mBoard[j] = (~moveToSquare) & mBoard[j];
				}
			}
			else
			{
				for (int j = 0; j <= 5; j++)
				{
					//if(i == j) continue;
					mBoard[j] = (~moveToSquare) & mBoard[j];
				}
			}
		}


	}
	//if move is a promotion place the new piece on the board
	if (mPlayerToMove == Player::WHITE)
	{
		if(move.mPromoteTo == Piece::QUEEN) mBoard[WHITE_QUEEN_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::ROOK) mBoard[WHITE_ROOKS_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::BISHOP) mBoard[WHITE_BISHOPS_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::KNIGHT) mBoard[WHITE_KNIGHTS_BOARD] |= moveToSquare;
	}else{
		if(move.mPromoteTo == Piece::QUEEN) mBoard[BLACK_QUEEN_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::ROOK) mBoard[BLACK_ROOKS_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::BISHOP) mBoard[BLACK_BISHOPS_BOARD] |= moveToSquare;
		if(move.mPromoteTo == Piece::KNIGHT) mBoard[BLACK_KNIGHTS_BOARD] |= moveToSquare;
	}

	//todo: calc move score
	mHistory.push(move);	
	mPlayerToMove = !mPlayerToMove;
	mBoard[ONPASSANT_BOARD] = move.mOnPassantBoard;

}
