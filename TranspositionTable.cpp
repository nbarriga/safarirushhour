/*
 * TranspositionTable.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: nbarriga
 */

#include "TranspositionTable.h"


TranspositionTable::TranspositionTable(unsigned int ttSize):ttSize(ttSize) {
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

void TranspositionTable::TTsave(const State* const s, uint16_t bound) {
	TT[s->getHash()%ttSize]=Entry(s->getHash2(),bound);
}

Entry TranspositionTable::TTLookup(const State* const s) const {
	return TT[s->getHash()%ttSize];
}

uint16_t TranspositionTable::TTGetBound(const State* const s) const {
	return TT[s->getHash()%ttSize].getBound();
}

bool TranspositionTable::exist(const State* const s) const {
	return TT[s->getHash()%ttSize].getHash2()==s->getHash2();

}

TranspositionTableMap::TranspositionTableMap(){

}
TranspositionTableMap::~TranspositionTableMap(){

}
void TranspositionTableMap::TTsave(const State *const s, uint16_t bound)  {
	mapTT[s->getHash2()]=bound;
}
Entry TranspositionTableMap::TTLookup(const State *const s) const {
	try{
		return Entry(s->getHash2(),mapTT.at(s->getHash2()));
	}catch(std::out_of_range &e)
	{
		cout << "Out of range lookup" << endl;
		return Entry();
	}
}
uint16_t TranspositionTableMap::TTGetBound(const State *const s) const { 
	try{
		return mapTT.at(s->getHash2());
	}catch(std::out_of_range &e)
	{
		cout << "Out of range getbound" << endl;
		return SHRT_MAX;
	}
}
void TranspositionTableMap::reset() {
	mapTT.clear();
}
bool TranspositionTableMap::exist(const State *const s) const {
	return mapTT.count(s->getHash2())>0;
}

