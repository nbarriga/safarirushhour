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
	string reconstruct(const State *end);
public:

	Puzzle(string name, string state);
	virtual ~Puzzle();
	string toString()const;
	string aStar();
};

#endif /* PUZZLE_H_ */
