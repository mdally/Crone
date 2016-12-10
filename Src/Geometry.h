#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Voronoi\Point2.h"
#include "Voronoi\Vector2.h"

namespace GEOM {
	inline bool LineSegmentsIntersect(GEOM::Point2& p0, GEOM::Point2& p1, GEOM::Point2& p2, GEOM::Point2& p3) {
		GEOM::Vector2 s0 = p1 - p0;
		GEOM::Vector2 s1 = p3 - p2;

		double s, t;
		s = (-s0.y * (p0.x - p2.x) + s0.x * (p0.y - p2.y)) / (-s1.x * s0.y + s0.x * s1.y);
		t = (s1.x * (p0.y - p2.y) - s1.y * (p0.x - p2.x)) / (-s1.x * s0.y + s0.x * s1.y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
			return true;
		}

		return false;
	}

	inline bool FindLineSegmentIntersection(GEOM::Point2& p0, GEOM::Point2& p1, GEOM::Point2& p2, GEOM::Point2& p3, GEOM::Point2& intersect) {
		GEOM::Vector2 s0 = p1 - p0;
		GEOM::Vector2 s1 = p3 - p2;

		double s, t;
		s = (-s0.y * (p0.x - p2.x) + s0.x * (p0.y - p2.y)) / (-s1.x * s0.y + s0.x * s1.y);
		t = (s1.x * (p0.y - p2.y) - s1.y * (p0.x - p2.x)) / (-s1.x * s0.y + s0.x * s1.y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
			intersect.x = p0.x + (t * s0.x);
			intersect.y = p0.y + (t * s0.y);
			return true;
		}

		return false;
	}

	inline double isLeft(GEOM::Point2& p0, GEOM::Point2& p1, GEOM::Point2& p2) {
		return ((p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y));
	}

	inline bool PointInPolygon(GEOM::Point2& p, std::vector<GEOM::Point2> polygon) {
		int winding = 0;

		size_t nVerts = polygon.size();
		Point2* curr;
		Point2* next;

		size_t currIdx;
		size_t nextIdx;

		for (size_t i = 0; i <= nVerts; ++i) {
			currIdx = i % nVerts;
			nextIdx = (i + 1) % nVerts;

			curr = &polygon[currIdx];
			next = &polygon[nextIdx];

			if (curr->y <= p.y) {
				if (next->y > p.y) {
					if (isLeft(*curr, *next, p) > 0)
						++winding;
				}
			}
			else {
				if (next->y <= p.y) {
					if (isLeft(*curr, *next, p) < 0) {
						--winding;
					}
				}
			}
		}

		return winding != 0;
	}
}

#endif