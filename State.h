/*
 * State.h
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#ifndef STATE_H_
#define STATE_H_
#include "Common.h"

class State {
	const State *const prev;
	const RowCol *const pieces;
	const size_t hash;
	const uint16_t g,h;
	uint16_t calculateH() const;
	size_t calculateHash() const;
	const RowCol* getPieces(const string s) const;
	const RowCol* copyPieces(const RowCol* prevPieces) const;
public:
	size_t getHash() const;
	uint16_t getH() const;
	uint16_t getF()const;
	uint16_t getG() const;
	vector<State*> genSuccesors() const;
	bool isGoal() const;
	State(const string s);
	State(const State &s);
	State(const RowCol *pieces, const State* prev);
	string toString()const;
	virtual ~State();
	friend class equal_to<State*>;
	const State* getPrev() const {
		return prev;
	}
};
class StatePointerComparator
{
public:
	StatePointerComparator(){}
	bool operator ()(const State* lhs, const State* rhs) const;
};
class ReverseStatePointerComparator
{
public:
	ReverseStatePointerComparator(){}
	bool operator ()(const State* lhs, const State* rhs) const;
};
namespace std {
template <>struct hash<State*>{
	size_t operator()(const State* s) const{return s->getHash();}
};

template <>struct equal_to<State*>{
	bool operator() (const State* x, const State* y) const{
//		return x->getHash()==y->getHash();
		if(memcmp(x->pieces,y->pieces,sizeof(RowCol)*numPieces)==0){
			return true;
		}
		return false;
	}
};
}


#endif /* STATE_H_ */
