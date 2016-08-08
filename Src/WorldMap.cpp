#include "WorldMap.h"
#include "Voronoi\VoronoiDiagramGenerator.h"
#include <ctime>

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void genRandomSites(std::vector<Point2>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites, unsigned int seed) {
	bbox = BoundingBox(0, dimension, dimension, 0);
	std::vector<Point2> tmpSites;

	tmpSites.reserve(numSites);
	sites.reserve(numSites);

	Point2 s;

	srand(seed);
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		tmpSites.push_back(s);
	}

	//remove any duplicates that exist
	std::sort(tmpSites.begin(), tmpSites.end(), sitesOrdered);
	sites.push_back(tmpSites[0]);
	for (Point2& s : tmpSites) {
		if (s != sites.back()) sites.push_back(s);
	}
}

WorldMap::~WorldMap() {
	if (diagram) delete diagram;
}

void WorldMap::generate(unsigned int seed) {
	std::vector<Point2> sites;
	BoundingBox bbox;
	unsigned int dimension = 10000;
	unsigned int siteCount = 2000;
	genRandomSites(sites, bbox, dimension, siteCount, seed);

	VoronoiDiagramGenerator vdg;
	diagram = vdg.compute(sites, bbox);
	for (int i = 0; i < 5; ++i) {
		Diagram* old = diagram;
		diagram = vdg.relax();
		delete old;
	}

	srand(0);
	std::vector<GLfloat> tmpTerrainVerts;
	std::vector<GLuint> tmpTerrainVertIndices;

	GLuint vertIdx = 0;
	for (Cell* c : diagram->cells) {
		float r = rand() / (float)RAND_MAX;
		float g = rand() / (float)RAND_MAX;
		float b = rand() / (float)RAND_MAX;

		Point2& site = c->site.p;
		tmpTerrainVerts.push_back((float)site.x);
		tmpTerrainVerts.push_back(0.0);
		tmpTerrainVerts.push_back(-((float)dimension-(float)site.y));
		tmpTerrainVerts.push_back(r); 
		tmpTerrainVerts.push_back(g); 
		tmpTerrainVerts.push_back(b);

		GLuint cellCenterIdx = vertIdx++;

		for (HalfEdge* he : c->halfEdges) {
			Point2& vert = *(he->startPoint());

			tmpTerrainVerts.push_back((float)vert.x);
			tmpTerrainVerts.push_back(0.0);
			tmpTerrainVerts.push_back(-((float)dimension-(float)vert.y));
			tmpTerrainVerts.push_back(r); 
			tmpTerrainVerts.push_back(g); 
			tmpTerrainVerts.push_back(b);

			++vertIdx;
		}

		GLuint nEdges = c->halfEdges.size();
		for (GLuint i = 1; i <= nEdges; ++i) {
			tmpTerrainVertIndices.push_back(cellCenterIdx);
			tmpTerrainVertIndices.push_back(cellCenterIdx + i);
			tmpTerrainVertIndices.push_back(cellCenterIdx + (i % nEdges) + 1);
		}
	}

	nTerrainVerts = tmpTerrainVerts.size();
	terrainVerts = (GLfloat*)malloc(sizeof(GLfloat)*nTerrainVerts);
	unsigned int idx = 0;
	for (GLfloat f : tmpTerrainVerts) {
		terrainVerts[idx] = tmpTerrainVerts[idx++];
	}

	nTerrainVertIndices = tmpTerrainVertIndices.size();
	terrainVertIndices = (GLuint*)malloc(sizeof(GLuint)*nTerrainVertIndices);
	idx = 0;
	for (GLuint f : tmpTerrainVertIndices) {
		terrainVertIndices[idx] = tmpTerrainVertIndices[idx++];
	}
}
