/*
 * State.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "State.h"

State::State() {
	// TODO Auto-generated constructor stub

}

int State::getH() const {
}

vector<State> State::genSuccesors() const{
}

bool State::isGoal() const{
}

State::~State() {
	// TODO Auto-generated destructor stub
}

 size_t State::getHash() const {
	return hasher(state);
}
