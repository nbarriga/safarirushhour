/*
 * TranspositionTable.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: nbarriga
 */

#include "TranspositionTable.h"

#ifdef ZOBRIST
TranspositionTable::TranspositionTable() {
	TT=new Entry[ttSize];

	//cout<<"TT entry size: "<<sizeof(Entry)<<"B, total "<<sizeof(Entry)*ttSize/1024<<"kB"<<endl;
}
void TranspositionTable::reset(){
	memset(TT,0,sizeof(Entry)*ttSize);
}
TranspositionTable::~TranspositionTable() {
	// TODO Auto-generated destructor stub
	delete[] TT;
}

void TranspositionTable::TTsave(const State* const s) {
	TT[s->getHash()%ttSize]=Entry(s->getHash2(),s->getG());
}

Entry TranspositionTable::TTLookup(const State* const s) const {
	return TT[s->getHash()%ttSize];
}

uint16_t TranspositionTable::TTGetG(const State* const s) const {
	return TT[s->getHash()%ttSize].getG();
}

bool TranspositionTable::exist(const State* const s) const {
	return TT[s->getHash()%ttSize].getHash2()==s->getHash2();
}
#endif
