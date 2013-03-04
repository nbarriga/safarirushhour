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
	uint16_t bound;
public:
	Entry(uint64_t hash2,uint16_t bound):
				hash2(hash2),
				bound(bound){}
	Entry():
		hash2(0),
		bound(SHRT_MAX){}

	uint64_t getHash2() const {
		return hash2;
	}
	uint16_t getBound() const {
			return bound;
		}

};

class TranspositionTable {
private:
	Entry *TT;
	unsigned int ttSize=256*1024;
public:
	TranspositionTable();
	virtual ~TranspositionTable();
	void TTsave(const State *const s, uint16_t bound);
	Entry TTLookup(const State *const s) const;
	uint16_t TTGetBound(const State *const s) const;
	void reset();
	bool exist(const State *const s) const;

};

#endif /* TRANSPOSITIONTABLE_H_ */
