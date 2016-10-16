#ifndef _DIAGRAM_H_
#define _DIAGRAM_H_

#include <MemoryPool/C-11/MemoryPool.h>
//#include <MemoryPool/C-98/MemoryPool.h> //You will need to use this version instead of the one above if your compiler doesn't handle C++11's noexcept operator
#include "Edge.h"
#include "Cell.h"
#include <set>

namespace VORONOI {
	struct BoundingBox;
	class Diagram {
	public:
		std::vector<Cell*> cells;
		std::vector<Edge*> edges;
		std::vector<GEOM::Point2*> vertices;

		void printDiagram();
	private:
		friend class VoronoiDiagramGenerator;

		std::set<Cell*> tmpCells;
		std::set<Edge*> tmpEdges;
		std::set<GEOM::Point2*> tmpVertices;

		MemoryPool<Cell> cellPool;
		MemoryPool<Edge> edgePool;
		MemoryPool<HalfEdge> halfEdgePool;
		MemoryPool<GEOM::Point2> vertexPool;

		GEOM::Point2* createVertex(double x, double y);
		Cell* createCell(GEOM::Point2 site);
		Edge* createEdge(Site* lSite, Site* rSite, GEOM::Point2* vertA, GEOM::Point2* vertB);
		Edge* createBorderEdge(Site* lSite, GEOM::Point2* vertA, GEOM::Point2* vertB);

		bool connectEdge(Edge* edge, BoundingBox bbox);
		bool clipEdge(Edge* edge, BoundingBox bbox);
		void clipEdges(BoundingBox bbox);
		void closeCells(BoundingBox bbox);
		void finalize();
	};
}

#endif