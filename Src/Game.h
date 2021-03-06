#ifndef _GAME_H_
#define _GAME_H_

#include "Shader.h"
#include "Camera.h"
#include "WorldMap.h"

#define chunkDimension 32	//tiles per chunk side
#define tileDimension 2		//meters per tile side
#define vertsPerTile 9
#define floatsPerVert 3
#define trisPerTile 8
#define idxsPerTri 3

class CroneGame{
public:
	CroneGame();
	~CroneGame();

	void update(userInput* inputs, GLfloat dT);
	void render();
private:
	WorldMap map;
	
	GLuint VAO, VBO, EBO;

	Shader terrainFloor;

	Camera cam;

	GLuint windowWidth, windowHeight;
};

#endif