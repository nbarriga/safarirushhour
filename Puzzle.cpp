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
	uint32_t maxLiveNodes=0;

	while (!open_queue.empty()) {

		State *current= open_queue.front();
		std::pop_heap(open_queue.begin(), open_queue.end(),comp);
		open_queue.pop_back();
		open.erase(current);

		//=======================IDA*===========================
		//		if(current->getF()>16){
		//			continue;
		//		}
		//cout<<"open queue size: "<<open_queue.size()<<" f:"<<current->getF()<<" first: "<<endl<<current->toString()<<endl;

		if (current->isGoal()) {
			stringstream ss;
			ss<<current->getG()<<reconstruct(current);

			cout<<"memory: "<<maxLiveNodes*(sizeof(State)+sizeof(RowCol)*numPieces)<<endl;
			cout<<"nodes searched: "<<nodes<<" closed size: "<<
					closed.size()<<" open size: "<<open.size()<<" "<<open_queue.size()<<endl;

			for(auto it=open.begin();it!=open.end();it++){
				delete *it;
			}
			for(auto it=closed.begin();it!=closed.end();it++){
				delete *it;
			}
			delete current;
			open.clear();
			closed.clear();
			return ss.str();
		}
		const vector<State*>& children =current->genSuccesors(false);

		for (auto it=children.begin();it!=children.end();it++){
			nodes++;
			bool updatedInOpen=false;

			auto stored = closed.find(*it);
			if(stored!=closed.end()){
				if((*stored)->getG()<=(*it)->getG()){
					delete *it;
					continue;
				}
				closed.erase(*it); // re-open node to
				delete *stored;
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

						(*toUpdate)->setG((*it)->getG());
						std::push_heap(open_queue.begin(), toUpdate+1,comp);

						open.erase(stored);
						open.insert(*toUpdate);
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
			maxLiveNodes=maxLiveNodes>closed.size()+open.size()?maxLiveNodes:closed.size()+open.size();

		}

		closed.insert(current);

		//============================SMA*====================================
		//this needs to make sure that for any deleted node, its parent is in the open list.
		//So, before deleting a node, if parent is not in open, reopen it from closed.
//		const int sizeLimit=1000;//,forgottenSize=20000;
//		if(open_queue.size()>sizeLimit){
//			//sort_heap(open_queue.begin(), open_queue.end(),comp);
//			//we want to delete the highest F's, breaking ties by lowest G
//			sort(open_queue.begin(), open_queue.end(),HighFLowGComparator());
//			unordered_set<State*> reopen;
//			vector<State*> erased;
//
//			while(erased.size()<100){
//				auto part=partition(
//						open_queue.begin(),
//						open_queue.end(),
//						FGComparator(open_queue.front()->getF(),open_queue.front()->getG()));
//
//
//				//unordered_set<State*> erased_set;
//				//erased.insert(erased.begin(),open_queue.begin(),open_queue.begin()+forgottenSize);
//				erased.insert(erased.end(),open_queue.begin(),part);
//				//erased_set.insert(open_queue.begin(),open_queue.begin()+forgottenSize);
//
//				//open_queue.erase(open_queue.begin(),open_queue.begin()+forgottenSize);
//				open_queue.erase(open_queue.begin(),part);
//
//
//
//				cout<<"deleted: "<<erased.size()<<endl;
//				cout<<erased.front()->getF()<<" "<<erased.front()->getG()<<" "<<
//						erased.back()->getF()<<" "<<erased.back()->getG()<<" "<<
//						open_queue.front()->getF()<<" "<<open_queue.front()->getG()<<" "<<
//						open_queue.back()->getF()<<" "<<open_queue.back()->getG()<<endl;
//			}
//			cout<<"DONE ERASING"<<endl<<endl;
//			for(auto toDel=erased.begin();toDel!=erased.end();toDel++){
//				open.erase(*toDel);
//
//			}
//			for(auto toDel=erased.begin();toDel!=erased.end();toDel++){
//				State *parent=const_cast<State*>((*toDel)->getPrev());
//				if(open.count(parent)==0 && reopen.count(parent)==0){
//					//reopen parent
//					if(closed.count(parent)==0){
//						cout<<"error!!!!! parent should be in one of the lists"<<endl;
//					}else{
//						reopen.insert(parent);
//						closed.erase(parent);
//					}
//				}
//				delete *toDel;
//			}
//			cout<<"reopening nodes: "<<reopen.size()<<endl;
//
//			open_queue.insert(open_queue.end(),reopen.begin(),reopen.end());
//			open.insert(reopen.begin(),reopen.end());
//			make_heap(open_queue.begin(), open_queue.end(),comp);
//		}
	}
	for(auto it=closed.begin();it!=closed.end();it++){
		delete (*it);
	}
	closed.clear();
	return "Can't find answer";
}

string Puzzle::innerIdaStar(State* s, int depth) {
	nodes++;
	path[depth]=s;
	auto f=s->getF();

	//cout<<depth<<" "<<visited.size()<<endl;
#ifdef ZOBRIST
	if(TT.exist(s)){
		if(TT.TTGetG(s)<s->getG()){
			return "";//TT cut off
		}else if(TT.TTGetG(s)>s->getG()){
			TT.TTsave(s);//replace
		}
	}else{
		TT.TTsave(s);
	}
#else
	auto old=visited.find(*s);
	if(old!=visited.end()){
		if(old->getG()<s->getG()){
			return "";
		}else if(old->getG()>s->getG()){
			visited.erase(old);
			//delete *old;
			visited.insert(*s);
		}
	}

	if(old==visited.end()){
		visited.insert(*s);
	}
#endif

	if (f > threshold) {
		if (f < f_min) {
			f_min = f;
		}
		done=false;
		return "";
	}

	if (s->getH() == 0){
		done=true;
		stringstream ss;
		ss<<s->getG()<<reconstruct(depth);
		return ss.str();  // goal found
	}


	const vector<State*>& children=s->genSuccesors(true);

	for (auto it=children.begin(); it!=children.end();it++) {
		string s=innerIdaStar(*it, depth+1);
		delete(*it);
		if (done){
			for(auto toDel=it+1;toDel!=children.end();toDel++){
				delete (*toDel);
			}
			return s;         // done
		}
	}


	return "Can't find answer";
}

string Puzzle::reconstruct(int endPos) {
	stringstream ss;
	for(int i=0;i<endPos;i++){
		ss<<" "<<path[i]->getMove(path[i+1]);
	}
	return ss.str();
}

string Puzzle::idaStar() {
	nodes=0;
	threshold = _initial.getH();
	done = false;
	string moves;

	while (!done) {
		f_min = SHRT_MAX;

		moves = innerIdaStar(&_initial,0);

		if (!done){
			threshold = f_min;
			//cout<<"increasing threshold to: "<<threshold<<endl;
		}
	}
	cout<<"nodes searched: "<<nodes<<endl;
	return moves;
}
