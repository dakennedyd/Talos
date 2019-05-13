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
	mBoard.reserve(15);
	for (int i = 0; i < 15; i++) { mBoard.push_back(0); }
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
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_PAWNS_BOARD]; if (b != 0) { std::cout << " p "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_ROOKS_BOARD]; if (b != 0) { std::cout << " r "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_KNIGHTS_BOARD]; if (b != 0) { std::cout << " n "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_BISHOPS_BOARD]; if (b != 0) { std::cout << " b "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_QUEEN_BOARD]; if (b != 0) { std::cout << " q "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[BLACK_KING_BOARD]; if (b != 0) { std::cout << " k "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_PAWNS_BOARD]; if (b != 0) { std::cout << " P "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_ROOKS_BOARD]; if (b != 0) { std::cout << " R "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_KNIGHTS_BOARD]; if (b != 0) { std::cout << " N "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_BISHOPS_BOARD]; if (b != 0) { std::cout << " B "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_QUEEN_BOARD]; if (b != 0) { std::cout << " Q "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[WHITE_KING_BOARD]; if (b != 0) { std::cout << " K "; continue; }
			b = (static_cast<uint64_t>(1) << (i * 8 + j)) & mBoard[ALL_PIECES_BOARD]; if (b == 0) std::cout << " - ";
		}
		std::cout << "\n";
	}
	std::cout << "  ------------------------\n   a  b  c  d  e  f  g  h\n";
}

void BoardState::reset()
{
	mPlayerToMove = WHITE;
	std::fill(mBoard.begin(), mBoard.end(), 0); //reset board

	std::queue<Move> empty;
	std::swap(mHistory, empty);	
}

void BoardState::makeMove(Move && move)
{
	Bitboard moveFromSquare = std::uint64_t(1) << move.mMoveFrom;
	Bitboard moveToSquare = std::uint64_t(1) << move.mMoveTo;

	int i = 0;
	mBoard[ALL_PIECES_BOARD] = 0;
	for (i = 0; i <= 11; i++)
	{
		//auto tmp = mBoard[i] & moveFromSquare;
		if ((mBoard[i] & moveFromSquare) != 0)
		{
			mBoard[i] = mBoard[i] & (~moveFromSquare); //remove piece from starting square
			mBoard[i] = mBoard[i] | moveToSquare; //move piece to target square
		}
		mBoard[ALL_PIECES_BOARD] = mBoard[ALL_PIECES_BOARD] | mBoard[i];
		if (mPlayerToMove == Player::WHITE)
		{
			mBoard[WHITE_PIECES_BOARD] = mBoard[WHITE_PIECES_BOARD] & mBoard[i];
		}
		else
		{
			mBoard[BLACK_PIECES_BOARD] = mBoard[BLACK_PIECES_BOARD] & mBoard[i];
		}
	}
	//todo: calc move score
	mHistory.push(move);	
	mPlayerToMove = !mPlayerToMove;
}
