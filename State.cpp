/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"


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
	int i=0;
	uint8_t row,col,dir,type;
	for(int i=0;i<49;i++){
		type=piece_type[s[i]];
		col=i%7;
		row=i/7;
		if(s[i]==s[i+1]){//square pieces are horizontal, who cares
			dir=0;//horizontal
		}else{
			dir=1;//vertical
		}
		_pieces[i]=(row<<6) | (col<<3) | (dir<<2) | (type);
		i++;
	}
	size=i;
	pieces=new uint16_t[size];
	memcpy(pieces,_pieces,size*sizeof(uint16_t));
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
