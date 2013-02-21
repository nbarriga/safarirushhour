/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"

_dummy_static_initializer _dummy;
int State::getH() const {
}

vector<State> State::genSuccesors() const{
}

bool State::isGoal() const{
}
//2x2 pieces: termite mounds (u,v) and safari rover (x); 3x1 pieces: elephant (o, q, s) and
//rhino (p, r); 2x1 pieces: lion (a, f),
//lioness (d, g, j), impala (b, h, i, k) and zebra (c, e).
State::State(string s) {
	uint16_t _pieces[19];
	bool seen[256];
	int i=0;
	uint16_t row,col,dir,type;
	size=0;
	for(int i=0;i<49;i++){
		if(s[i]!='.' && !seen[(uint8_t)s[i]]){
			seen[(uint8_t)s[i]]=true;
			type=piece_type[(uint8_t)s[i]];
			col=i%7;
			row=i/7;
			if(s[i]==s[i+1]){//square pieces are horizontal, who cares
				dir=HOR;//horizontal
			}else{
				dir=VERT;//vertical
			}
			_pieces[size]=encode_piece(letter_piece[(uint8_t)s[i]], row, col, dir, type);
			printf("%c %d %d %d %d %d\n",s[i],letter_piece[(uint8_t)s[i]], row, col, dir, type);
			auto piece = decode_piece(_pieces[size]);

			size++;
		}
	}
	pieces=new uint16_t[size];
	memcpy(pieces,_pieces,size*sizeof(uint16_t));
}

string State::toString() const {
	string s(49,'.');
	uint64_t b=0;
	for(int i=0;i<size;i++){
		auto piece = decode_piece(pieces[i]);
		//printf("%c %d %d %d %d %d\n",piece.letter,((pieces[size]>>9)&31), piece.row, piece.col, piece.dir, piece.type);
		switch(piece.type){
		case rover:
		case two_by_two:
			s[piece.row*7+piece.col]=piece.letter;
			s[piece.row*7+piece.col+1]=piece.letter;
			s[(piece.row+1)*7+piece.col]=piece.letter;
			s[(piece.row+1)*7+piece.col+1]=piece.letter;
			break;
		case three_by_one:
			if(piece.dir==HOR){
				s[piece.row*7+piece.col+2]=piece.letter;
			}else{
				s[(piece.row+2)*7+piece.col]=piece.letter;
			}
		case two_by_one:
			if(piece.dir==HOR){
				s[piece.row*7+piece.col]=piece.letter;
				s[piece.row*7+piece.col+1]=piece.letter;
			}else{
				s[piece.row*7+piece.col]=piece.letter;
				s[(piece.row+1)*7+piece.col]=piece.letter;
			}
			break;
		}
		//cout<<piece.letter<<" "<<shorten(pieces[i])<<" "<<bitset<64>(piece_board[shorten(pieces[i])])<<endl;
	b|=piece_board[shorten(pieces[i])];
	}
	printBoard(b);
	return s;
}

State::~State() {
	delete[] pieces;
}

size_t State::getHash() const {
	size_t hash=0;
	for(int i=0;i<size;i++){
		boost::hash_combine(hash,pieces[i]);
	}
	return hash;
}
