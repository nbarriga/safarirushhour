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
	auto succ=_initial.genSuccesors();
	string s("P ");
	s.append(_name);
	s.append("\n");
	string state= _initial.toString();
	s.append(state);
	return s;
}
string Puzzle::reconstruct(const Node *node){
	if(node==NULL){
		return "";
	}else{
		return reconstruct(node->getPrev())+"\n"+node->toString();
	}
}
string Puzzle::aStar(){
	priority_queue<Node*,vector<Node*>,NodeComparator> open_queue;
	unordered_set<Node*> open;
	unordered_set<Node*> closed;

	Node *n=new Node(_initial,NULL);
	open_queue.push(n);


	  while (!open_queue.empty()) {

	    Node *current= open_queue.top();
	    open_queue.pop();
	   // cout<<"open queue size: "<<open_queue.size()<<" f:"<<current->getF()<<" first: "<<endl<<current->toString()<<endl;

	    if (current->isGoal()) {
	    	string s =reconstruct(current);
//	  	  for(auto it=closed.begin();it!=closed.end();it++){
//	  		  delete *it;
//	  	  }
	    	delete n;
		  for(auto it=open.begin();it!=open.end();it++){
			  delete (*it);
		  }

	      return s;
	    }
	    const vector<Node*>& children =current->genSuccesors();
//	    for (auto it=children.begin();it!=children.end();it++){
//	    	cout<<"child:"<<endl<<(*it)->toString()<<endl;
//	    }
	    for (auto it=children.begin();it!=children.end();it++){
	    	bool inOpen=false,inClosed=false;

	    	auto stored = open.find(*it);
	    	if(stored!=open.end() ){
	    		inOpen=true;
	    		if((*stored)->getG()<=(*it)->getG()){
	    			delete *it;
	    			continue;
	    		}
	    	}
	    	stored = closed.find(*it);
	    	if(stored!=closed.end()){
	    		inClosed=true;
	    		if((*stored)->getG()<=(*it)->getG()){
	    			delete *it;
	    			continue;
	    		}
	    	}

	    	if (inClosed) closed.erase(*it); // re-open node to
	    	if(inOpen)open.erase(*it);
	    	open.insert(*it); // fast, but nodes may appear multiple times
	    	open_queue.push(*it);//TODO: handle repeated nodes in open queue

	    	// other solution requires decreaseKey priority queue operation
	    	//   if (to,*) in OPEN then decrease key of state to to to.f in OPEN
	    	//                     else insert (to, to.f) into OPEN
	    }
	    closed.insert(current);
	  }
	  for(auto it=closed.begin();it!=closed.end();it++){
		  delete (*it);
	  }
	  closed.clear();
	  return "Can't find answer";
}
