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


	priority_queue<Node,vector<Node>,NodeComparator> open;
	unordered_set<Node> closed;
public:

	Puzzle();
	virtual ~Puzzle();
};

#endif /* PUZZLE_H_ */
