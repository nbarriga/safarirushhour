/*
 * Puzzle.h
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#ifndef PUZZLE_H_
#define PUZZLE_H_
#include "Common.h"
#include "Node.h"
#include "State.h"

class Puzzle {

	string _name;
	State _initial;
	priority_queue<Node,vector<Node>,NodeComparator> open;
	unordered_set<Node> closed;
public:

	Puzzle(string name, string state);
	virtual ~Puzzle();
	string toString()const;
};

#endif /* PUZZLE_H_ */
