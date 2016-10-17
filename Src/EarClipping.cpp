#include "EarClipping.h"
#include <GLEW/glew.h>
#include <vector>
#include <list>

using namespace GEOM;
using namespace DLLIST;
using namespace VORONOI;

typedef std::pair<Point2,Point2> lineSegment;

struct PolygonVert {
	Point2 vert;
	int32_t idx;
	double angle;
	bool isValidEar;

	PolygonVert(Point2 p) : vert(p), idx(-1), angle(0.0), isValidEar(false) {};

	inline bool isConvex() { return angle < M_PI; };
	inline bool isReflex() { return angle >= M_PI; };
};

void calcVertAngle(DLListNode<PolygonVert>* vertNode);
bool pointInTri(Point2& A, Point2& B, Point2& C, Point2& p);
bool checkValidEar(DLListNode<PolygonVert>* pVertNode, DLList<PolygonVert>& allVerts);
bool lineSegmentsIntersect(Point2& a, Point2& b, Point2& c, Point2& d);
double area2(Point2& a, Point2& b, Point2& c);
inline bool collinear(Point2& a, Point2& b, Point2& c);
inline bool left(Point2& a, Point2& b, Point2& c);
inline bool XOR(bool a, bool b);

openGL_TriData performEarClipping(Diagram* diagram, int dimension) {
	DLList<PolygonVert> allVerts;
	std::list<DLListNode<PolygonVert>*> earVerts;
	uint32_t idxCounter = 0;

	openGL_TriData triData;
	for (Cell* c : diagram->cells) {
		uint32_t cellVertCount = 0;
		for (HalfEdge* he : c->halfEdges) {
			if (he->edge->nNoisyVerts) {
				cellVertCount += he->edge->nNoisyVerts - 1;
			}
			else {
				cellVertCount += 1;
			}
		}

		triData.nVerts += cellVertCount;
		triData.nTris += (cellVertCount - 2);
	}

	triData.verts = (GLfloat*)malloc(sizeof(GLfloat)*triData.nVerts * 6);
	triData.idxs = (GLuint*)malloc(sizeof(GLuint)*triData.nTris * 3);

	GLfloat* currentVert = triData.verts;
	GLuint* currentIdx = triData.idxs;

	for (Cell* c : diagram->cells) {
		float r, g, b;
#define DO_OCEANS 0
#if DO_OCEANS
		bool ocean = (rand() / (double)RAND_MAX)>0.5;
		for (HalfEdge* he : c->halfEdges) {
			if (he->edge->lSite == nullptr || he->edge->rSite == nullptr) {
				ocean = true;
				break;
			}
		}

		switch (ocean) {
			case true: {
				r = 0.0f;
				g = 0.0f;
				b = 1.0f;
			} break;
			case false: {
				r = 0.0f;
				g = 1.0f;
				b = 0.0f;
			} break;
		}
#else
		r = rand() / (float)RAND_MAX;
		g = rand() / (float)RAND_MAX;
		b = rand() / (float)RAND_MAX;
#endif

		//add all the cell's noisy verts in order to a doubly linked list
		for (HalfEdge* he : c->halfEdges) {
			Point2* noisyVerts = he->edge->noisyVerts;
			if (noisyVerts) {
				bool reverse = *(he->startPoint()) != noisyVerts[0];

				uint32_t nNoisyVerts = he->edge->nNoisyVerts - 1;
				for (uint32_t i = 0; i < nNoisyVerts; ++i) {
					uint32_t idx = (reverse ? nNoisyVerts - i : i);
					Point2 site = noisyVerts[idx];

					allVerts.addToTail(PolygonVert(site));
				}
			}
			else {
				allVerts.addToTail(PolygonVert(*he->startPoint()));
			}
		}

		//determine vert angles
		DLListNode<PolygonVert>* vertNode = allVerts.head;
		do {
			calcVertAngle(vertNode);

			vertNode = vertNode->next;
		} while (vertNode != allVerts.head);

		//initialize list of acceptable ear verts for clipping
		do {
			if (vertNode->data.isConvex() && checkValidEar(vertNode, allVerts)) {
				earVerts.push_back(vertNode);
				vertNode->data.isValidEar = true;
			}

			vertNode = vertNode->next;
		} while (vertNode != allVerts.head);

		//start clipping
		while (!earVerts.empty()) {
			DLListNode<PolygonVert>* earVert = earVerts.front();
			earVerts.pop_front();

			DLListNode<PolygonVert>* prev = earVert->prev;
			DLListNode<PolygonVert>* next = earVert->next;
			if (prev == next) {
				continue;
			}

			//output the ear that is being clipped
			if (prev->data.idx == -1) {
				prev->data.idx = idxCounter++;

				*(currentVert++) = (float)prev->data.vert.x;
				*(currentVert++) = (float)0.0;
				*(currentVert++) = -((float)dimension - (float)prev->data.vert.y);
				*(currentVert++) = r;
				*(currentVert++) = g;
				*(currentVert++) = b;
			}
			if (earVert->data.idx == -1) {
				earVert->data.idx = idxCounter++;

				*(currentVert++) = (float)earVert->data.vert.x;
				*(currentVert++) = (float)0.0;
				*(currentVert++) = -((float)dimension - (float)earVert->data.vert.y);
				*(currentVert++) = r;
				*(currentVert++) = g;
				*(currentVert++) = b;
			}
			if (next->data.idx == -1) {
				next->data.idx = idxCounter++;

				*(currentVert++) = (float)next->data.vert.x;
				*(currentVert++) = (float)0.0;
				*(currentVert++) = -((float)dimension - (float)next->data.vert.y);
				*(currentVert++) = r;
				*(currentVert++) = g;
				*(currentVert++) = b;
			}

			*(currentIdx++) = prev->data.idx;
			*(currentIdx++) = earVert->data.idx;
			*(currentIdx++) = next->data.idx;

			allVerts.removeNode(earVert);

			//re-evaluate the state of prev and next verts
			//check previously reflex verts. They might now be convex, and if so might now be valid ears
			//check ones that were ears to make sure they still are valid ears

			if (prev->data.isReflex()) {
				calcVertAngle(prev);
				if (prev->data.isConvex()) {
					if (checkValidEar(prev, allVerts)) {
						prev->data.isValidEar = true;
						earVerts.push_back(prev);
					}
				}
			}
			else if (prev->data.isValidEar) {
				if (!checkValidEar(prev, allVerts)) {
					prev->data.isValidEar = false;
					earVerts.remove(prev);
				}
			}

			if (next->data.isReflex()) {
				calcVertAngle(next);
				if (next->data.isConvex()) {
					if (checkValidEar(next, allVerts)) {
						next->data.isValidEar = true;
						earVerts.push_back(next);
					}
				}
			}
			else if (next->data.isValidEar) {
				if (!checkValidEar(next, allVerts)) {
					next->data.isValidEar = false;
					earVerts.remove(next);
				}
			}
		}

		while (allVerts.tail != nullptr) {
			allVerts.removeNode(allVerts.tail);
		}
		earVerts.clear(); 
	}

	return triData;
}

void calcVertAngle(DLListNode<PolygonVert>* vertNode) {
	Point2 p0 = vertNode->prev->data.vert;
	Point2 p1 = vertNode->data.vert;
	Point2 p2 = vertNode->next->data.vert;

	Vector2 v0 = p0 - p1;
	Vector2 v1 = p2 - p1;

	vertNode->data.angle = Vector2::signedAngle(v0, v1);
}

bool pointInTri(Point2& P, Point2& A, Point2& B, Point2& C) {
	// Compute vectors        
	Vector2 v0 = C - A;
	Vector2 v1 = B - A;
	Vector2 v2 = P - A;

	// Compute dot products
	double dot00 = v0*v0;
	double dot01 = v0*v1;
	double dot02 = v0*v2;
	double dot11 = v1*v1;
	double dot12 = v1*v2;

	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool checkValidEar(DLListNode<PolygonVert>* pVertNode, DLList<PolygonVert>& allVerts) {
	Point2 p0 = pVertNode->prev->data.vert;
	Point2 p1 = pVertNode->data.vert;
	Point2 p2 = pVertNode->next->data.vert;

	//check if any reflex verts are inside this ear's triangle
	bool noOtherPointsInside = true;
	DLListNode<PolygonVert>* pOtherVert = allVerts.head;
	do {
		if (pOtherVert != pVertNode->prev && pOtherVert != pVertNode && pOtherVert != pVertNode->next) {
			if (pOtherVert->data.isReflex() && pointInTri(pOtherVert->data.vert, p0, p1, p2)) {
				noOtherPointsInside = false;
				break;
			}
		}

		pOtherVert = pOtherVert->next;
	} while (pOtherVert != allVerts.head);

	return noOtherPointsInside;
}

bool lineSegmentsIntersect(Point2& a, Point2& b, Point2& c, Point2& d) {
	/* Eliminate improper cases. */
	if (collinear(a, b, c) || collinear(a, b, d) || collinear(c, d, a) || collinear(c, d, b))
		return false;

	return XOR(left(a, b, c), left(a, b, d)) && XOR(left(c, d, a), left(c, d, b));
}

double area2(Point2& a, Point2& b, Point2& c) {
	return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

bool collinear(Point2& a, Point2& b, Point2& c) {
	return area2(a, b, c) == 0;
}

bool left(Point2& a, Point2& b, Point2& c) {
	return area2(a, b, c) > 0;
}

bool XOR(bool a, bool b) {
	return !a ^ !b;
}