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

enum Player {WHITE, BLACK};
enum Piece {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE};


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
static int BLACK_PAWNS_BOARD = 6;
static int BLACK_ROOKS_BOARD = 7;
static int BLACK_KNIGHTS_BOARD = 8;
static int BLACK_BISHOPS_BOARD = 9;
static int BLACK_KING_BOARD = 10;
static int BLACK_QUEEN_BOARD = 11;
static int BLACK_PIECES_BOARD = 12;
static int WHITE_PIECES_BOARD = 13;
static int ALL_PIECES_BOARD = 14;
static int ONPASSANT_BOARD = 15;

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

