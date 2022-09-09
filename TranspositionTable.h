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
	protected:
		string bestState;
		int bestLength = -1;
		bool hasSolution=false;
	public:
		
		int getBestLength() const;
		string getBestState() const;
		void setSolvable();
		bool isSolvable() const;
		virtual ~TTInterface(){};
		virtual void TTsave(const State *const s, uint16_t bound) = 0;
		virtual Entry TTLookup(const State *const s) const = 0;
		virtual void TTmerge( TTInterface * other) = 0;
		virtual void TTRegisterBoard(const State *const s, int solutionLength);
		virtual string statistics() const = 0;
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
	virtual void TTmerge( TTInterface * other);
	virtual string statistics() const;
};

class TranspositionTableMap : public TTInterface{ 
	std::unordered_map<uint64_t,Entry> mapTT;
	public:
	TranspositionTableMap(unsigned int startingSize);
	virtual ~TranspositionTableMap();
	virtual void TTsave(const State *const s, uint16_t bound);
	virtual Entry TTLookup(const State *const s) const;
	virtual void TTmerge(TTInterface * other);
	virtual string statistics() const;
	
};

#endif /* TRANSPOSITIONTABLE_H_ */
