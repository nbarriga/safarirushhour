/*
 * Node.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#include "Node.h"

int Node::getF() const {
	return g+getH();
}

int Node::getG() const {
	return g;
}

int Node::getH() const {
	return _state.getH();
}
bool Node::isGoal(){
	return _state.isGoal();
}
Node::~Node() {
	// TODO Auto-generated destructor stub
}

Node::Node(State state, const Node *prev) :
		_state(state), _prev(prev), g(_prev==NULL?0:_prev->getG()+1) {
}
size_t Node::getHash() const {
	return _state.getHash();
}
string Node::toString()const{
	return _state.toString();
}
bool NodeComparator::operator ()(const Node* lhs, const Node* rhs) const {
	return lhs->getF() > rhs->getF();
}

vector<Node*> Node::genSuccesors() const{
	const vector<State>& children=_state.genSuccesors();
	vector<Node*> nodes;
	for(auto it=children.begin();it!=children.end();it++){
		nodes.push_back(new Node(*it,this));//TODO: this is slow!!!
	}
	return nodes;
}
