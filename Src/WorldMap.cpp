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
	unsigned int siteCount = 30000;
	genRandomSites(sites, bbox, dimension, siteCount, seed);

	VoronoiDiagramGenerator vdg;
	diagram = vdg.compute(sites, bbox);
	for (int i = 0; i < 5; ++i) {
		Diagram* old = diagram;
		diagram = vdg.relax();
		delete old;
	}

#define useNoisyEdges 1
#if useNoisyEdges
	genNoisyEdges();

	nTerrainVerts = 0;
	nTerrainVertIndices = 0;

	for (Cell* c : diagram->cells) {
		for (HalfEdge* he : c->halfEdges) {
			size_t nVerts = (he->edge->noisyVerts ? he->edge->nNoisyVerts-1 : 1);
			nTerrainVerts += nVerts;
			nTerrainVertIndices += nVerts;
		}
		++nTerrainVerts;
	}
	terrainVerts = (GLfloat*)malloc(sizeof(GLfloat) * nTerrainVerts * 6);
	terrainVertIndices = (GLuint*)malloc(sizeof(GLuint) * nTerrainVertIndices * 3);

	srand(0);
	GLuint vertCount = 0;
	GLuint vertIdxCount = 0;
	for (Cell* c : diagram->cells) {
		float r = rand() / (float)RAND_MAX;
		float g = rand() / (float)RAND_MAX;
		float b = rand() / (float)RAND_MAX;
		bool border = false;
		for (HalfEdge* he : c->halfEdges) {
			if (!(he->edge->lSite && he->edge->rSite)) {
				border = true;
				break;
			}
		}
		if (border) {
			r = 0.0f;
			g = 0.0f;
			b = 1.0f;
		}

		Point2& site = c->site.p;

		terrainVerts[vertCount * 6 + 0] = (float)site.x;
		terrainVerts[vertCount * 6 + 1] = (float)0.0;
		terrainVerts[vertCount * 6 + 2] = -((float)dimension - (float)site.y);
		terrainVerts[vertCount * 6 + 3] = r;
		terrainVerts[vertCount * 6 + 4] = g;
		terrainVerts[vertCount * 6 + 5] = b;

		size_t cellCenterIdx = vertCount++;
		size_t cellVerts = 0;

		for (HalfEdge* he : c->halfEdges) {
			Point2* noisyVerts = he->edge->noisyVerts;
			if (noisyVerts) {
				bool reverse = *(he->startPoint()) != noisyVerts[0];

				size_t nNoisyVerts = he->edge->nNoisyVerts-1;
				for (size_t i = 0; i < nNoisyVerts; ++i) {
					size_t idx = (reverse ? nNoisyVerts - i : i);
					Point2& site = noisyVerts[idx];

					terrainVerts[vertCount * 6 + 0] = (float)site.x;
					terrainVerts[vertCount * 6 + 1] = (float)0.0;
					terrainVerts[vertCount * 6 + 2] = -((float)dimension - (float)site.y);
					terrainVerts[vertCount * 6 + 3] = r;
					terrainVerts[vertCount * 6 + 4] = g;
					terrainVerts[vertCount * 6 + 5] = b;

					++vertCount;
					++cellVerts;
				}
			}
			else {
				Point2& site = *(he->startPoint());

				terrainVerts[vertCount * 6 + 0] = (float)site.x;
				terrainVerts[vertCount * 6 + 1] = (float)0.0;
				terrainVerts[vertCount * 6 + 2] = -((float)dimension - (float)site.y);
				terrainVerts[vertCount * 6 + 3] = r;
				terrainVerts[vertCount * 6 + 4] = g;
				terrainVerts[vertCount * 6 + 5] = b;

				++vertCount;
				++cellVerts;
			}
		}

		for (GLuint i = 1; i <= cellVerts; ++i) {
			terrainVertIndices[vertIdxCount++] = cellCenterIdx;
			terrainVertIndices[vertIdxCount++] = cellCenterIdx + i;
			terrainVertIndices[vertIdxCount++] = cellCenterIdx + (i % cellVerts) + 1;
		}
	}


#else
	nTerrainVerts = 0;
	nTerrainVertIndices = 0;
	for (Cell* c : diagram->cells) {
		nTerrainVerts += c->halfEdges.size() + 1;
		nTerrainVertIndices += c->halfEdges.size();
	}
	terrainVerts = (GLfloat*)malloc(sizeof(GLfloat) * nTerrainVerts * 6);
	terrainVertIndices = (GLuint*)malloc(sizeof(GLuint) * nTerrainVertIndices * 3);

	srand(0);
	GLuint vertCount = 0;
	GLuint vertIdxCount = 0;
	for (Cell* c : diagram->cells) {
		float r = rand() / (float)RAND_MAX;
		float g = rand() / (float)RAND_MAX;
		float b = rand() / (float)RAND_MAX;

		Point2& site = c->site.p;

		terrainVerts[vertCount * 6 + 0] = (float)site.x;
		terrainVerts[vertCount * 6 + 1] = 0.0;
		terrainVerts[vertCount * 6 + 2] = -((float)dimension - (float)site.y);
		terrainVerts[vertCount * 6 + 3] = r;
		terrainVerts[vertCount * 6 + 4] = g;
		terrainVerts[vertCount * 6 + 5] = b;

		GLuint cellCenterIdx = vertCount++;

		for (HalfEdge* he : c->halfEdges) {
			Point2& vert = *(he->startPoint());

			terrainVerts[vertCount * 6 + 0] = (float)vert.x;
			terrainVerts[vertCount * 6 + 1] = 0.0;
			terrainVerts[vertCount * 6 + 2] = -((float)dimension - (float)vert.y);
			terrainVerts[vertCount * 6 + 3] = r;
			terrainVerts[vertCount * 6 + 4] = g;
			terrainVerts[vertCount * 6 + 5] = b;

			++vertCount;
		}

		GLuint nEdges = c->halfEdges.size();
		for (GLuint i = 1; i <= nEdges; ++i) {
			terrainVertIndices[vertIdxCount++] = cellCenterIdx;
			terrainVertIndices[vertIdxCount++] = cellCenterIdx + i;
			terrainVertIndices[vertIdxCount++] = cellCenterIdx + (i % nEdges) + 1;
		}
	}
#endif

	delete diagram;
	diagram = nullptr;
}

void WorldMap::genNoisyEdges() {
	if (!diagram) return;
	srand(0);

	for (Edge* e : diagram->edges) {
		if (e->lSite && e->rSite) {
			std::vector<Point2> path1;
			std::vector<Point2> path2;

			Point2& v0 = *e->vertA;
			Point2& v1 = *e->vertB;
			Point2& d0 = e->lSite->p;
			Point2& d1 = e->rSite->p;

			double tradeoff = 0.5;
			Point2 t = lerp(v0, d0, tradeoff);
			Point2 q = lerp(v0, d1, tradeoff);
			Point2 r = lerp(v1, d0, tradeoff);
			Point2 s = lerp(v1, d1, tradeoff);
			Point2 mid = lerp(v0, v1, 0.5);

			//TODO : change this to vary based on type of border
			// i.e. river, coastline, ocean-ocean, etc
			double minLength = 1;

			//first half of path
			path1.push_back(v0);
			recursiveSubdivideRect(&path1, v0, t, mid, q, minLength);
			path1.push_back(mid);

			//second half, done in backwards order
			path2.push_back(v1);
			recursiveSubdivideRect(&path2, v1, s, mid, r, minLength);

			//concatenate the two halves
			while (!path2.empty()) {
				path1.push_back(path2.back());
				path2.pop_back();
			}

			//copy verts to memory pool
			size_t nVerts = path1.size();
			Point2* vertArray = noisyVerts.allocContiguousArray(nVerts);
			size_t idx = 0;
			for (Point2& p : path1) {
				vertArray[idx++] = p;
			}
			e->nNoisyVerts = nVerts;
			e->noisyVerts = vertArray;
		}
	}
}

void WorldMap::recursiveSubdivideRect(std::vector<Point2>* points, Point2& A, Point2& B, Point2& C, Point2& D, double minLength) {
	if ((A-C).length() < minLength || (B-D).length() < minLength) {
		return;
	}

	// Subdivide the quadrilateral
	double p = (rand() / (double)RAND_MAX)*0.6 + 0.2; // vertical (along A-D and B-C)  [0.2, 0.8]
	double q = (rand() / (double)RAND_MAX)*0.6 + 0.2; // horizontal (along A-B and D-C) [0.2, 0.8]

	// Midpoints
	Point2 E = lerp(A, D, p);
	Point2 F = lerp(B, C, p);
	Point2 G = lerp(A, B, q);
	Point2 I = lerp(D, C, q);

	// Central point
	Point2 H = lerp(E, F, q);

	// Divide the quad into subquads, but meet at H
	double s = (rand() / (double)RAND_MAX)*0.8 + 0.6; // [0.6, 1.4]
	double t = (rand() / (double)RAND_MAX)*0.8 + 0.6; // [0.6, 1.4]

	recursiveSubdivideRect(points, A, G/*lerp(G, B, s)*/, H, E/*lerp(E, D, t)*/, minLength);
	points->push_back(H);
	recursiveSubdivideRect(points, H, F/*lerp(F, C, s)*/, C, I/*lerp(I, D, t)*/, minLength);
}