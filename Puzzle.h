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
	TranspositionTable *TT;
	string solution;
	boost::posix_time::ptime start;
	float memFactor=(sizeof(State)+sizeof(RowCol)*numPieces+11*sizeof(State*))/1024.0f/1024.0f;
	int baseMemory;
	uint32_t maxLiveNodes;
public:
	void init(int baseMemory);
	Puzzle(string name, string state);
	Puzzle(string name, const State& state);
	virtual ~Puzzle();
	string toString()const;
	int aStar(int memoryLimit);
	uint16_t getInitialH() const{
		return _initial.getH();
	}
	int idaStar(unsigned int ttSize);

	const string& getSolution() const {
		return solution;
	}

	bool isTimeOut() const {
		return timeOut;
	}
};

#endif /* PUZZLE_H_ */
