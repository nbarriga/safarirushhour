/*
 * Puzzle.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Puzzle.h"
string Problems, times, memories, nodesSearched, initialH, actualSolution;
Puzzle::Puzzle(string name, string state):_name(name), _initial(State(state)) {


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
int Puzzle::aStar(int memoryLimit){

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
			// stringstream ss;
			// ss<<(int)maxLiveNodes*memFactor+baseMemory;
			// memories.append(ss.str()).append(" ");
			// stringstream ss2;
			// ss2<<nodes;
			// nodesSearched.append(ss2.str()).append(" ");
			// stringstream ss3;
			// ss3<<solutionLength;
			// actualSolution.append(ss3.str()).append(" ");

//			struct rusage usage;
//			getrusage(RUSAGE_SELF , &usage);
//
//			if(init_done){
//				cout<<"Usage: "<<usage.ru_maxrss/1024<<"MB"<<endl;
//				cout<<"memory: "<<(int)maxLiveNodes*memFactor<<"MB"<<endl;
//				cout<<"nodes searched: "<<nodes<<endl;
//
//			}
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
					//solution=reconstruct(current);;
					int solutionLength=-1;
					Problems.append("'").append(_name).append("',");
					// stringstream ss;
					// ss<<(int)maxLiveNodes*memFactor;
					// memories.append(ss.str()).append(" ");
					// stringstream ss2;
					// ss2<<nodes;
					// nodesSearched.append(ss2.str()).append(" ");
					// stringstream ss3;
					// ss3<<solutionLength;
					// actualSolution.append(ss3.str()).append(" ");
				}
				overMemBudget=maxLiveNodes*memFactor+baseMemory>memoryLimit;
				if(overMemBudget&&algorithmArg=="Astar"){
					//solution=reconstruct(current);;
					int solutionLength=-1;
					Problems.append("'").append(_name).append("',");
					// stringstream ss;
					// ss<<(int)maxLiveNodes*memFactor+baseMemory;
					// memories.append(ss.str()).append(" ");
					// stringstream ss2;
					// ss2<<nodes;
					// nodesSearched.append(ss2.str()).append(" ");
					// stringstream ss3;
					// ss3<<solutionLength;
					// actualSolution.append(ss3.str()).append(" ");
				}
				if(timeOut||overMemBudget){
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
//				cout<<"closed";
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
						if((*stored)!=(*toUpdate)){
							cout<<"jdjdjdjdjd";
						}
						(*toUpdate)->setG((*it)->getG());
						(*toUpdate)->setParent((*it)->getPrev());
						std::push_heap(open_queue.begin(), toUpdate+1,comp);

                        //dont touch the open set.it is already updated
//						open.erase(stored);
//						open.insert(*toUpdate);
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
			// maxLiveNodes=maxLiveNodes>closed.size()+open.size()?maxLiveNodes:closed.size()+open.size();

		}
		//cout<<closed.size()<<" "<<open.size()<<endl;
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
		return 0;
	}


	vector<State*> children=s->genSuccesors(true);//todo: avoid copy


	for (auto it=children.begin(); it!=children.end();it++) {
		if (TT->exist(*it)){
			auto newH=TT->TTGetBound(*it);
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
	sort(children.begin(), children.end(),ReverseStatePointerComparator());
	//cout<<(*children.begin())->getF()<<" "<<(*(children.end()-1))->getF()<<endl;

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

void Puzzle::init(int baseMemory) {
	start=chrono::high_resolution_clock::now();
	timeOut=false;
	overMemBudget=false;
	nodes=0;
	this->baseMemory=baseMemory;
	maxLiveNodes=0;
	threshold=0;
}

int Puzzle::idaStar(unsigned int ttSize, unsigned int maxSolLength) {
	TT=new TranspositionTable(ttSize);
	path=new State*[maxSolLength];
//	if(threshold==0){
		threshold = _initial.getH();
//	}
//	cout<<"threshold: "<<threshold<<" H: "<<_initial.getH()<<endl;
	done = false;
	int moves;

	while (!done) {
		//f_min = SHRT_MAX;
//		cout<<"Threshold: "<<threshold<<", nodes: "<<nodes<<endl;
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
	// if(algorithmArg=="fallback"){
	// 	stringstream ss;
	// 			ss<<maxLiveNodes*memFactor+baseMemory;
	// 	memories.append(ss.str()).append(" ");
	// }else{
	// 	struct rusage usage;
	// 	getrusage(RUSAGE_SELF , &usage);
	// 	stringstream ss;
	// 	ss<<usage.ru_maxrss/1024;

	// 	memories.append(ss.str()).append(" ");
	// }
	// stringstream ss2;
	// ss2<<nodes;
	// nodesSearched.append(ss2.str()).append(" ");
	// stringstream ss3;
	// ss3<<moves;
	// actualSolution.append(ss3.str()).append(" ");
	//cout<<"total nodes searched: "<<nodes<<endl;
	delete TT;
	delete path;
	return moves;
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
