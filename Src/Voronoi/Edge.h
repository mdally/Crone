#ifndef _EDGE_H_
#define _EDGE_H_

#include "Point2.h"

namespace VORONOI {
	struct Site;

	struct Edge {
		Site* lSite;
		Site* rSite;
		GEOM::Point2* vertA;
		GEOM::Point2* vertB;

		unsigned int nNoisyVerts;
		GEOM::Point2* noisyVerts;

		Edge() : lSite(nullptr), rSite(nullptr), vertA(nullptr), vertB(nullptr), nNoisyVerts(0), noisyVerts(nullptr) {};
		Edge(Site* _lSite, Site* _rSite) : lSite(_lSite), rSite(_rSite), vertA(nullptr), vertB(nullptr), nNoisyVerts(0), noisyVerts(nullptr) {};
		Edge(Site* lS, Site* rS, GEOM::Point2* vA, GEOM::Point2* vB) : lSite(lS), rSite(rS), vertA(vA), vertB(vB), nNoisyVerts(0), noisyVerts(nullptr) {};

		void setStartPoint(Site* _lSite, Site* _rSite, GEOM::Point2* vertex);
		void setEndPoint(Site* _lSite, Site* _rSite, GEOM::Point2* vertex);
	};

	struct HalfEdge {
		Site* site;
		Edge* edge;
		double angle;

		HalfEdge() : site(nullptr), edge(nullptr) {};
		HalfEdge(Edge* e, Site* lSite, Site* rSite);

		inline GEOM::Point2* startPoint();
		inline GEOM::Point2* endPoint();
	};

	inline GEOM::Point2* HalfEdge::startPoint() {
		return (edge->lSite == site) ? edge->vertA : edge->vertB;
	}

	inline GEOM::Point2 * HalfEdge::endPoint() {
		return (edge->lSite == site) ? edge->vertB : edge->vertA;
	}
}

#endif