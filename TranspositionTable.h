/*
 * TranspositionTable.h
 *
 *  Created on: Jan 25, 2013
 *      Author: nbarriga
 */

#ifndef TRANSPOSITIONTABLE_H_
#define TRANSPOSITIONTABLE_H_
#include "Common.h"
#include "State.h"

class Entry{
private:
	uint64_t hash2;
	uint16_t g;
public:
	Entry(uint64_t hash2,uint16_t g):
				hash2(hash2),
				g(g){}
	Entry():
		hash2(0),
		g(0){}

	uint64_t getHash2() const {
		return hash2;
	}
	uint16_t getG() const {
			return g;
		}

};

class TranspositionTable {
private:
	Entry *TT;
	unsigned int ttSize=256*1024;
public:
	TranspositionTable();
	virtual ~TranspositionTable();
	void TTsave(const State *const s);
	Entry TTLookup(const State *const s) const;
	uint16_t TTGetG(const State *const s) const;
	void reset();
	bool exist(const State *const s) const;

};

#endif /* TRANSPOSITIONTABLE_H_ */
