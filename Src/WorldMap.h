#ifndef _WORLD_MAP_H_
#define _WORLD_MAP_H_

#include "Voronoi\Diagram.h"
#include "Platform.h"
#include "SimplexNoise\SimplexNoise.h"

class WorldMap {
	public:
		GLfloat* terrainVerts;
		unsigned int nTerrainVerts;
		GLuint* terrainVertIndices;
		unsigned int nTerrainTris;

		WorldMap() { diagram = nullptr; };
		~WorldMap();

		void generate(unsigned int seed);
	private:
		VORONOI::Diagram* diagram;
		MemoryPool<GEOM::Point2> noisyVerts;
		SimplexNoise simplex;

		uint32_t siteCount;
		uint32_t dimension;

		void pickLandWater();
		void genNoisyEdges();
		static void recursiveSubdivideRect(std::vector<GEOM::Point2>* points, GEOM::Point2& A, GEOM::Point2& B, GEOM::Point2& C, GEOM::Point2& D, double minLength);
};

#endif