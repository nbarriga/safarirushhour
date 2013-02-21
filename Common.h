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

const uint16_t HOR=0;
const uint16_t VERT=1;
const uint8_t rover=0;
const uint8_t two_by_two=1;
const uint8_t three_by_one=2;
const uint8_t two_by_one=3;

static uint8_t piece_type[256];
static uint8_t piece_letter[19]={'u','v','x','o', 'q', 's','p', 'r','a', 'f','d',
		'g', 'j','b', 'h', 'i', 'k','c', 'e'};
static uint16_t letter_piece[256];

static uint64_t piece_board[512];

struct piece_t {
	char letter;
	uint16_t row, col, dir, type;
	piece_t(char letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type):
		letter(letter),
		row(row),
		col(col),
		dir(dir),
		type(type){}
};
static uint16_t encode_piece(uint16_t letter, uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	return (letter<<9) | (row<<6) | (col<<3) | (dir<<2) | (type);
}
static uint16_t encode_piece(uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	return (row<<6) | (col<<3) | (dir<<2) | (type);
}
static piece_t decode_piece(uint16_t piece){
	return piece_t(piece_letter[(piece>>9)&31], (piece>>6)&7, (piece>>3)&7, (piece>>2)&1, piece&3);
}
//board is 7 rows and 9 columns, 2 rightmost columns are outside
static uint64_t get_board(uint16_t row,uint16_t col,uint16_t dir,uint16_t type){
	uint64_t board;
	switch(type){
	case rover:
	case two_by_two:
		board=(3ull<<62) & (3ull<<53);
		break;
	case two_by_one:
		if(dir==HOR){
			board=(3ull<<62);
		}else{
			board=(1ull<<63) & (1ull<<54);
		}
		break;
	case three_by_one:
		if(dir==HOR){
			board=(7ull<<62);
		}else{
			board=(1ull<<63) & (1ull<<54) & (1ull<<45);
		}
		break;
	}
	board>>=col;
	board>>=row*9;
	return board;
}

struct _dummy_static_initializer{
	_dummy_static_initializer(){
		for(auto i=0;i<19;i++){
			letter_piece[piece_letter[i]]=i;
			//cout<<"letter_piece["<<piece_letter[i]<<"]="<<i<<endl;;
			switch(piece_letter[i]){
			case 'x':
				piece_type[piece_letter[i]]=rover;
				for(int row=0;row<6;row++){
					for(int col=0;col<6;col++){
						piece_board[encode_piece(row,col,HOR,rover)]=get_board(row,col,HOR,rover);
					}
				}
				break;
			case 'u':
			case 'v':
				piece_type[piece_letter[i]]=two_by_two;
				for(int row=0;row<6;row++){
					for(int col=0;col<6;col++){
						piece_board[encode_piece(row,col,HOR,two_by_two)]=get_board(row,col,HOR,two_by_two);
					}
				}
				break;
			case 'o':
			case 'q':
			case 's':
			case 'p':
			case 'r':
				piece_type[piece_letter[i]]=three_by_one;
				for(int row=0;row<7;row++){
					for(int col=0;col<7;col++){
						for(int dir=0;dir<2;dir++){
							if(dir==HOR && col>=4)continue;
							if(dir==VERT && row>=4)continue;
							piece_board[encode_piece(row,col,dir,three_by_one)]=get_board(row,col,dir,three_by_one);
						}
					}
				}
				break;
			case 'a':
			case 'f':
			case 'd':
			case 'g':
			case 'j':
			case 'b':
			case 'h':
			case 'i':
			case 'k':
			case 'c':
			case 'e':
				piece_type[piece_letter[i]]=two_by_one;
				for(int row=0;row<7;row++){
					for(int col=0;col<7;col++){
						for(int dir=0;dir<2;dir++){
							if(dir==HOR && col>=5)continue;
							if(dir==VERT && row>=5)continue;
							piece_board[encode_piece(row,col,dir,two_by_one)]=get_board(row,col,dir,two_by_one);
						}
					}
				}
				break;
			}
		}
	}
};
#endif /* COMMON_H_ */
