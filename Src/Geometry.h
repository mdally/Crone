#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Voronoi\Point2.h"
#include "Voronoi\Vector2.h"

inline bool LineSegmentsIntersect(GEOM::Point2& p0, GEOM::Point2& p1, GEOM::Point2& p2, GEOM::Point2& p3) {
	GEOM::Vector2 s0 = p1 - p0;
	GEOM::Vector2 s1 = p3 - p2;

	double s, t;
	s = (-s0.y * (p0.x - p2.x) + s0.x * (p0.y - p2.y)) / (-s1.x * s0.y + s0.x * s1.y);
	t = (s1.x * (p0.y - p2.y) - s1.y * (p0.x - p2.x)) / (-s1.x * s0.y + s0.x * s1.y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1){
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

#endif