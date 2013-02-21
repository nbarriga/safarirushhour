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
	//string state;
	uint16_t *pieces;
	uint8_t size;
	uint64_t occupied;
public:
	size_t getHash() const;
	int getH() const;
	vector<State> genSuccesors() const;
	bool isGoal() const;
	State(string s);
	State(const State &s);
	State(uint16_t *pieces,	uint8_t size);
	string toString()const;
	virtual ~State();
};



#endif /* STATE_H_ */
