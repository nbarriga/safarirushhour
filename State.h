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
	const State *prev;
	uint16_t g,h;
	uint16_t *pieces;
	uint8_t size;
	void calculateH();
public:
	size_t getHash() const;
	uint16_t getH() const;
	uint16_t getF()const;
	uint16_t getG() const;
	vector<State*> genSuccesors() const;
	bool isGoal() const;
	State(string s);
	State(const State &s);
	State(uint16_t *pieces,	uint8_t size, const State* prev);
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
namespace std {
template <>struct hash<State*>{
	size_t operator()(const State* s) const{return s->getHash();}
};

template <>struct equal_to<State*>{
	bool operator() (const State* x, const State* y) const{
		if(x->size==y->size){
			if(memcmp(x->pieces,y->pieces,sizeof(uint16_t)*x->size)==0){
				return true;
			}
		}return false;
	}
};
}


#endif /* STATE_H_ */
