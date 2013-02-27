/*
 * Puzzle.h
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#ifndef PUZZLE_H_
#define PUZZLE_H_
#include "Common.h"
#include "State.h"

class Puzzle {

	string _name;
	State _initial;
	string reconstruct(const State *const end);
	string reconstruct(int endPos);
	string innerIdaStar(State* s, int depth);
	uint16_t threshold,f_min;
	uint64_t nodes;
	bool done;
	State* path[1000];
	unordered_set<State> visited;
public:

	Puzzle(string name, string state);
	virtual ~Puzzle();
	string toString()const;
	string aStar();

	string idaStar();
};

#endif /* PUZZLE_H_ */
