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
#include<unordered_map>


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

class TTInterface
{
	public:
		virtual ~TTInterface(){};
		virtual void TTsave(const State *const s, uint16_t bound) = 0;
		virtual Entry TTLookup(const State *const s) const = 0;
		virtual uint16_t TTGetBound(const State *const s) const = 0;
		virtual void reset() = 0;
		virtual bool exist(const State *const s) const = 0;

};

class TranspositionTable : public TTInterface {
private:
	Entry *TT;
	unsigned int ttSize;
public:
	TranspositionTable(unsigned int ttSize);
	virtual ~TranspositionTable();
	virtual void TTsave(const State *const s, uint16_t bound);
	virtual Entry TTLookup(const State *const s) const;
	virtual uint16_t TTGetBound(const State *const s) const;
	virtual void reset();
	virtual bool exist(const State *const s) const;

};

class TranspositionTableMap : public TTInterface{ 
	std::unordered_map<uint64_t,uint16_t> mapTT;
	public:
	TranspositionTableMap();
	virtual ~TranspositionTableMap();
	virtual void TTsave(const State *const s, uint16_t bound);
	virtual Entry TTLookup(const State *const s) const;
	virtual uint16_t TTGetBound(const State *const s) const;
	virtual void reset();
	virtual bool exist(const State *const s) const;
};

#endif /* TRANSPOSITIONTABLE_H_ */
