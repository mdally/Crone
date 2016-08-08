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

		~WorldMap();

		void generate(unsigned int seed);
	private:
		Diagram* diagram;
};

#endif