#ifndef PUZZLE_H_
#define PUZZLE_H_
#include "Common.h"
#include "State.h"
#include "TranspositionTable.h"

extern thread_local vector<BFSCache*> TTs;
extern thread_local BFSCache * bfsTT;	

class Puzzle {

	string _name;
	State _initial;
	string reconstruct(const State *const end);
	string reconstruct(int endPos);
	int innerIdaStar(State* s, int depth, uint16_t bound);
	void innerBFS(const State *s);
	uint16_t threshold;
	uint64_t nodes;
	static int genCalls;
	bool done,timeOut;
	State** path;
	TTInterface *TT=nullptr;
	
	bool foundInPreviousTT=false;
	string solution;
	chrono::time_point<chrono::high_resolution_clock> start;
	float memFactor=(sizeof(State)+sizeof(RowCol)*numPieces+11*sizeof(State*)+16)/1024.0f/1024.0f;
	int64_t totalH=0, countH=0,cutMin=INT_MAX,cutTotal=0,cutCount=0,cutMax=0;
public:
	void init();
	Puzzle(string name, string state);
	Puzzle(string name, string state, bool order);
	Puzzle(string name, const State& state);
	virtual ~Puzzle();
	string toString()const;
	int aStar();
	void BFS(unsigned int ttSize);
	int beamSearch(int beamWidth);
	void setTTForReuse(TTInterface *_TT);

	int idaStar(unsigned int ttSize, unsigned int maxSolLength);
	int rbfs(unsigned int ttSize, unsigned int maxSolLength);
	int innerRbfs(State* s, int depth, uint16_t f_limit, uint16_t node_f, uint16_t& out_f);

	bool Lookup(const State *s);

	const string& getSolution() const {
		return solution;
	}

	bool isTimeOut() const {
		return timeOut;
	}

	string statistics();
};

#endif /* PUZZLE_H_ */
