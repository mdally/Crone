#ifndef _CIRCLE_EVENT_QUEUE_H_
#define _CIRCLE_EVENT_QUEUE_H_

#include "RBTree.h"
#include "BeachLine.h"

namespace VORONOI {
	struct Site;
	struct BeachSection;

	struct CircleEvent {
		Site* site;
		double x;
		double y;
		double yCenter;
		RBTREE::treeNode<BeachSection>* beachSection;

		CircleEvent() {};
		~CircleEvent() {};
		CircleEvent(Site* _site, double _x, double _y, double _yCenter, RBTREE::treeNode<BeachSection>* _section) {
			site = _site;
			x = _x;
			y = _y;
			yCenter = _yCenter;
			beachSection = _section;
		}
	};

	struct CircleEventQueue {
		RBTREE::treeNode<CircleEvent>* firstEvent;
		RBTREE::RBTree<CircleEvent> eventQueue;

		CircleEventQueue() : firstEvent(nullptr) {};
		~CircleEventQueue() {};

		void addCircleEvent(RBTREE::treeNode<BeachSection>* section);
		void removeCircleEvent(RBTREE::treeNode<BeachSection>* section);
	};
}

#endif