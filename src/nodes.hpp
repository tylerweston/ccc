#ifndef ECE467_NODE_HPP_INCLUDED
#define ECE467_NODE_HPP_INCLUDED

#include "location.hh"
#include <memory>

class Node {
public:
	yy::location location;

	virtual ~Node() = 0;
};

#endif // ECE467_NODE_HPP_INCLUDED
