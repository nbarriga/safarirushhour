/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"


int State::getH() const {
	return 0;
}

vector<State> State::genSuccesors() const{
	//for each piece, generate all moves in all directions at all distances
	vector<State> succesors;
	for(int p=0;p<size;p++){
		for(int dir=0;dir<4;dir++){
			for(int distance=1;distance<=7;distance++){
				uint16_t newPiece=piece_neighbours[dir][distance-1][shorten(pieces[p])];
				if(newPiece==illegal_piece)continue;
				//cout<<pieces[p]<<" "<<newPiece<<" "<<shorten(pieces[p])<<" "<<shorten(newPiece)<<endl;
				//cout<<decode_piece(pieces[p]).toString()<<" "<<decode_piece(newPiece).toString()<<endl;
				uint64_t newBoard=piece_board[newPiece];
				if((newBoard&(occupied^piece_board[shorten(pieces[p])]))!=0)continue;
				newPiece|=(pieces[p]&LETTER);
				uint16_t *new_pieces=new uint16_t[size];
				memcpy(new_pieces,pieces,size*sizeof(uint16_t));
				new_pieces[p]=newPiece;
				const State newState(new_pieces,size);
				succesors.push_back(newState);
			}
		}
	}
	return succesors;
}

bool State::isGoal() const{
	//look for rover in pieces
	//check if rover position is == to goal
	return false;
}
//2x2 pieces: termite mounds (u,v) and safari rover (x); 3x1 pieces: elephant (o, q, s) and
//rhino (p, r); 2x1 pieces: lion (a, f),
//lioness (d, g, j), impala (b, h, i, k) and zebra (c, e).
State::State(string s) {
	uint16_t _pieces[19];
	bool seen[256];
	for(int i=0;i<256;i++){
		seen[i]=false;
	}
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
			//printf("%c %d %d %d %d %d\n",s[i],letter_piece[(uint8_t)s[i]], row, col, dir, type);
			//auto piece = decode_piece(_pieces[size]);

			size++;
		}
	}
	pieces=new uint16_t[size];
	memcpy(pieces,_pieces,size*sizeof(uint16_t));
	occupied=0ull;
	for(int p=0;p<size;p++){
		occupied|=piece_board[shorten(pieces[p])];
	}
}

string State::toString() const {
	string s(49,'.');
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
	}
	string formatted;
	for(int i=0;i<7;i++){
		formatted.append(s.substr(7*i,7));
		formatted.append("\n");
	}

	return formatted;
}

State::State(uint16_t* pieces, uint8_t size):pieces(pieces),size(size) {
	occupied=0ull;
	for(int p=0;p<size;p++){
		occupied|=piece_board[shorten(pieces[p])];
	}
}
State::State(const State &s):size(s.size){
	pieces=new uint16_t[size];
	memcpy(pieces,s.pieces,sizeof(uint16_t)*size);
	occupied=0ull;
	for(int p=0;p<size;p++){
		occupied|=piece_board[shorten(pieces[p])];
	}
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
