#include "State.h"
#include <algorithm>

uint16_t State::getTwoOne(const RowCol& rover) const{
	if(rover.split.row==2){
		return 1;
	}else{
		return 2;
	}
}
uint16_t State::calculateH() const{
	uint16_t _h=0;
	RowCol _rover;
	_rover.packed=pieces[letter_index[(uint8_t)'x']].packed;
	if(_rover.split.row==2&&_rover.split.col==7){
		return 0;
	}
#ifdef GENERATE_RELAXED
	return getTwoOne(_rover);
#endif
	switch(heuristic){
	case zero:
		return 0;
	case twoonezero:
		return getTwoOne(_rover);
	case obstacles0:
		_h+=getTwoOne(_rover);
		if(!rover_on_exit[0][_rover.packed]){
			for(int p=0;p<numPieces;p++){
				if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles[index_type[p]][index_orientation[p]][pieces[p].packed]){
					_h+=1;
				}
			}
		}
		break;
	case obstacles1:
		//_h+=getTwoOne(_rover);
		if(!rover_on_exit[1][_rover.packed]){
			int minVal=INT_MAX;
			for(int i=0;i<3;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles2[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up
					if(_rover.split.row<2){
						count+=2;
					}else{
						count+=3;
					}
					if(_rover.split.col<5){
						count+=1;
					}
					break;
				case 1://left
					if(_rover.split.row==2){
						count+=1;
					}else{
						count+=2;
					}
					break;
				case 2://down
					if(_rover.split.row>2){
						count+=2;
					}else{
						count+=3;
					}
					if(_rover.split.col<5){
						count+=1;
					}
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		}else if(!rover_on_exit[0][_rover.packed]){
			_h+=getTwoOne(_rover);
			for(int p=0;p<numPieces;p++){
				if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles[index_type[p]][index_orientation[p]][pieces[p].packed]){
					_h+=1;
				}
			}
		}else{
			_h+=getTwoOne(_rover);
		}
		break;
	case obstacles2:
		//_h+=getTwoOne(_rover);
		if(!rover_on_exit[2][_rover.packed]){
			int minVal=INT_MAX;
			for(int i=0;i<7;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles3[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up-up
					if(_rover.split.row==0){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 1://up-left
					if(_rover.split.row==1){
						count+=3;
					}else if(_rover.split.row==0 && _rover.split.col==4){
						count+=4;
					}else{
						count+=5;
					}
					break;
				case 2://left-up
					if(_rover.split.row==0 && _rover.split.col==4){
						count+=2;
					}else if(_rover.split.row<=1){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 3://left-left
					if(_rover.split.row==2){
						count+=1;
					}else{
						count+=2;
					}
					break;
				case 4://left-down
					if(_rover.split.row>4 && _rover.split.col==4){
						count+=2;
					}else if(_rover.split.row==4){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 5://down-left
					if(_rover.split.row==4){
						count+=3;
					}else if(_rover.split.row>4 && _rover.split.col==4){
						count+=4;
					}else{
						count+=5;
					}
					break;
				case 6://down-down
					if(_rover.split.row==5){
						count+=3;
					}else{
						count+=4;
					}
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		}else if(!rover_on_exit[1][_rover.packed]){
			//_h+=getTwoOne(_rover);
			int minVal=INT_MAX;
			for(int i=0;i<3;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles2[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up
					count+=2;
					break;
				case 1://left
					count+=1;
					break;
				case 2://down
					count+=2;
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		}else if(!rover_on_exit[0][_rover.packed]){
			_h+=getTwoOne(_rover);
			for(int p=0;p<numPieces;p++){
				if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles[index_type[p]][index_orientation[p]][pieces[p].packed]){
					_h+=1;
				}
			}
		}else{
			_h+=getTwoOne(_rover);
		}
		break;
	case obstacles3:
		//_h+=getTwoOne(_rover);
		if(!rover_on_exit[3][_rover.packed]){
			int minVal=INT_MAX;
			for(int i=0;i<14;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles4[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up-up-left
				case 1://up-left-up
				case 3://left-up-up
					if(_rover.split.row==0){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 2://up-left-left
				case 4://left-up-left
				case 5://left-left-up
					if(_rover.split.row==1){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 6://left-left-left
					if(_rover.split.row==2){
						count+=1;
					}else{
						count+=2;
					}
					break;
				case 7://left-left-down
				case 8://left-down-left
				case 10://down-left-left
					if(_rover.split.row==3){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 9://left-down-down
				case 11://down-left-down
				case 12://down-down-left
					if(_rover.split.row==4){
						count+=3;
					}else{
						count+=4;
					}
					break;
				case 13://down-down-down
					if(_rover.split.row==5){
						count+=3;
					}else{
						count+=4;
					}
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		} else if(!rover_on_exit[2][_rover.packed]){
			//_h+=getTwoOne(_rover);
			int minVal=INT_MAX;
			for(int i=0;i<7;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles3[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up-up
					count+=2;
					break;
				case 1://up-left
					count+=3;
					break;
				case 2://left-up
					count+=2;
					break;
				case 3://left-left
					count+=1;
					break;
				case 4://left-down
					count+=2;
					break;
				case 5://down-left
					count+=3;
					break;
				case 6://down-down
					count+=2;
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		}else if(!rover_on_exit[1][_rover.packed]){
			//_h+=getTwoOne(_rover);
			int minVal=INT_MAX;
			for(int i=0;i<3;i++){
				int count=0;
				for(int p=0;p<numPieces;p++){
					if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles2[i][index_type[p]][index_orientation[p]][pieces[p].packed]){
						count++;
					}
				}
				switch(i){
				case 0://up
					count+=2;
					break;
				case 1://left
					count+=1;
					break;
				case 2://down
					count+=2;
					break;
				}
				if(count<minVal){
					minVal=count;
				}
			}
			_h+=minVal;
		}else if(!rover_on_exit[0][_rover.packed]){
			_h+=getTwoOne(_rover);
			for(int p=0;p<numPieces;p++){
				if((p!=letter_index[(uint8_t)'x'])&&exit_obstacles[index_type[p]][index_orientation[p]][pieces[p].packed]){
					_h+=1;
				}
			}
		}else{
			_h+=getTwoOne(_rover);
		}
		break;
	case relaxed1:
	{
		uint16_t minH=SHRT_MAX;
		for(int p=0;p<numPieces;p++){
			if(p!=letter_index[(uint8_t)'x']){
				int value=relaxationOne[_rover.split.row]
				                        [_rover.split.col]
				                         [pieces[p].split.row]
				                          [pieces[p].split.col]
				                           [index_type[p]]
				                            [index_orientation[p]]
				                             ;
				if(value<minH){
					minH=value;
				}
			}
		}
		_h=minH;
	}
	break;
	case relaxed2:
	{
		uint16_t minH=SHRT_MAX;
		for(int p=0;p<numPieces;p++){
			if(p!=letter_index[(uint8_t)'x']){
				for(int p2=0;p2<numPieces;p2++){
					if(p!=p2&&p2!=letter_index[(uint8_t)'x']){
						int value=relaxationTwo[_rover.split.row]
						                        [_rover.split.col]
						                         [pieces[p].split.row]
						                          [pieces[p].split.col]
						                           [index_type[p]]
						                            [index_orientation[p]]
						                             [pieces[p2].split.row]
						                              [pieces[p2].split.col]
						                               [index_type[p2]]
						                                [index_orientation[p2]]
						                                 ;
						if(value<minH){
							minH=value;
						}
					}
				}
			}
		}
		_h=minH;
	}
	break;
	}



	return _h;
}
uint16_t State::getH() const {
	return h;
}

vector<State*> State::genSuccesors(bool sorted) const{
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

				//calculate zobrist hash incrementally here
				uint64_t _hash=hash^zobrist[pieces[p].packed][index_orientation[p]][index_type[p]][0];
				_hash^=zobrist[new_pieces[p].packed][index_orientation[p]][index_type[p]][0];
				uint64_t _hash2=hash2^zobrist[pieces[p].packed][index_orientation[p]][index_type[p]][1];
				_hash2^=zobrist[new_pieces[p].packed][index_orientation[p]][index_type[p]][1];
				succesors.push_back(new State(new_pieces,this,_hash,_hash2));
			}
		}
	}
	if(sorted){
		sort(succesors.begin(),succesors.end(),ReverseStatePointerComparator());
	}
	return succesors;
}

bool State::isGoal() const{
	auto p=letter_index[(uint8_t)'x'];
	return (piece_board[pieces[p].packed][index_orientation[p]][index_type[p]]&goal)==goal;
}

bool State::isOneMoveFromGoal() const{
	auto r=letter_index[(uint8_t)'x'];
	auto rover_mask=piece_board[pieces[r].packed][index_orientation[r]][index_type[r]];
	if(pieces[r].split.row==2&&!isGoal()){
		for(int p=0;p<numPieces;p++){
			auto piece_mask=piece_board[pieces[p].packed][index_orientation[p]][index_type[p]];
			if((piece_mask&goal_row_positions[pieces[r].split.col+2])==piece_mask){
				return false;
			}
		}
		return true;
	}
	return false;
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
State::State(const string s):prev(NULL), pieces(getPieces(s)),
		hash(calculateZobristHash(0)),hash2(calculateZobristHash(1)),
		g(0),h(calculateH()) {

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
				s[pieces[i].split.row*7+pieces[i].split.col]=index_letter[i];
				s[pieces[i].split.row*7+pieces[i].split.col+1]=index_letter[i];
				s[pieces[i].split.row*7+pieces[i].split.col+2]=index_letter[i];
			}else{
				s[(pieces[i].split.row)*7+pieces[i].split.col]=index_letter[i];
				s[(pieces[i].split.row+1)*7+pieces[i].split.col]=index_letter[i];
				s[(pieces[i].split.row+2)*7+pieces[i].split.col]=index_letter[i];
			}
			break;
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

State::State(const RowCol* pieces, const State* prev):prev(prev),pieces(pieces),
		hash(calculateZobristHash(0)),hash2(calculateZobristHash(1)),
		g(prev==NULL?0:prev->getG()+1),h(calculateH()) {

}
const RowCol* State::copyPieces(const RowCol* prevPieces) const{
	RowCol *_pieces=new RowCol[numPieces];
	memcpy(_pieces,prevPieces,sizeof(RowCol)*numPieces);
	return _pieces;
}
State::State(const State &s):prev(s.prev),pieces(copyPieces(s.pieces)),
		hash(s.hash),hash2(s.hash2),
		g(s.g),h(s.h){
}

uint16_t State::getF() const {
	return getH()+getG();
}

uint16_t State::getG() const {
	return g;
}

uint64_t State::calculateZobristHash(int n) const{
	uint64_t _hash=0;
	for(int i=0;i<numPieces;i++){
		_hash^= zobrist[pieces[i].packed][index_orientation[i]][index_type[i]][n];
	}
	return _hash;;
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

uint64_t State::getHash() const {
	return hash;
}

uint64_t State::getHash2() const {
	return hash2;
}


State::State(const RowCol* pieces, const State* prev, uint64_t hash,
		uint64_t hash2):prev(prev),pieces(pieces),
				hash(hash),hash2(hash2),
				g(prev==NULL?0:prev->getG()+1),h(calculateH()) {

}

void State::setH(uint16_t h) {
	this->h=h;
}

void State::setParent(const State* const parent) {
	prev=parent;
}

State* State::applyMove(const string move) const {
	char letter=move[0];
	char dir=move[1];
	int d=move[2]-'0';

	RowCol* newPieces=new RowCol[numPieces];
	memcpy(newPieces,pieces,numPieces*sizeof(RowCol));

	auto index=letter_index[(uint8_t)letter];

	DIRECTION direction=UP;
	switch(dir){
	case 'u':
		direction=UP;
		break;
	case 'd':
		direction=DOWN;
		break;
	case 'l':
		direction=LEFT;
		break;
	case 'r':
		direction=RIGHT;
		break;
	}

	RowCol newPos=piece_neighbours[direction][d-1][pieces[index].packed][index_type[index]][index_orientation[index]];

	newPieces[index].packed=newPos.packed;
	return new State(newPieces,this);
}

State::~State() {
	delete[] pieces;
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

bool HighFLowGComparator::operator ()(const State* lhs,
		const State* rhs) const {
	if(lhs->getF() == rhs->getF()){
		return lhs->getG() < rhs->getG();
	}
	return lhs->getF() > rhs->getF();
}

bool FGComparator::operator ()(const State* lhs) const {
	return lhs->getG()==g && lhs->getF()==f;
}
namespace std{
bool equal_to<State*>::operator() (const State* x, const State* y) const{
	//return x->getHash()==y->getHash() && x->getHash2()==y->getHash2() ;
	if(memcmp(x->pieces,y->pieces,sizeof(RowCol)*numPieces)==0){
		return true;
	}
	return false;
}
bool equal_to<State>::operator() (const State& x, const State& y) const{
	//return x.getHash()==y.getHash() && x.getHash2()==y.getHash2() ;
	if(memcmp(x.pieces,y.pieces,sizeof(RowCol)*numPieces)==0){
		return true;
	}
	return false;
}
}
