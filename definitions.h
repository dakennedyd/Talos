#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <cctype>
#include <unordered_map>

#define LOG_ERROR(msg) std::cout << msg;
//#include "Move.h"
//#include "BoardState.h"

typedef uint64_t Bitboard;

enum Player {WHITE = 0, BLACK = 6};
enum Piece {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE};

static Bitboard AFILE = 0x0101010101010101;
static Bitboard BFILE = AFILE << 1;
static Bitboard CFILE = BFILE << 1;
static Bitboard DFILE = CFILE << 1;
static Bitboard EFILE = DFILE << 1;
static Bitboard FFILE = EFILE << 1;
static Bitboard GFILE = FFILE << 1;
static Bitboard HFILE = GFILE << 1;

static Bitboard RANK1 = 0xFF00000000000000;
static Bitboard RANK2 = RANK1 >> 8;
static Bitboard RANK3 = RANK2 >> 8;
static Bitboard RANK4 = RANK3 >> 8;
static Bitboard RANK5 = RANK4 >> 8;
static Bitboard RANK6 = RANK5 >> 8;
static Bitboard RANK7 = RANK6 >> 8;
static Bitboard RANK8 = RANK7 >> 8;

static double PAWN_VALUE = 1.0;
static double KNIGHT_VALUE = 3.0;
static double BISHOP_VALUE = 3.5;
static double ROOK_VALUE = 5.0;
static double QUEEN_VALUE = 9.0;

static const std::string NAME = "Chess Engine ver 0.1 Alpha";
static const std::string AUTHOR = "David A. Kennedy";
static const std::string START_POS_FEN_STRING = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static int WHITE_PAWNS_BOARD = 0;
static int WHITE_ROOKS_BOARD = 1;
static int WHITE_KNIGHTS_BOARD = 2;
static int WHITE_BISHOPS_BOARD = 3;
static int WHITE_KING_BOARD = 4;
static int WHITE_QUEEN_BOARD = 5;
static int WHITE_PIECES_BOARD = 6;
static int BLACK_PAWNS_BOARD = 7;
static int BLACK_ROOKS_BOARD = 8;
static int BLACK_KNIGHTS_BOARD = 9;
static int BLACK_BISHOPS_BOARD = 10;
static int BLACK_KING_BOARD = 11;
static int BLACK_QUEEN_BOARD = 12;
static int BLACK_PIECES_BOARD = 13;
static int ALL_PIECES_BOARD = 14;
static int ONPASSANT_BOARD = 15;

enum Square {
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1
};

//fix to be able to work with libc++ on ubuntu 16.04
//should remove this in the future when lib updates
namespace std {
template <> struct hash<Square> {
	size_t operator() (const Square &t) const 
	{ 
		return static_cast<size_t>(t); 
	}
};
}

static std::unordered_map<enum Square, std::string> SQUARE_NUM_TO_STR = 
{ 
{ A1,"a1" },{ B1,"b1" },{ C1,"c1" },{ D1,"d1" },
{ E1,"e1" },{ F1,"f1" },{ G1,"g1" },{ H1,"h1" },
{ A2,"a2" },{ B2,"b2" },{ C2,"c2" },{ D2,"d2" },
{ E2,"e2" },{ F2,"f2" },{ G2,"g2" },{ H2,"h2" },
{ A3,"a3" },{ B3,"b3" },{ C3,"c3" },{ D3,"d3" },
{ E3,"e3" },{ F3,"f3" },{ G3,"g3" },{ H3,"h3" },
{ A4,"a4" },{ B4,"b4" },{ C4,"c4" },{ D4,"d4" },
{ E4,"e4" },{ F4,"f4" },{ G4,"g4" },{ H4,"h4" },
{ A5,"a5" },{ B5,"b5" },{ C5,"c5" },{ D5,"d5" },
{ E5,"e5" },{ F5,"f5" },{ G5,"g5" },{ H5,"h5" },
{ A6,"a6" },{ B6,"b6" },{ C6,"c6" },{ D6,"d6" },
{ E6,"e6" },{ F6,"f6" },{ G6,"g6" },{ H6,"h6" },
{ A7,"a7" },{ B7,"b7" },{ C7,"c7" },{ D7,"d7" },
{ E7,"e7" },{ F7,"f7" },{ G7,"g7" },{ H7,"h7" },
{ A8,"a8" },{ B8,"b8" },{ C8,"c8" },{ D8,"d8" },
{ E8,"e8" },{ F8,"f8" },{ G8,"g8" },{ H8,"h8" }
};

static std::unordered_map<std::string, enum Square> SQUARE_STR_TO_NUM =
{ { "a1",A1 },{ "b1",B1 },{ "c1",C1 },{ "d1",D1 },
{ "e1",E1 },{ "f1",F1 },{ "g1",G1 },{ "h1",H1 },
{ "a2",A2 },{ "b2",B2 },{ "c2",C2 },{ "d2",D2 },
{ "e2",E2 },{ "f2",F2 },{ "g2",G2 },{ "h2",H2 },
{ "a3",A3 },{ "b3",B3 },{ "c3",C3 },{ "d3",D3 },
{ "e3",E3 },{ "f3",F3 },{ "g3",G3 },{ "h3",H3 },
{ "a4",A4 },{ "b4",B4 },{ "c4",C4 },{ "d4",D4 },
{ "e4",E4 },{ "f4",F4 },{ "g4",G4 },{ "h4",H4 },
{ "a5",A5 },{ "b5",B5 },{ "c5",C5 },{ "d5",D5 },
{ "e5",E5 },{ "f5",F5 },{ "g5",G5 },{ "h5",H5 },
{ "a6",A6 },{ "b6",B6 },{ "c6",C6 },{ "d6",D6 },
{ "e6",E6 },{ "f6",F6 },{ "g6",G6 },{ "h6",H6 },
{ "a7",A7 },{ "b7",B7 },{ "c7",C7 },{ "d7",D7 },
{ "e7",E7 },{ "f7",F7 },{ "g7",G7 },{ "h7",H7 },
{ "a8",A8 },{ "b8",B8 },{ "c8",C8 },{ "d8",D8 },
{ "e8",E8 },{ "f8",F8 },{ "g8",G8 },{ "h8",H8 } };

static int NORTH = 		0;
static int NORTH_EAST = 1;
static int EAST = 		2;
static int SOUTH_EAST = 3;
static int SOUTH = 		4;
static int SOUTH_WEST = 5;
static int WEST = 		6;
static int NORTH_WEST = 7;

static Bitboard attackRays[64][8];//precalculated rays first dim is square pos second dim is direction

/** parse tokens in a string and return them as a vector */
static std::vector<std::string> split(const std::string & str, const std::string & delimiter)
{
	std::vector<std::string> result;
	std::string s(str);
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	result.push_back(s);
	return result;
}

//https://stackoverflow.com/questions/671815/what-is-the-fastest
//-most-efficient-way-to-find-the-highest-set-bit-msb-in-an-i
#if defined(__GNUC__)
static Square getLowestSetBit(Bitboard n)
{
  return Square(__builtin_ctzll(n));
}

static Square getHighestSetBit(Bitboard n)
{
  return Square(63 ^ __builtin_clzll(n));
}

#elif defined(_MSC_VER)

static Square getLowestSetBit(Bitboard n)
{
  unsigned long index;
  _BitScanForward64(&index, n);
  return static_cast<Square>(index);
}

static Square getHighestSetBit(Bitboard n)
{
  unsigned long index;
  _BitScanReverse64(&index, n);
  return static_cast<Square>(index);
}
#endif
