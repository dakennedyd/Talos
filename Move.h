#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
#include "definitions.h"

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

//taken from https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
// struct EnumClassHash
// {
//     template <typename T>
//     std::size_t operator()(T t) const
//     {
//         //return  static_cast<std::size_t>(t);
// 		return std::hash<T>{}(t);
//     }
// };

static std::unordered_map<enum Square, Bitboard> SQUARE_TO_BITBOARD = 
{
	{A8,uint64_t(1)<<7}, {B8,uint64_t(1)<<6}, {C8,uint64_t(1)<<5}, {D8,uint64_t(1)<<4}, {E8,uint64_t(1)<<3}, {F8,uint64_t(1)<<2}, {G8,uint64_t(1)<<1}, {H8,uint64_t(1)<<0},
	{A7,uint64_t(1)<<15}, {B7,uint64_t(1)<<14}, {C7,uint64_t(1)<<13}, {D7,uint64_t(1)<<12}, {E7,uint64_t(1)<<11}, {F7,uint64_t(1)<<10}, {G7,uint64_t(1)<<9}, {H7,uint64_t(1)<<8},
	{A6,uint64_t(1)<<23}, {B6,uint64_t(1)<<22}, {C6,uint64_t(1)<<21}, {D6,uint64_t(1)<<20}, {E6,uint64_t(1)<<19}, {F6,uint64_t(1)<<18}, {G6,uint64_t(1)<<17}, {H6,uint64_t(1)<<16},
	{A5,uint64_t(1)<<31}, {B5,uint64_t(1)<<30}, {C5,uint64_t(1)<<29}, {D5,uint64_t(1)<<28}, {E5,uint64_t(1)<<27}, {F5,uint64_t(1)<<26}, {G5,uint64_t(1)<<25}, {H5,uint64_t(1)<<24},
	{A4,uint64_t(1)<<39}, {B4,uint64_t(1)<<38}, {C4,uint64_t(1)<<37}, {D4,uint64_t(1)<<36}, {E4,uint64_t(1)<<35}, {F4,uint64_t(1)<<34}, {G4,uint64_t(1)<<33}, {H4,uint64_t(1)<<32},
	{A3,uint64_t(1)<<47}, {B3,uint64_t(1)<<46}, {C3,uint64_t(1)<<45}, {D3,uint64_t(1)<<44}, {E3,uint64_t(1)<<43}, {F3,uint64_t(1)<<42}, {G3,uint64_t(1)<<41}, {H3,uint64_t(1)<<40},
	{A2,uint64_t(1)<<55},  {B2,uint64_t(1)<<54},  {C2,uint64_t(1)<<53}, {D2,uint64_t(1)<<52}, {E2,uint64_t(1)<<51}, {F2,uint64_t(1)<<50}, {G2,uint64_t(1)<<49}, {H2,uint64_t(1)<<48},
	{A1,uint64_t(1)<<63},  {B1,uint64_t(1)<<62},  {C1,uint64_t(1)<<61},  {D1,uint64_t(1)<<60},  {E1,uint64_t(1)<<59},  {F1,uint64_t(1)<<58},  {G1,uint64_t(1)<<57},  {H1,uint64_t(1)<<56}
};

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

struct Move
{
	//Move() = delete;
	Move(const enum Square moveFrom, const enum Square moveTo, const enum Piece piecePromotion = Piece::NO_PIECE,
	const Bitboard onPassant = 0x0 )
	: mMoveFrom(moveFrom), mMoveTo(moveTo), mPromoteTo(piecePromotion),
	mOnPassantBoard(onPassant) {
		//SQUARE_NUM_TO_STR.emplace(std::pair<Square, std::string>{ A1,"a1" });
	}
	~Move() = default;
	void printMove();

	Square mMoveFrom;
	Square mMoveTo;
	Piece mPromoteTo;
	double mScore = 0;
	Bitboard mOnPassantBoard;
};

