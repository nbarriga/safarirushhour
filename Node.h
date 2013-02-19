/*
 * Node.h
 *
 *  Created on: Feb 19, 2013
 *      Author: nbarriga
 */

#ifndef NODE_H_
#define NODE_H_
#include "Common.h"
#include "State.h"

class Node {
	State _state;
	Node *_prev;
	int g;
public:
	int getF()const;
	int getG() const;
	int getH() const;
	size_t getHash() const;

	Node(State state, Node *prev);
	virtual ~Node();
};
class NodeComparator
{
public:
	NodeComparator(){}
	bool operator ()(const Node& lhs, const Node& rhs) const;
};
namespace std {
template <>struct hash<Node>
{

	size_t operator()(const Node & node) const{return node.getHash();}
};
}



#endif /* NODE_H_ */
