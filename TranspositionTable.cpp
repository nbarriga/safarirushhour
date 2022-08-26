#include "TranspositionTable.h"


TranspositionTable::TranspositionTable(unsigned int ttSize):ttSize(ttSize) {
	TT=new Entry[ttSize];

	//cout<<"TT entry size: "<<sizeof(Entry)<<"B, total "<<sizeof(Entry)*ttSize/1024<<"kB"<<endl;
}

TranspositionTable::~TranspositionTable() {
	delete[] TT;
}

void TranspositionTable::TTsave(const State* const s, uint16_t bound) {
	TT[s->getHash()%ttSize]=Entry(s->getHash2(),bound);
}

Entry TranspositionTable::TTLookup(const State* const s) const {
	return TT[s->getHash()%ttSize];
}


TranspositionTableMap::TranspositionTableMap(unsigned int startingSize){
	mapTT.reserve(startingSize);
}
TranspositionTableMap::~TranspositionTableMap(){

}
void TranspositionTableMap::TTsave(const State *const s, uint16_t bound)  {
	mapTT[s->getHash()]=Entry(s->getHash2(),bound);
}
Entry TranspositionTableMap::TTLookup(const State *const s) const {
	if(mapTT.find(s->getHash())!=mapTT.end()){
		return mapTT.at(s->getHash());
	}else{
		return Entry();
	}
}


