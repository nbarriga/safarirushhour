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
	string state;
	static hash<string> hasher;
public:
	size_t getHash() const;
	int getH() const;
	vector<State> genSuccesors() const;
	bool isGoal() const;
	State();

	virtual ~State();
};



#endif /* STATE_H_ */
