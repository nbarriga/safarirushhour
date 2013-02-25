/*
 * Puzzle.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Puzzle.h"

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
string Puzzle::aStar(){
	StatePointerComparator comp;
	ReverseStatePointerComparator revComp;
	equal_to<State*> stateEqual;
	vector<State*> open_queue;

	unordered_map<State*,int> open;
	unordered_map<State*,int> closed;
	State *initial= new State(_initial);
	open_queue.push_back(initial);
	std::make_heap(open_queue.begin(), open_queue.end(),comp);


	open.insert(make_pair(initial,initial->getG()));

	uint64_t nodes=0;

	while (!open_queue.empty()) {

		State *current= open_queue.front();
		std::pop_heap(open_queue.begin(), open_queue.end(),comp);
		open_queue.pop_back();
		open.erase(current);

		 //cout<<"open queue size: "<<open_queue.size()<<" f:"<<current->getF()<<" first: "<<endl<<current->toString()<<endl;

		if (current->isGoal()) {
			stringstream ss;
			ss<<current->getG()<<" "<<reconstruct(current);

			cout<<"nodes searched: "<<nodes<<" closed size: "<<
					closed.size()<<" open size: "<<open.size()<<" "<<open_queue.size()<<endl;

			for(auto it=open.begin();it!=open.end();it++){
				delete it->first;
			}
			for(auto it=closed.begin();it!=closed.end();it++){
				delete it->first;
			}
			delete current;
			open.clear();
			closed.clear();
			return ss.str();
		}
		const vector<State*>& children =current->genSuccesors();
		bool deletedFromOpen=false;
		for (auto it=children.begin();it!=children.end();it++){
			nodes++;

			auto stored = closed.find(*it);
			if(stored!=closed.end()){
				if(stored->second<=(*it)->getG()){
					delete *it;
					continue;
				}
				closed.erase(*it); // re-open node to
				delete stored->first;
			}

			stored = open.find(*it);
			if(stored!=open.end() ){
				if(stored->second<=(*it)->getG()){
					delete *it;
					continue;
				}

				//check in queue and delete
				for(auto toDel=open_queue.begin();toDel!=open_queue.end();toDel++){
					if(stateEqual(*toDel,*it)){
						deletedFromOpen=true;
						open_queue.erase(toDel);
						break;
					}
				}
				delete stored->first;
				open.erase(stored);

			}
			open.insert(make_pair(*it,(*it)->getG())); // fast, but nodes may appear multiple times
			open_queue.push_back(*it);

			//			uint64_t memOpen=open.size()*sizeof(State);
//			uint64_t memClosed=closed.size()*sizeof(State);
//			uint64_t currMem=memOpen+memClosed;
//			memory=memory>currMem?memory:currMem;
		}
		if(deletedFromOpen){
			std::make_heap(open_queue.begin(), open_queue.end(),comp);
		}else{
			std::push_heap(open_queue.begin(), open_queue.end(),comp);
		}
		//cout<<open_queue.size()<<" "<<open.size()<<" "<<closed.size()<<endl;
		closed.insert(make_pair(current,current->getG()));

		//============================SMA*====================================
//		if(open_queue.size()>50000){
//			sort_heap(open_queue.begin(), open_queue.end(),comp);
//			for(auto toDel=open_queue.begin();toDel!=(open_queue.begin()+10000);toDel++){
//				open.erase(*toDel);
//				delete *toDel;
//			}
//			open_queue.erase(open_queue.begin(),open_queue.begin()+10000);
//			make_heap(open_queue.begin(), open_queue.end(),comp);
//		}
	}
	//	for(auto it=closed.begin();it!=closed.end();it++){
	//		delete (*it);
	//	}
	closed.clear();
	return "Can't find answer";
}
