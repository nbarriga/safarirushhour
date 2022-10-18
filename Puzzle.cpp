#include "Puzzle.h"
#include <algorithm>
#include <cassert>

string Problems, times, memories, nodesSearched, initialH, actualSolution;
vector<BFSCache*> TTs;
int Puzzle::genCalls = 0;

Puzzle::Puzzle(string name, string state):_name(name), _initial(State(state)) {


}

Puzzle::Puzzle(string name, string state, bool order):_name(name), _initial(State(state, order)) {


}

Puzzle::~Puzzle() {
	// TODO Auto-generated destructor stub
}

string Puzzle::toString() const {
	string s("P ");
	s.append(_name);
	s.append("\n");
	string state= _initial.toString();
	s.append(state);
	return s;
}
string Puzzle::reconstruct(const State *const state){
	if(state->getPrev()==NULL){
		return "";
	}else{
		return reconstruct(state->getPrev())+" "+state->getPrev()->getMove(state);
	}
}
int Puzzle::aStar(){

	equal_to<State*> stateEqual;
	vector<State*> open_queue;
	StatePointerComparator comp;
	ReverseStatePointerComparator revComp;

	unordered_set<State*> open;
	unordered_set<State*> closed;
	State *initial= new State(_initial);
	open_queue.push_back(initial);
	std::make_heap(open_queue.begin(), open_queue.end(),comp);


	open.insert(initial);

	nodes=0;


	while (!open_queue.empty()) {

		State *current= open_queue.front();
		std::pop_heap(open_queue.begin(), open_queue.end(),comp);
		open_queue.pop_back();
		open.erase(current);


		if (current->isGoal()) {
			solution=reconstruct(current);;
			int solutionLength=current->getG();
			Problems.append("'").append(_name).append("',");

			for(auto it=open.begin();it!=open.end();it++){
				delete *it;
			}
			for(auto it=closed.begin();it!=closed.end();it++){
				delete *it;
			}
			delete current;
			open.clear();
			closed.clear();

			return solutionLength;
		}
		const vector<State*>& children =current->genSuccesors(false);

		for (auto it=children.begin();it!=children.end();it++){
			nodes++;
			if(nodes%5000==0){
				timeOut=chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() -start).count()>timeArg*1000;
			
				if(timeOut){
					threshold=current->getF();
					for(auto toDel=it;toDel!=children.end();toDel++){
						delete (*toDel);
					}
					for(auto it=open.begin();it!=open.end();it++){
						delete *it;
					}
					for(auto it=closed.begin();it!=closed.end();it++){
						delete *it;
					}
					delete current;
					open.clear();
					closed.clear();


					return -1;
				}
			}
			bool updatedInOpen=false;

			auto stored = closed.find(*it);
			if(stored!=closed.end()){
				if((*stored)->getG()<=(*it)->getG()){
					delete *it;
					continue;
				}
				auto toDel=*stored;//needed because iterator gets invalidated on next line
				closed.erase(*it); // re-open node to
				delete toDel;
			}

			stored = open.find(*it);
			if(stored!=open.end() ){
				if((*stored)->getG()<=(*it)->getG()){
					delete *it;
					continue;
				}

				//check in queue and delete
				for(auto toUpdate=open_queue.begin();toUpdate!=open_queue.end();toUpdate++){
					if(stateEqual(*toUpdate,*it)){
						updatedInOpen=true;
						assert((*stored)==(*toUpdate));
						(*toUpdate)->setG((*it)->getG());
						(*toUpdate)->setParent((*it)->getPrev());
						std::push_heap(open_queue.begin(), toUpdate+1,comp);
						delete *it;
						break;
					}
				}

			}
			if(!updatedInOpen){
				open.insert(*it); // fast, but nodes may appear multiple times
				open_queue.push_back(*it);
				std::push_heap(open_queue.begin(), open_queue.end(),comp);
			}
		}
		closed.insert(current);

	}
	for(auto it=closed.begin();it!=closed.end();it++){
		delete (*it);
	}
	closed.clear();
	solution="Can't find answer";
	return -1;
}

int Puzzle::innerIdaStar(State* s, int depth, uint16_t bound) {
	nodes++;
	if(nodes%5000==0){
		if(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() -start).count()>timeArg*1000){
			timeOut=true;
			return -1;
		}
	}

	if (s->isGoal()){// goal found
		done=true;
		path[depth]=new State(*s);
		TT->TTsave(s, 0);
		return 0;
	}

	

	vector<State*> children=s->genSuccesors(false);//todo: avoid copy


	for (auto it=children.begin(); it!=children.end();it++) {
		auto entry=TT->TTLookup(*it);
		genCalls++;
		if (entry.getHash2()==(*it)->getHash2()){
				auto newH=entry.getBound();
				if(newH>(*it)->getH()){
					if(depth<cutMin){
						cutMin=depth;
					}
					if(depth>cutMax){
						cutMax=depth;
					}
					cutTotal+=depth;
					cutCount++;
				}
				(*it)->setH(newH);

			}
		}

	//Sorting needs to reduce number of nodes a bit, but it's slower
 	//sort(children.begin(), children.end(),ReverseStatePointerComparator());



	uint16_t new_bound=SHRT_MAX;

	for (auto it=children.begin(); it!=children.end();it++) {
		if(timeOut){
			for(auto toDel=it;toDel!=children.end();toDel++){
				delete (*toDel);
			}
			return -1;
		}

		uint16_t t;
		totalH+=(*it)->getH();
		countH++;
		if ((*it)->getH() <= bound-1) {
			t = 1 + innerIdaStar(*it, depth+1, bound - 1);
		} else {
			t = 1 + (*it)->getH();
		}
		delete (*it);
		if (done) {
			path[depth]=new State(*s);
			for(auto toDel=it+1;toDel!=children.end();toDel++){
				delete (*toDel);
			}
			return t;
		}

		if (t < new_bound) {
			new_bound = t ; // better lower bound
		}
	}


	solution="Can't find answer";
	TT->TTsave(s,new_bound);
	return new_bound;
}

string Puzzle::reconstruct(int endPos) {
	stringstream ss;
	for(int i=0;i<endPos;i++){
		ss<<" "<<path[i]->getMove(path[i+1]);
	}
	return ss.str();
}

Puzzle::Puzzle(string name,const State& state):_name(name), _initial(state) {
}

void Puzzle::init() {
	start=chrono::high_resolution_clock::now();
	timeOut=false;
	nodes=0;
	threshold=0;
}

void Puzzle::setTTForReuse(TTInterface*_TT){
	TT= _TT;
}

int Puzzle::idaStar(unsigned int ttSize, unsigned int maxSolLength) {
	if (!clusterCache||TT==nullptr){
			if(useMapTT) TT=new TranspositionTableMap(ttSize);
			else TT=new TranspositionTable(ttSize);
	}
	

	path=new State*[maxSolLength];
	threshold = _initial.getH();

	//Let's see if we can improve threshold it from previous searches
	if(clusterCache){
		auto entry=TT->TTLookup(&_initial);
		genCalls++;
		if (entry.getHash2()==_initial.getHash2()){ 
			auto newH=entry.getBound();
			if(newH>_initial.getH()){
				threshold = newH;
			}
		}	
		
	}
	done = false;
	int moves;

	while (!done) {
		moves = innerIdaStar(&_initial,0, threshold);
		if (!done){
			threshold = moves;
			if(threshold>maxSolLength){
				moves=-1;
				break;
			}
		}
		if(moves<0){
			break;
		}
	}
	if(moves>=0){
		solution=reconstruct(moves);
		for(int i=0;i<=moves;i++){
			delete path[i];
		}
	}
	Problems.append("'").append(_name).append("',");
	if(!clusterCache){
		delete TT;
	}
	delete[] path;
	return moves;
}

void Puzzle::BFS(unsigned int ttSize)
{
	assert(clusterCache && connectedComponents);
	
	foundInPreviousTT = false;

	if(Lookup(&_initial)){
		return;
	}
	else
	{
		bfsTT = new BFSCache(ttSize);
	}

	

	//innerBFS(&_initial);
	std::unordered_set<State*, hash<State*>, equal_to<State*>> set;

	std::deque<State*> queue;
	State* copy = new State(_initial);
	set.insert(copy); queue.push_back(copy);
	while(!queue.empty())
	{
		auto s = queue.front();
		queue.pop_front();
		set.erase(s);

		if(s->isGoal())
		{
			delete s;
			bfsTT->setSolvable();
			continue;
		}

		if(Lookup(s))
		{
			delete s;
			continue;
		}
		
		bfsTT->save(s);
		vector<State*> children=(s)->genSuccesors(false);//todo: avoid copy
		for (auto it=children.begin(); it!=children.end();it++) {
			genCalls++;
			if(!Lookup(*it) && (set.count(*it) <= 0))
			{
				queue.push_back(*it); set.insert(*it);
			}
			else
			{
				delete *it;
			}
		}
		//delete s;
	}
	if(find(TTs.begin(), TTs.end(), bfsTT) == TTs.end())
	{
		TTs.push_back(bfsTT);
	}
}

void Puzzle::innerBFS(const State *s)
{
	if(s->isGoal())
	{
		bfsTT->setSolvable();
		return;
	}

	if(Lookup(s))
	{
		return;
	}

	bfsTT->save(s);

	vector<State*> children=s->genSuccesors(false);//todo: avoid copy

	for (auto it=children.begin(); it!=children.end();it++) {
		innerBFS(*it);
		//delete *it;
	}


}


bool Puzzle::Lookup(const State * s)
{
	if(bfsTT!=nullptr && bfsTT->lookup(s)){ 
		return true;
	}

	if(!foundInPreviousTT)
	{
		for(auto otherTT : TTs)
		{
			if(otherTT->lookup(s))
			{
				if(bfsTT != nullptr){
					otherTT->merge(bfsTT);
					delete bfsTT;
				}
				bfsTT = otherTT;
				foundInPreviousTT = true;
				return true;
			}
		}		
	}
	else
	{
		for(auto it = TTs.begin(); it != TTs.end(); it++)
		{
			if(*it == bfsTT) continue; // self
			if((*it)->lookup(s))
			{
				bfsTT->merge(*it);
				TTs.erase(it);
				return true;
			}
		}
			
	}

	return false;
}


string Puzzle::statistics() {
	// nodes searched,
	//execution time,
	//average h-value for all nodes where an evaluation occurred in the tree,
	//and min/average/max depth of nodes where a cutoff occurred
	stringstream ss;
	ss<<"Nodes: "<<nodes<<", average H: "<<totalH/(float)countH<<", cut-off depth (min/average/max): "<<
			cutMin<<"/"<<cutTotal/(float)cutCount<<"/"<<cutMax;

	return ss.str();
}
