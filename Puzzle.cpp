#include "Puzzle.h"
#include <algorithm>
#include <cassert>

namespace {
// Stable identity key for tracking active queue entries by pointer instance.
inline uintptr_t ptrKey(const State *s) {
	return reinterpret_cast<uintptr_t>(s);
}

// Replays the move list from the initial board and checks length + goal.
bool isLegalSolutionPath(const State &initial, const string &moves,
												int expectedLength) {
	// Defensive validation: replay reconstructed moves before accepting a goal.
	State *cursor = new State(initial);
	stringstream ss(moves);
	string move;
	int actualLength = 0;
	while (ss >> move) {
		State *next = cursor->applyMove(move);
		delete cursor;
		cursor = next;
		actualLength++;
	}

	bool ok = actualLength == expectedLength && cursor->isGoal();
	delete cursor;
	return ok;
}
} // namespace

thread_local string Problems, times, memories, nodesSearched, initialH, actualSolution;
thread_local vector<BFSCache*> TTs;
thread_local BFSCache* bfsTT = nullptr;
int Puzzle::genCalls = 0;
//BFSCache * bfsTT=nullptr;	
//bool foundInPreviousTT=false;

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
	start = chrono::high_resolution_clock::now();

	// Min-heap (via custom comparator) used as A* open queue.
	vector<State*> open_queue;
	StatePointerComparator comp;

	// Content-equality sets for best-known open/closed states.
	unordered_set<State*> open;
	// Pointer-identity set to detect stale heap entries in open_queue.
	unordered_set<uintptr_t> openByAddress;
	unordered_set<State*> closed;

	auto cleanup = [&](State *current) {
		// Collect unique pointers across all containers to avoid double frees.
		unordered_set<State*> toDelete;
		if (current != nullptr) {
			toDelete.insert(current);
		}
		for (auto s : open) {
			toDelete.insert(s);
		}
		for (auto s : closed) {
			toDelete.insert(s);
		}
		for (auto s : open_queue) {
			toDelete.insert(s);
		}
		for (auto s : toDelete) {
			delete s;
		}
		open.clear();
		openByAddress.clear();
		closed.clear();
		open_queue.clear();
	};

	State *initial= new State(_initial);
	open_queue.push_back(initial);
	std::make_heap(open_queue.begin(), open_queue.end(),comp);


	open.insert(initial);
	openByAddress.insert(ptrKey(initial));

	nodes=0;


	while (!open_queue.empty()) {

		State *current= open_queue.front();
		std::pop_heap(open_queue.begin(), open_queue.end(),comp);
		open_queue.pop_back();

		// Lazy-delete stale heap entries that are no longer active in open.
		if (openByAddress.erase(ptrKey(current)) == 0) {
			delete current;
			continue;
		}
		auto currentOpen = open.find(current);
		if (currentOpen != open.end()) {
			open.erase(currentOpen);
		}


		if (current->isGoal()) {
			string candidateSolution = reconstruct(current);
			int solutionLength=current->getG();
			if (isLegalSolutionPath(_initial, candidateSolution, solutionLength)) {
				solution = candidateSolution;
				if (_name != "Test") {
					Problems.append("'").append(_name).append("',");
				}
				cleanup(current);
				return solutionLength;
			}
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
					cleanup(current);
					return -1;
				}
			}
			bool insertedInOpen=false;

			auto stored = closed.find(*it);
			if(stored!=closed.end()){
				if((*stored)->getG()<=(*it)->getG()){
					delete *it;
					continue;
				}
				auto toDel=*stored;//needed because iterator gets invalidated on next line
				closed.erase(stored); // re-open node to
				delete toDel;
			}

			stored = open.find(*it);
			if(stored!=open.end() ){
				if((*stored)->getG()<=(*it)->getG()){
					delete *it;
					continue;
				}
				// Replace open record instead of mutating it in-place to keep
				// heap entries and parent chains consistent.
				State *toReplace = *stored;
				openByAddress.erase(ptrKey(toReplace));
				open.erase(stored);
				open.insert(*it);
				openByAddress.insert(ptrKey(*it));
				open_queue.push_back(*it);
				std::push_heap(open_queue.begin(), open_queue.end(),comp);
				insertedInOpen=true;
			}
			if(!insertedInOpen){
				open.insert(*it); // fast, but nodes may appear multiple times
				openByAddress.insert(ptrKey(*it));
				open_queue.push_back(*it);
				std::push_heap(open_queue.begin(), open_queue.end(),comp);
			}
		}
		closed.insert(current);

	}
	cleanup(nullptr);
	solution="Can't find answer";
	return -1;
}

int Puzzle::beamSearch(int beamWidth) {
	start = chrono::high_resolution_clock::now();

	unordered_set<State*> closed;

	auto cleanup = [&](State *current, vector<State*>& beam, vector<State*>& next_beam) {
		unordered_set<State*> toDelete;
		if (current != nullptr) {
			toDelete.insert(current);
		}
		for (auto s : closed) {
			toDelete.insert(s);
		}
		for (auto s : beam) {
			toDelete.insert(s);
		}
		for (auto s : next_beam) {
			toDelete.insert(s);
		}
		for (auto s : toDelete) {
			delete s;
		}
		closed.clear();
		beam.clear();
		next_beam.clear();
	};

	State *initial = new State(_initial);
	vector<State*> beam;
	beam.push_back(initial);

	nodes = 0;

	while (!beam.empty()) {
		vector<State*> next_beam;

		for (State* current : beam) {
			if (current->isGoal()) {
				string candidateSolution = reconstruct(current);
				int solutionLength = current->getG();
				if (isLegalSolutionPath(_initial, candidateSolution, solutionLength)) {
					solution = candidateSolution;
					if (_name != "Test") {
						Problems.append("'").append(_name).append("',");
					}
					cleanup(current, beam, next_beam);
					return solutionLength;
				}
			}

			const vector<State*>& children = current->genSuccesors(false);

			for (auto it = children.begin(); it != children.end(); it++) {
				nodes++;
				if (nodes % 5000 == 0) {
					timeOut = chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeArg * 1000;
					if (timeOut) {
						threshold = current->getF();
						for (auto toDel = it; toDel != children.end(); toDel++) {
							delete (*toDel);
						}
						cleanup(current, beam, next_beam);
						return -1;
					}
				}

				if (closed.find(*it) != closed.end()) {
					delete *it;
					continue;
				}

				next_beam.push_back(*it);
			}
			closed.insert(current);
		}

		beam.clear(); // Current beam states are now in closed

		if (next_beam.empty()) break;

		// Sort next_beam to keep the most promising states
		ReverseStatePointerComparator comp;
		sort(next_beam.begin(), next_beam.end(), comp);

		// Remove duplicates that might have been generated in the same level
		unordered_set<uintptr_t> seen;
		for (auto it = next_beam.begin(); it != next_beam.end(); ) {
			if (seen.find((*it)->getHash()) != seen.end()) {
				delete *it;
				it = next_beam.erase(it);
			} else {
				seen.insert((*it)->getHash());
				it++;
			}
		}

		// Keep top beamWidth elements
		if ((int)next_beam.size() > beamWidth) {
			for (size_t i = beamWidth; i < next_beam.size(); i++) {
				delete next_beam[i];
			}
			next_beam.resize(beamWidth);
		}

		beam = next_beam;
	}

	vector<State*> dummy;
	cleanup(nullptr, beam, dummy);
	solution = "Can't find answer";
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

	

	uint32_t prevBestChild = 0;
	auto parentEntry = TT->TTLookup(s);
	if (parentEntry.getHash2() == s->getHash2()) {
		prevBestChild = parentEntry.getBestChild();
	}

	vector<State*> children=s->genSuccesors(false);//todo: avoid copy
    
	if (prevBestChild != 0 && children.size() > 1) {
		for (size_t i = 0; i < children.size(); i++) {
			if ((uint32_t)(children[i]->getHash2()) == prevBestChild) {
				std::swap(children[0], children[i]);
				break;
			}
		}
	}



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
	uint32_t bestChildFound = 0;

	for (auto it=children.begin(); it!=children.end();it++) {
		if(timeOut){
			for(auto toDel=it;toDel!=children.end();toDel++){
				delete (*toDel);
			}
			return -1;
		}

		uint32_t childHash32 = (uint32_t)((*it)->getHash2());
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
			new_bound = t; // better lower bound
			bestChildFound = childHash32;
		}
	}


	solution="Can't find answer";
	TT->TTsave(s, new_bound, bestChildFound);
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
	

	path=new State*[maxSolLength + 1];
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
	if (_name != "Test") {
		Problems.append("'").append(_name).append("',");
	}
	if(!clusterCache){
		delete TT;
	}
	delete[] path;
	return moves;
}

int Puzzle::rbfs(unsigned int ttSize, unsigned int maxSolLength) {
	if (!clusterCache||TT==nullptr){
		if(useMapTT) TT=new TranspositionTableMap(ttSize);
		else TT=new TranspositionTable(ttSize);
	}

	path = new State*[maxSolLength + 1];
	done = false;
	uint16_t out_f = 0;
	
	int moves = innerRbfs(&_initial, 0, SHRT_MAX, _initial.getH(), out_f);
	
	if (done && moves >= 0) {
		solution = reconstruct(moves);
		for(int i = 0; i <= moves; i++) {
			delete path[i];
		}
	} else {
		solution = "Can't find answer";
	}

	if (_name != "Test") {
		Problems.append("'").append(_name).append("',");
	}

	if(!clusterCache){
		delete TT;
	}
	delete[] path;
	return moves;
}

int Puzzle::innerRbfs(State* s, int depth, uint16_t f_limit, uint16_t node_f, uint16_t& out_f) {
	nodes++;
	if(nodes % 5000 == 0) {
		if(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeArg * 1000) {
			timeOut = true;
			out_f = node_f;
			return -1;
		}
	}

	if (s->isGoal()) {
		done = true;
		path[depth] = new State(*s);
		out_f = node_f;
		return 0;
	}

	uint32_t prevBestChild = 0;
	auto parentEntry = TT->TTLookup(s);
	if (parentEntry.getHash2() == s->getHash2()) {
		prevBestChild = parentEntry.getBestChild();
		uint16_t cached_f = (parentEntry.getBound() == SHRT_MAX) ? SHRT_MAX : parentEntry.getBound() + s->getG();
		if (cached_f > node_f) {
			node_f = cached_f;
		}
	}

	vector<State*> children = s->genSuccesors(false);

	if (children.empty()) {
		out_f = SHRT_MAX;
		TT->TTsave(s, SHRT_MAX, 0);
		return -1;
	}

	for (auto it = children.begin(); it != children.end(); it++) {
		auto entry = TT->TTLookup(*it);
		genCalls++;
		if (entry.getHash2() == (*it)->getHash2()) {
			auto newH = entry.getBound();
			if (newH > (*it)->getH()) {
				(*it)->setH(newH);
			}
		}
	}

	if (prevBestChild != 0 && children.size() > 1) {
		for (size_t i = 0; i < children.size(); i++) {
			if ((uint32_t)(children[i]->getHash2()) == prevBestChild) {
				std::swap(children[0], children[i]);
				break;
			}
		}
	}

	struct RbfsNode {
		State* state;
		uint16_t f;
	};
	vector<RbfsNode> child_nodes;
	for (auto it = children.begin(); it != children.end(); it++) {
		genCalls++;
		uint16_t child_f = max((uint16_t)((*it)->getG() + (*it)->getH()), node_f);
		child_nodes.push_back({*it, child_f});
	}

	while (true) {
		if (timeOut) {
			out_f = node_f;
			for (auto cn : child_nodes) delete cn.state;
			return -1;
		}

		sort(child_nodes.begin(), child_nodes.end(), [](const RbfsNode& a, const RbfsNode& b) {
			return a.f < b.f;
		});

		RbfsNode& best = child_nodes[0];
		if (best.f > f_limit) {
			out_f = best.f;
			uint16_t h_bound = (out_f == SHRT_MAX) ? SHRT_MAX : out_f - s->getG();
			TT->TTsave(s, h_bound, (uint32_t)(best.state->getHash2()));
			for (auto cn : child_nodes) delete cn.state;
			return -1;
		}

		uint16_t alternative_f = (child_nodes.size() > 1) ? child_nodes[1].f : SHRT_MAX;
		uint16_t next_limit = min(f_limit, alternative_f);
		uint16_t new_f = 0;

		totalH += best.state->getH();
		countH++;

		int res = innerRbfs(best.state, depth + 1, next_limit, best.f, new_f);
		best.f = new_f;

		if (done) {
			path[depth] = new State(*s);
			for (auto cn : child_nodes) delete cn.state;
			return res + 1;
		}
	}
}

void Puzzle::BFS(unsigned int ttSize)
{
	assert(clusterCache && connectedComponents);
	foundInPreviousTT = false;

	if(Lookup(&_initial)){
		return;
	}else
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

	uint32_t prevBestChild = 0;
	auto parentEntry = TT->TTLookup(s);
	if (parentEntry.getHash2() == s->getHash2()) {
		prevBestChild = parentEntry.getBestChild();
	}

	vector<State*> children=s->genSuccesors(false);//todo: avoid copy
    
	if (prevBestChild != 0 && children.size() > 1) {
		for (size_t i = 0; i < children.size(); i++) {
			if ((uint32_t)(children[i]->getHash2()) == prevBestChild) {
				std::swap(children[0], children[i]);
				break;
			}
		}
	}


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
		for(auto it = TTs.begin(); it != TTs.end(); it++)
		{
			if(*it == bfsTT) continue;
			if((*it)->lookup(s))
			{
				(*it)->merge(bfsTT);
				//delete bfsTT;
				bfsTT = *it;
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
