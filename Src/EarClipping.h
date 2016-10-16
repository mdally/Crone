#include "DLList.h"
#include "Voronoi\Point2.h"
#include "Voronoi\Diagram.h"
#include <utility>
#include <vector>
#include <GLEW\glew.h>

struct openGL_TriData {
	GLfloat* verts;
	uint32_t nVerts;

	GLuint* idxs;
	uint32_t nTris;

	openGL_TriData() : verts(nullptr), nVerts(0), idxs(nullptr), nTris(0) {};
};

openGL_TriData performEarClipping(VORONOI::Diagram* diagram, int dimension);