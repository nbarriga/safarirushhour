/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"

void State::calculateH(){
	h=0;
//	if(pieces[0].split.row==2){
//		h=1;
//	}else{
//		h=2;
//	}
//	for(int p=1;p<numPieces;p++){
//		if((piece_board[pieces[p].packed][index_orientation[p]][index_type[p]]&pre_exit)!=0){
//			h++;
//		}
//	}
}
uint16_t State::getH() const {
	return h;
}

vector<State*> State::genSuccesors() const{
	//for each piece, generate all moves in all directions at all distances
	vector<State*> succesors;
	uint64_t occupied=0;
	occupied=0ull;
	for(int p=0;p<numPieces;p++){
		occupied|=piece_board[pieces[p].packed][index_orientation[p]][index_type[p]];
	}
	for(int p=0;p<numPieces;p++){
		uint64_t occupiedNoCurrent=occupied&(~piece_board[pieces[p].packed][index_orientation[p]][index_type[p]]);
		for(int dir=0;dir<4;dir++){
			for(int distance=1;distance<=7;distance++){
				RowCol newPos=piece_neighbours[dir][distance-1][pieces[p].packed][index_type[p]][index_orientation[p]];

				if(newPos.packed==illegal_pos){
					continue;
				}
				if((occupiedNoCurrent&
						piece_board_paths[index_type[p]][pieces[p].packed][newPos.packed])!=0){
					continue;
				}
				RowCol *new_pieces=new RowCol[numPieces];
				memcpy(new_pieces,pieces,numPieces*sizeof(RowCol));
				new_pieces[p].packed=newPos.packed;
				succesors.push_back(new State(new_pieces,this));
			}
		}
	}
	return succesors;
}

bool State::isGoal() const{
	auto p=letter_index[(uint8_t)'x'];
	return (piece_board[pieces[p].packed][index_orientation[p]][index_type[p]]&goal)==goal;
}
//2x2 pieces: termite mounds (u,v) and safari rover (x); 3x1 pieces: elephant (o, q, s) and
//rhino (p, r); 2x1 pieces: lion (a, f),
//lioness (d, g, j), impala (b, h, i, k) and zebra (c, e).
State::State(string s):prev(NULL), g(0) {
	RowCol _pieces[19];
	bool seen[256];
	for(int i=0;i<256;i++){
		seen[i]=false;
	}
	ORIENTATION dir;
	PIECE_TYPE type;
	RowCol rowCol;
	int size=0;
	for(int i=0;i<49;i++){
		if(s[i]!='.' && !seen[(uint8_t)s[i]]){
			seen[(uint8_t)s[i]]=true;
			type=piece_type[(uint8_t)s[i]];
			rowCol.split.col=i%7;
			rowCol.split.row=i/7;
			if(s[i]==s[i+1]){//square pieces are horizontal, who cares
				dir=HOR;//horizontal
			}else{
				dir=VERT;//vertical
			}
			_pieces[size]=rowCol;
			index_letter[size]=s[i];
			letter_index[(uint8_t)s[i]]=size;
			index_type[size]=type;
			index_orientation[size]=dir;
			size++;
		}
	}
	numPieces=size;
	pieces=new RowCol[size];
	memcpy(pieces,_pieces,size*sizeof(RowCol));
	calculateH();
}

string State::toString() const {
	string s(49,'.');
	for(int i=0;i<numPieces;i++){
		switch(index_type[i]){
		case rover:
			if(pieces[i].split.col<7){
				s[pieces[i].split.row*7+pieces[i].split.col]=index_letter[i];
				s[(pieces[i].split.row+1)*7+pieces[i].split.col]=index_letter[i];
				if(pieces[i].split.col<6){
					s[pieces[i].split.row*7+pieces[i].split.col+1]=index_letter[i];
					s[(pieces[i].split.row+1)*7+pieces[i].split.col+1]=index_letter[i];
				}
			}
			break;
		case two_by_two:
			s[pieces[i].split.row*7+pieces[i].split.col]=index_letter[i];
			s[pieces[i].split.row*7+pieces[i].split.col+1]=index_letter[i];
			s[(pieces[i].split.row+1)*7+pieces[i].split.col]=index_letter[i];
			s[(pieces[i].split.row+1)*7+pieces[i].split.col+1]=index_letter[i];
			break;
		case three_by_one:
			if(index_orientation[i]==HOR){
				s[pieces[i].split.row*7+pieces[i].split.col+2]=index_letter[i];
			}else{
				s[(pieces[i].split.row+2)*7+pieces[i].split.col]=index_letter[i];
			}
		case two_by_one:
			if(index_orientation[i]==HOR){
				s[pieces[i].split.row*7+pieces[i].split.col]=index_letter[i];
				s[pieces[i].split.row*7+pieces[i].split.col+1]=index_letter[i];
			}else{
				s[pieces[i].split.row*7+pieces[i].split.col]=index_letter[i];
				s[(pieces[i].split.row+1)*7+pieces[i].split.col]=index_letter[i];
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

State::State(RowCol* pieces, const State* prev):prev(prev),g(prev==NULL?0:prev->getG()+1),pieces(pieces) {
	calculateH();
}
State::State(const State &s):prev(s.prev),g(s.g),h(s.h){
//	cout<<"State copy constructor!"<<endl;
	pieces=new RowCol[numPieces];
	memcpy(pieces,s.pieces,sizeof(RowCol)*numPieces);
}

uint16_t State::getF() const {
	return getH()+getG();
}

uint16_t State::getG() const {
	return g;
}

State::~State() {
	delete[] pieces;
}

size_t State::getHash() const {
	size_t hash=0;
	for(int i=0;i<numPieces;i++){
		boost::hash_combine(hash,pieces[i].packed);
	}
	return hash;
}
bool StatePointerComparator::operator ()(const State* lhs, const State* rhs) const {
	return lhs->getF() > rhs->getF();
}
