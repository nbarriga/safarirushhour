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
#include "TranspositionTable.h"

class Puzzle {

	string _name;
	State _initial;
	string reconstruct(const State *const end);
	string reconstruct(int endPos);
	int innerIdaStar(State* s, int depth, uint16_t bound);
	uint16_t threshold,f_min;
	uint64_t nodes;
	bool done,timeOut;
	State* path[1000];
	TranspositionTable TT;
	string solution;
	boost::posix_time::ptime start;
public:

	Puzzle(string name, string state);
	Puzzle(string name, const State& state);
	virtual ~Puzzle();
	string toString()const;
	int aStar();
	uint16_t getInitialH() const{
		return _initial.getH();
	}
	int idaStar();

	const string& getSolution() const {
		return solution;
	}
};

#endif /* PUZZLE_H_ */
