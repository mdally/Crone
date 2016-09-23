#ifndef _WORLD_MAP_H_
#define _WORLD_MAP_H_

#include "Voronoi\Diagram.h"
#include "Platform.h"

class WorldMap {
	public:
		GLfloat* terrainVerts;
		unsigned int nTerrainVerts;
		GLuint* terrainVertIndices;
		unsigned int nTerrainVertIndices;

		WorldMap() { diagram = nullptr; };
		~WorldMap();

		void generate(unsigned int seed);
	private:
		Diagram* diagram;
		MemoryPool<Point2> noisyVerts;

		void genNoisyEdges();
		static void recursiveSubdivideRect(std::vector<Point2>* points, Point2& A, Point2& B, Point2& C, Point2& D, double minLength);
};

#endif