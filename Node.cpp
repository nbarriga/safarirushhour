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

Node::~Node() {
	// TODO Auto-generated destructor stub
}

Node::Node(State state, Node *prev) :
		_state(state), _prev(prev), g(_prev->getG()+1) {
}
size_t Node::getHash() const {
	return _state.getHash();
}

bool NodeComparator::operator ()(const Node& lhs, const Node& rhs) const {
	return lhs.getF() > rhs.getF();
}

