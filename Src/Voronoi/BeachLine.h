#ifndef _BEACHLINE_H_
#define _BEACHLINE_H_

#include "RBTree.h"

namespace VORONOI {
	struct Site;
	struct Edge;
	struct CircleEvent;
	struct BeachSection {
		Site* site;
		Edge* edge;
		RBTREE::treeNode<CircleEvent>* circleEvent;

		BeachSection() : site(nullptr), edge(nullptr), circleEvent(nullptr) {};
		~BeachSection() {};
		BeachSection(Site* _site) : site(_site), edge(nullptr), circleEvent(nullptr) {};
	};
}

#endif