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
	bestState.clear();
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

	auto otherMap = dynamic_cast< TranspositionTableMap*>(other);
	if(other->isSolvable()) {
		setSolvable();
	}
	mapTT.insert(otherMap->mapTT.cbegin(), otherMap->mapTT.cend());

	
}

void TTInterface::TTRegisterBoard(const State *const s, int solutionLength)
{
	//cout << bestLength << endl;
	if(solutionLength > bestLength != (solutionLength==-1 && bestLength==-1))
	{
		bestState = s->toString();
		bestLength = solutionLength;
	

		//cout << "New best found: " << bestLength << endl;
		//if(!bestState.empty()) cout << bestState << endl;
	}	
}

int TTInterface::getBestLength() const
{
	return bestLength;
}
	
string TTInterface::getBestState() const
{
	return bestState;
}

string TranspositionTableMap::statistics() const
{
	return "Size:"+to_string(mapTT.size())+", Has solution:"+to_string(hasSolution);
}

string TranspositionTable::statistics() const{
	return "";
}

void TTInterface::setSolvable()
{
	hasSolution=true;
}

bool TTInterface::isSolvable() const
{
	return hasSolution;
}



