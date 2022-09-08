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

void TranspositionTable::TTmerge(TTInterface* other)
{
	throw std::logic_error("Not Implemented");
}


TranspositionTableMap::TranspositionTableMap(unsigned int startingSize){
	//mapTT.reserve(startingSize);
}
TranspositionTableMap::~TranspositionTableMap(){
	mapTT.clear();
	delete bestState;
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

void TranspositionTableMap::TTmerge(TTInterface* other)
{
	//mapTT.merge(other.mapTT);
	cout << "Merge." << endl;

	auto otherMap = dynamic_cast< TranspositionTableMap*>(other);

	mapTT.insert(otherMap->mapTT.cbegin(), otherMap->mapTT.cend());

	
}

void TTInterface::TTRegisterBoard(const State *const s, int solutionLength)
{
	cout << bestLength << endl;
	if(solutionLength > bestLength != (solutionLength==-1 && bestLength==-1))
	{
		if(bestState != nullptr) delete bestState;
		bestState = new State(*s);
		bestLength = solutionLength;
	

		cout << "New best found: " << bestLength << endl;
		if(bestState!=nullptr)	cout << bestState->toString() << endl;
	}	
}

int TTInterface::getBestLength() const
{
	return bestLength;
}
	
State* TTInterface::getBestState() const
{
	return bestState;
}



