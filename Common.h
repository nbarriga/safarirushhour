/*
 * Common.h
 *
 *  Created on: Jan 23, 2013
 *      Author: nbarriga
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <string>

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <cmath>
#include <sstream>
#include <bitset>
#include <utility>
#include <climits>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cinttypes>
#include <boost/random.hpp>
#include <queue>
#include <unordered_set>
#include <functional>
#include <boost/functional/hash/hash.hpp>
using namespace std;

const uint16_t HOR=0, VERT=1;
const uint8_t UP=0, DOWN=1, LEFT=2, RIGHT=3;
const uint8_t rover=0;
const uint8_t two_by_two=1;
const uint8_t three_by_one=2;
const uint8_t two_by_one=3;

const uint64_t outside=3ull|(3ull<<9)|(3ull<<18)|(3ull<<27)|(3ull<<36)|(3ull<<45)|(3ull<<54);
const uint64_t outside_exit=3ull|(3ull<<9)|(3ull<<18)/*|(3ull<<27)|(3ull<<36)*/|(3ull<<45)|(3ull<<54);
const uint64_t inside=~outside;
const uint64_t inside_exit=~outside_exit;
const uint64_t goal=inside_exit^inside;
const uint64_t illegal=~0ull;
const uint16_t illegal_piece=~0;

//masks to extract each part
const uint16_t LETTER=31<<9,COL=7<<6,ROW=7<<3,DIR=3<<2,TYPE=3;

extern uint8_t piece_type[256];
extern uint8_t piece_letter[20];//index to char
extern uint16_t letter_piece[256];//char to index

extern uint64_t piece_board[512];

//[direction][distance][starting piece]
extern uint64_t piece_board_neighbours[4][7][512];
extern uint16_t piece_neighbours[4][7][512];

struct piece_t {
	char letter;
	uint16_t row, col, dir, type, Xsize, Ysize;
	piece_t(char letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type);
	string toString() const;
};
uint16_t encode_piece(uint16_t letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type);
uint16_t shorten(uint16_t piece);
uint16_t encode_piece(uint16_t row,uint16_t col,uint16_t dir,uint16_t type);
piece_t decode_piece(uint16_t piece);

//board is 7 rows and 9 columns, 2 rightmost columns are outside
uint64_t get_board(uint16_t row,uint16_t col,uint16_t dir,uint16_t type);

//uint64_t move(uint64_t board, uint8_t direction, uint8_t distance, uint8_t type, bool vertical);
uint16_t move(uint16_t piece, uint8_t direction, uint8_t distance);

void printBoard(uint64_t board);

uint16_t countSetBits(uint64_t i);

struct _dummy_static_initializer{
	_dummy_static_initializer();
};
#endif /* COMMON_H_ */
