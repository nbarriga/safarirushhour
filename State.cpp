/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"

uint16_t State::calculateH() const{
	uint16_t _h=0;
	if(pieces[letter_index[(uint8_t)'x']].split.row==2){
		if(pieces[letter_index[(uint8_t)'x']].split.col==7){
			_h=0;
		}else{
			_h=1;
		}
	}else{
		_h=2;
	}
//	if(pieces[letter_index[(uint8_t)'x']].split.row==2 &&pieces[letter_index[(uint8_t)'x']].split.col==7){
//        _h=0;
//    }else{
//    _h=abs(pieces[letter_index[(uint8_t)'x']].split.row-2)+(7-pieces[letter_index[(uint8_t)'x']].split.col);
//	}
    for(int p=0;p<numPieces;p++){
		if((p!=letter_index[(uint8_t)'x'])&&(piece_board[pieces[p].packed][index_orientation[p]][index_type[p]]&pre_exit)!=0){
			_h++;
		}
	}
	return _h;
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
const RowCol* State::getPieces(const string s) const{
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
		RowCol* __pieces=new RowCol[size];
		memcpy(__pieces,_pieces,size*sizeof(RowCol));
		return __pieces;
}
//2x2 pieces: termite mounds (u,v) and safari rover (x); 3x1 pieces: elephant (o, q, s) and
//rhino (p, r); 2x1 pieces: lion (a, f),
//lioness (d, g, j), impala (b, h, i, k) and zebra (c, e).
State::State(const string s):prev(NULL), pieces(getPieces(s)),hash(calculateHash()),g(0),h(calculateH()) {

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

State::State(const RowCol* pieces, const State* prev):prev(prev),pieces(pieces),hash(calculateHash()),g(prev==NULL?0:prev->getG()+1),h(calculateH()) {

}
const RowCol* State::copyPieces(const RowCol* prevPieces) const{
	RowCol *_pieces=new RowCol[numPieces];
	memcpy(_pieces,prevPieces,sizeof(RowCol)*numPieces);
	return _pieces;
}
State::State(const State &s):prev(s.prev),pieces(copyPieces(s.pieces)),hash(s.getHash()),g(s.g),h(s.h){

}

uint16_t State::getF() const {
	return getH()+getG();
}

uint16_t State::getG() const {
	return g;
}

size_t State::calculateHash() const {
	size_t _hash=0;
	for(int i=0;i<numPieces;i++){
		boost::hash_combine(_hash,pieces[i].packed);
	}
	return _hash;
}

const string State::getMove(const State* const next) const {
	stringstream ss;
	for(int i=0;i<numPieces;i++){
		if(pieces[i].packed!=next->pieces[i].packed){
			ss<<index_letter[i];
			int thisRow=pieces[i].split.row;
			int nextRow=next->pieces[i].split.row;
			auto rowDiff=thisRow-nextRow;
			if(rowDiff>0){
				ss<<"u"<<rowDiff;
			}else if(rowDiff<0){
				ss<<"d"<<(-rowDiff);
			}else{
				int thisCol=pieces[i].split.col;
				int nextCol=next->pieces[i].split.col;
				auto colDiff=thisCol-nextCol;
				if(colDiff>0){
					ss<<"l"<<colDiff;
				}else if(colDiff<0){
					ss<<"r"<<(-colDiff);
				}else{
					ss<<"pass";
				}
			}
		}
	}
	return ss.str();
}

void State::setG(uint16_t g) {
	this->g=g;
}

State::~State() {
	delete[] pieces;
}

size_t State::getHash() const {
	return hash;
}
bool StatePointerComparator::operator ()(const State* lhs, const State* rhs) const {
	if(lhs->getF() == rhs->getF()){
		return lhs->getG() > rhs->getG();
	}
	return lhs->getF() > rhs->getF();
}
bool ReverseStatePointerComparator::operator ()(const State* lhs, const State* rhs) const {
	if(lhs->getF() == rhs->getF()){
		return lhs->getG() < rhs->getG();
	}
	return lhs->getF() < rhs->getF();
}
