#include "Puzzle.h"
#include <cmath>   // INSTRUMENTATION
#include <sstream>   // INSTRUMENTATION
#include <set>   // INSTRUMENTATION
#include <map>   // INSTRUMENTATION
#include <algorithm>
#include <cassert>

string Problems, times, memories, nodesSearched, initialH, actualSolution;
vector<BFSCache*> TTs;
BFSCache* bfsTT = nullptr;
int Puzzle::genCalls = 0;
//BFSCache * bfsTT=nullptr;	
//bool foundInPreviousTT=false;

Puzzle::Puzzle(string name, string state):_name(name), _initial(State(state)) {
	_stateString = state;   // INSTRUMENTATION


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
	metrics = SearchMetrics();                                   // INSTRUMENTATION
	auto metricsStart = chrono::high_resolution_clock::now();    // INSTRUMENTATION


	while (!open_queue.empty()) {

		State *current= open_queue.front();
		std::pop_heap(open_queue.begin(), open_queue.end(),comp);
		open_queue.pop_back();
		open.erase(current);


		if (current->isGoal()) {
			solution=reconstruct(current);;
			int solutionLength=current->getG();
			// ===== INSTRUMENTATION: store search metrics at the goal =====
			metrics.solved = true;
			metrics.solver_time = chrono::duration<double>(chrono::high_resolution_clock::now() - metricsStart).count();
			metrics.movements = solutionLength;
			metrics.open_list = (long)open.size();
			metrics.closed_list = (long)closed.size();
			if(metrics.real_min < 0){ metrics.real_min = metrics.real_max = 0; }
			if(metrics.eff_min < 0){ metrics.eff_min = metrics.eff_max = 0; }
			computeSolutionMetrics(current);
			computeStructuralMetrics();
			// =============================================================
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
		// ===== INSTRUMENTATION: successors of this expansion =====
		metrics.expanded++;
		int realThis = (int)children.size(), effThis = 0;
		metrics.real_total += realThis;
		// ========================================================

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
					metrics.repeated++;   // INSTRUMENTATION
					delete *it;
					continue;
				}
				auto toDel=*stored;//needed because iterator gets invalidated on next line
				closed.erase(*it); // re-open node to
				metrics.reopened++;       // INSTRUMENTATION
				delete toDel;
			}

			stored = open.find(*it);
			if(stored!=open.end() ){
				if((*stored)->getG()<=(*it)->getG()){
					metrics.repeated++;   // INSTRUMENTATION
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
			effThis++; metrics.eff_total++;   // INSTRUMENTATION: inserted or updated in open
		}
		// ===== INSTRUMENTATION: per-expansion min/max =====
		if(metrics.real_min < 0 || realThis < metrics.real_min) metrics.real_min = realThis;
		if(realThis > metrics.real_max) metrics.real_max = realThis;
		if(metrics.eff_min < 0 || effThis < metrics.eff_min) metrics.eff_min = effThis;
		if(effThis > metrics.eff_max) metrics.eff_max = effThis;
		// ==================================================
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

// ===================== INSTRUMENTATION =====================
// Metrics along the solution path: for every state of the path (initial state up to the state
// before the goal), the successors are generated with the same generator used by A*; a successor
// is "repeated" if it was already generated from an earlier state of the path, "effective" otherwise.
void Puzzle::computeSolutionMetrics(const State* goal){
	equal_to<State*> stateEqual; (void)stateEqual;
	vector<const State*> path;
	for(const State* t = goal->getPrev(); t != NULL; t = t->getPrev()) path.push_back(t);
	std::reverse(path.begin(), path.end());
	unordered_set<State*> seen;
	vector<State*> owned;
	for(const State* ps : path){
		vector<State*> ch = ps->genSuccesors(false);
		int realThis = (int)ch.size(), effThis = 0;
		for(State* c : ch){
			if(seen.find(c) != seen.end()){ metrics.sol_repeated++; delete c; continue; }
			seen.insert(c); owned.push_back(c); effThis++;
		}
		metrics.sol_real_total += realThis; metrics.sol_eff_total += effThis;
		if(metrics.sol_real_min < 0 || realThis < metrics.sol_real_min) metrics.sol_real_min = realThis;
		if(realThis > metrics.sol_real_max) metrics.sol_real_max = realThis;
		if(metrics.sol_eff_min < 0 || effThis < metrics.sol_eff_min) metrics.sol_eff_min = effThis;
		if(effThis > metrics.sol_eff_max) metrics.sol_eff_max = effThis;
	}
	for(State* o : owned) delete o;
	if(metrics.sol_real_min < 0){ metrics.sol_real_min = metrics.sol_real_max = 0; }
	if(metrics.sol_eff_min < 0){ metrics.sol_eff_min = metrics.sol_eff_max = 0; }
}

// Structural features of the initial board.
// Grid 7x7 given row-major in _stateString; '.' empty, 'x' rover (2x2); exit at the right edge
// of rows 3-4 (indices 2-3). Pieces: square (h==w), horizontal (h==1) or vertical (w==1).
void Puzzle::computeStructuralMetrics(){
	const int N = 7, ER0 = 2, ER1 = 3;
	struct P { char k; int h, w, r, c; };
	std::map<char, vector<pair<int,int>>> cells;
	for(int i = 0; i < N; i++) for(int j = 0; j < N; j++){
		char ch = _stateString[i * N + j];
		if(ch != '.') cells[ch].push_back({i, j});
	}
	std::map<char, P> pcs;
	for(auto& kv : cells){
		int r0 = N, c0 = N, r1 = -1, c1 = -1;
		for(auto& rc : kv.second){ r0 = std::min(r0, rc.first); c0 = std::min(c0, rc.second); r1 = std::max(r1, rc.first); c1 = std::max(c1, rc.second); }
		int h = r1 - r0 + 1, w = c1 - c0 + 1;
		pcs[kv.first] = P{ h == w ? 'S' : (h == 1 ? 'H' : 'V'), h, w, r0, c0 };
	}
	if(pcs.find('x') == pcs.end()) return;
	int rr = pcs['x'].r, rc = pcs['x'].c;
	auto pieceCells = [&](const P& p, int r, int c){ std::set<pair<int,int>> s; for(int a = 0; a < p.h; a++) for(int b = 0; b < p.w; b++) s.insert({r + a, c + b}); return s; };
	std::set<pair<int,int>> occupied, corridor;
	for(auto& kv : cells) for(auto& rcell : kv.second) occupied.insert(rcell);
	for(int i = ER0; i <= ER1; i++) for(int j = rc + 2; j < N; j++) corridor.insert({i, j});
	auto intersects = [](const std::set<pair<int,int>>& a, const std::set<pair<int,int>>& b){ for(auto& x : a) if(b.count(x)) return true; return false; };
	vector<char> blockers; int nRect = 0, nH = 0;
	for(auto& kv : pcs){
		if(kv.first == 'x') continue;
		const P& p = kv.second;
		metrics.F1_pieces++;
		if(p.k != 'S'){ nRect++; if(p.k == 'H') nH++; int len = std::max(p.h, p.w); if(len == 2) metrics.F2_cars++; else if(len == 3) metrics.F3_trucks++; }
		else metrics.F4_squares++;
		if(intersects(pieceCells(p, p.r, p.c), corridor)) blockers.push_back(kv.first);
	}
	metrics.F5_occupancy = (double)occupied.size() / (N * N);
	metrics.F6_rover_exit_distance = N - (rc + 2);
	metrics.F7_rover_misalignment = std::abs(rr - ER0);
	metrics.F8_direct_blockers = (int)blockers.size();
	for(char k : blockers){
		const P& p = pcs[k];
		std::set<pair<int,int>> occ = occupied; for(auto& x : pieceCells(p, p.r, p.c)) occ.erase(x);
		vector<pair<int,int>> dirs;
		if(p.k == 'H') dirs = {{0,1},{0,-1}}; else if(p.k == 'V') dirs = {{1,0},{-1,0}}; else dirs = {{0,1},{0,-1},{1,0},{-1,0}};
		bool canClear = false;
		for(auto d : dirs){
			int nr = p.r + d.first, nc = p.c + d.second;
			while(nr >= 0 && nc >= 0 && nr + p.h <= N && nc + p.w <= N && !intersects(pieceCells(p, nr, nc), occ)){
				if(!intersects(pieceCells(p, nr, nc), corridor)){ canClear = true; break; }
				nr += d.first; nc += d.second;
			}
			if(canClear) break;
		}
		if(!canClear) metrics.F9_blocked_blockers++;
	}
	metrics.F10_horizontal_ratio = nRect > 0 ? (double)nH / nRect : -1.0;
}

string Puzzle::metricsCsvHeader(){
	return "title;solver_time;movements;br_expanded_nodes;br_real_total_children;br_eff_total_children;"
	       "br_repeated;reopen;open_list;closed_list;br_real_avg;br_real_min;br_real_max;"
	       "br_eff_avg;br_eff_min;br_eff_max;br_classic;br_redundancy;"
	       "sol_br_real_total_children;sol_br_real_avg;sol_br_real_min;sol_br_real_max;"
	       "sol_br_eff_total_children;sol_br_eff_avg;sol_br_eff_min;sol_br_eff_max;"
	       "sol_br_repeated;sol_br_redundancy;"
	       "F1_pieces;F2_cars;F3_trucks;F4_squares;F5_occupancy;F6_rover_exit_distance;"
	       "F7_rover_misalignment;F8_direct_blockers;F9_blocked_blockers;F10_horizontal_ratio";
}

string Puzzle::metricsCsvRow() const{
	const SearchMetrics& M = metrics;
	auto ratio = [](double a, double b){ return b > 0 ? a / b : 0.0; };
	double classic = (M.movements > 0 && M.expanded > 0) ? pow((double)M.expanded, 1.0 / M.movements) : 0.0;
	stringstream o; o.precision(6);
	o << _name << ';' << M.solver_time << ';' << M.movements << ';' << M.expanded << ';'
	  << M.real_total << ';' << M.eff_total << ';' << M.repeated << ';' << M.reopened << ';'
	  << M.open_list << ';' << M.closed_list << ';'
	  << ratio(M.real_total, M.expanded) << ';' << M.real_min << ';' << M.real_max << ';'
	  << ratio(M.eff_total, M.expanded) << ';' << M.eff_min << ';' << M.eff_max << ';'
	  << classic << ';' << ratio(M.real_total, M.repeated) << ';'
	  << M.sol_real_total << ';' << ratio(M.sol_real_total, M.movements) << ';' << M.sol_real_min << ';' << M.sol_real_max << ';'
	  << M.sol_eff_total << ';' << ratio(M.sol_eff_total, M.movements) << ';' << M.sol_eff_min << ';' << M.sol_eff_max << ';'
	  << M.sol_repeated << ';' << ratio(M.sol_real_total, M.sol_repeated) << ';'
	  << M.F1_pieces << ';' << M.F2_cars << ';' << M.F3_trucks << ';' << M.F4_squares << ';' << M.F5_occupancy << ';'
	  << M.F6_rover_exit_distance << ';' << M.F7_rover_misalignment << ';' << M.F8_direct_blockers << ';'
	  << M.F9_blocked_blockers << ';';
	if(M.F10_horizontal_ratio >= 0) o << M.F10_horizontal_ratio;   // empty = undefined
	return o.str();
}

string Puzzle::metricsReport() const{
	string header = metricsCsvHeader(), row = metricsCsvRow();
	stringstream o; o << "===== Metrics =====\n";
	size_t hp = 0, rp = 0;
	while(hp != string::npos){
		size_t hn = header.find(';', hp), rn = row.find(';', rp);
		o << header.substr(hp, hn == string::npos ? string::npos : hn - hp) << ": "
		  << row.substr(rp, rn == string::npos ? string::npos : rn - rp) << "\n";
		hp = (hn == string::npos) ? hn : hn + 1;
		rp = (rn == string::npos) ? rn : rn + 1;
	}
	return o.str();
}
// ===========================================================================
