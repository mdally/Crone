#include "WorldMap.h"
#include "Voronoi\VoronoiDiagramGenerator.h"
#include <ctime>
#include <utility>
#include <algorithm>
#include "DLList.h"
#include "EarClipping.h"
#include "Geometry.h"
#include <limits>

using namespace GEOM;
using namespace VORONOI;

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void genRandomSites(std::vector<Point2>& sites, BoundingBox& bbox, uint32_t dimension, uint32_t numSites, uint32_t seed) {
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

void WorldMap::generate(uint32_t seed) {
	std::vector<Point2> sites;
	BoundingBox bbox;

	double sitePerUnitArea = 500 / (double)(2000 * 2000);

	dimension = 3000;
	siteCount = (uint32_t)(dimension * dimension * sitePerUnitArea);
	genRandomSites(sites, bbox, dimension, siteCount, seed);

	VoronoiDiagramGenerator vdg;
	diagram = vdg.compute(sites, bbox);
	for (uint32_t i = 0; i < 5; ++i) {
		Diagram* old = diagram;
		diagram = vdg.relax();
		delete old;
	}

	//divide land/water
	pickLandWater();

	genNoisyEdges();

	//perform ear clipping to generate non-intersecting triangles that completely cover each cell
	std::vector<GLfloat> verts;
	std::vector<GLuint> idxs;
	openGL_TriData triData = performEarClipping(diagram, dimension);

	terrainVerts = triData.verts;
	nTerrainVerts = triData.nVerts;

	terrainVertIndices = triData.idxs;
	nTerrainTris = triData.nTris;

	delete diagram;
	diagram = nullptr;
}

void WorldMap::pickLandWater() {
	float mult = 5.0f / (float)dimension;
	float distMod;
	float noise;
	float out;

	Point2 center = Point2(dimension/2.0, dimension/2.0);

	std::vector<Point2> centers;
	centers.push_back(Point2(dimension*(1 / 4.0), dimension*(1 / 4.0)));
	centers.push_back(Point2(dimension*(1 / 4.0), dimension*(3 / 4.0)));
	centers.push_back(Point2(dimension*(3 / 4.0), dimension*(1 / 4.0)));
	centers.push_back(Point2(dimension*(3 / 4.0), dimension*(3 / 4.0)));


	for (Cell* c : diagram->cells) {
		uint32_t nPoints = 0;
		noise = 0.0f;

		for (HalfEdge* he : c->halfEdges) {
			++nPoints;

			noise += (1.0f+simplex.fractal(5, (float)(he->startPoint()->x*mult), (float)(he->startPoint()->y*mult)))/2.0f;
		}
		noise /= nPoints;

		distMod = std::numeric_limits<float>::max();
		for (Point2& p : centers) {
			float dist = (float)p.distanceTo(c->site.p);

			if (dist < distMod)
				distMod = dist;
		}

		distMod = 1.0f - (distMod / (dimension/4.0f));
		if (distMod < 0.0f) distMod = 0.0f;

		out = 0.0f + distMod*0.4f + 0.6f*noise;

		if (out > 0.5) {
			c->biome = LAND;
		}
		c->height = out;
	}
}

void WorldMap::genNoisyEdges() {
	if (!diagram) return;
	srand(0);

	for (Edge* e : diagram->edges) {
		if (e->lSite && e->rSite && (e->lSite->cell->biome != e->rSite->cell->biome)) {
			if (e->lSite->cell == diagram->cells[4] || e->rSite->cell == diagram->cells[4])
				int i = 0;

			std::vector<Point2> path1;
			std::vector<Point2> path2;

			Point2& v0 = *e->vertA;
			Point2& v1 = *e->vertB;
			Point2 d0 = e->lSite->p;
			Point2 d1 = e->rSite->p;
			Point2 mid = lerp(v0, v1, 0.5);

			//project the delaunay verts closer to the voronoi edge midpoint if the delaunay edge doesn't intersect the voronoi edge
			if (!LineSegmentsIntersect(v0, v1, d0, d1)) {
				Point2 targetVert = (d0.distanceToSquared(v0) < d0.distanceToSquared(v1) ? v0 : v1);
				Vector2 delaunayEdge = d1 - d0;

				FindLineSegmentIntersection(mid, d0, targetVert, targetVert - 0.5*delaunayEdge, d0);
				FindLineSegmentIntersection(mid, d1, targetVert, targetVert + 0.5*delaunayEdge, d1);
			}
			//project the delaunay verts closer to voronoi edge midpoint if the distance is greater than twice the length of the voronoi edge
			#define voronoiDelaunayMultiplier 1
			if ((v1 - v0).lengthSquared()*voronoiDelaunayMultiplier < (d0 - mid).lengthSquared()) {
				double voronoiEdgeLength = (v1 - v0).length();

				Vector2 tmp = d0 - mid;
				tmp.normalize();
				d0 = mid + voronoiDelaunayMultiplier*voronoiEdgeLength*tmp;

				tmp = d1 - mid;
				tmp.normalize();
				d1 = mid + voronoiDelaunayMultiplier*voronoiEdgeLength*tmp;
			}

			double tradeoff = 0.5;
			Point2 t = lerp(v0, d0, tradeoff);
			Point2 q = lerp(v0, d1, tradeoff);
			Point2 r = lerp(v1, d0, tradeoff);
			Point2 s = lerp(v1, d1, tradeoff);

			//TODO : change this to vary based on type of border
			// i.e. river, coastline, ocean-ocean, etc
			double minLength = 1;

			if (v0.distanceTo(v1) < minLength) {
				continue;
			}

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
			Point2* vertArray = noisyVerts.allocContiguousArray(nVerts, Point2());
			size_t idx = 0;
			for (Point2& p : path1) {
				vertArray[idx++] = p;
			}
			e->nNoisyVerts = (uint32_t)nVerts;
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