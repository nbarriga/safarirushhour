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
#ifdef ZOBRIST
	const uint64_t hash, hash2;
	uint64_t calculateZobristHash(int n) const;
#else
	const size_t hash;
	size_t calculateHash() const;
#endif
	uint16_t g;
	const uint16_t h;

	uint16_t calculateH() const;

	const RowCol* getPieces(const string s) const;
	const RowCol* copyPieces(const RowCol* prevPieces) const;
public:
#ifdef ZOBRIST
	uint64_t getHash() const;
	uint64_t getHash2() const;
#else
	size_t getHash() const;
#endif
	uint16_t getH() const;
	uint16_t getF()const;
	uint16_t getG() const;
	void setG(uint16_t g);
	vector<State*> genSuccesors(bool sorted) const;
	bool isGoal() const;
	State(const string s);
	State(const State &s);
	State(const RowCol *pieces, const State* prev);
	string toString()const;
	const string getMove(const State *const next)const;
	virtual ~State();
	friend class equal_to<State*>;
	friend class equal_to<State>;
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
class HighFLowGComparator
{
public:
	HighFLowGComparator(){}
	bool operator ()(const State* lhs, const State* rhs) const;
};
class FGComparator{
	uint16_t f,g;
public:
	FGComparator(uint16_t f,uint16_t g):f(f),g(g){}
	bool operator ()(const State* lhs) const;
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

template <>struct hash<State>{
	size_t operator()(const State& s) const{return s.getHash();}
};
template <>struct equal_to<State>{
	bool operator() (const State& x, const State& y) const{
//		return x->getHash()==y->getHash();
		if(memcmp(x.pieces,y.pieces,sizeof(RowCol)*numPieces)==0){
			return true;
		}
		return false;
	}
};
}

#endif /* STATE_H_ */
