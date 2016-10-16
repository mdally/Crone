#ifndef _VORONOI_DIAGRAM_GENERATOR_H_
#define _VORONOI_DIAGRAM_GENERATOR_H_

#include "RBTree.h"
#include "CircleEventQueue.h"
#include "BeachLine.h"
#include "Diagram.h"
#include <vector>

namespace VORONOI {
	struct BoundingBox {
		double xL;
		double xR;
		double yB;
		double yT;

		BoundingBox() {};
		BoundingBox(double xmin, double xmax, double ymin, double ymax) :
			xL(xmin), xR(xmax), yB(ymin), yT(ymax) {};
	};

	class VoronoiDiagramGenerator {
	public:
		VoronoiDiagramGenerator() : circleEventQueue(nullptr), siteEventQueue(nullptr), beachLine(nullptr) {};
		~VoronoiDiagramGenerator() {};

		Diagram* compute(std::vector<GEOM::Point2>& sites, BoundingBox bbox);
		Diagram* relax();
	private:
		Diagram* diagram;
		CircleEventQueue* circleEventQueue;
		std::vector<GEOM::Point2*>* siteEventQueue;
		BoundingBox	boundingBox;

		void printBeachLine();

		//BeachLine
		RBTREE::RBTree<BeachSection>* beachLine;
		RBTREE::treeNode<BeachSection>* addBeachSection(Site* site);
		inline void detachBeachSection(RBTREE::treeNode<BeachSection>* section);
		void removeBeachSection(RBTREE::treeNode<BeachSection>* section);
		double leftBreakpoint(RBTREE::treeNode<BeachSection>* section, double directrix);
		double rightBreakpoint(RBTREE::treeNode<BeachSection>* section, double directrix);
	};

	inline void VoronoiDiagramGenerator::detachBeachSection(RBTREE::treeNode<BeachSection>* section) {
		circleEventQueue->removeCircleEvent(section);
		beachLine->removeNode(section);
	}
}

#endif