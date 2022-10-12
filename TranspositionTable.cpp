#include "TranspositionTable.h"


TranspositionTable::TranspositionTable(unsigned int ttSize):ttSize(ttSize) {
	TT=new Entry[ttSize];
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
	if(other->isSolvable()) setSolvable();
	throw std::logic_error("Not Implemented");
}

string TranspositionTable::statistics() const{
	return "";
}




TranspositionTableMap::TranspositionTableMap(unsigned int startingSize){

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

	auto otherMap = dynamic_cast< TranspositionTableMap*>(other);
	if(other->isSolvable()) {
		setSolvable();
	}
	mapTT.insert(otherMap->mapTT.cbegin(), otherMap->mapTT.cend());

	
}
string TranspositionTableMap::statistics() const
{
	return to_string(mapTT.size())+";"+to_string(hasSolution);
}


BFSCache::BFSCache(unsigned int startingSize){

}
BFSCache::~BFSCache(){
	setTT.clear();
}
void BFSCache::save(const State *const s){
	setTT.insert(std::make_pair(s->getHash(), s->getHash2()));
}

bool BFSCache::lookup(const State *const s) const{
	return setTT.find(std::make_pair(s->getHash(), s->getHash2())) != setTT.end();

}
void BFSCache::merge(BFSCache * other){
	if(other->isSolvable()){
		setSolvable();
	}
	setTT.insert(other->setTT.cbegin(), other->setTT.cend());
}
string BFSCache::statistics() const{
	return to_string(setTT.size())+";"+to_string(hasSolution);
}

int TTInterface::getBestLength() const
{
	return bestLength;
}
	
string TTInterface::getBestState() const
{
	return bestState;
}


void TTInterface::setSolvable()
{
	hasSolution=true;
}

bool TTInterface::isSolvable() const
{
	return hasSolution;
}

void BFSCache::setSolvable()
{
	hasSolution=true;
}

bool BFSCache::isSolvable() const
{
	return hasSolution;
}



