#include "Platform.h"
#include "Game.h"

using glm::vec3;
using glm::mat4;

GLuint stdTileVertIndices[trisPerTile*idxsPerTri] = {
	4, 1, 0,
	4, 2, 1,
	4, 5, 2,
	4, 8, 5,
	4, 7, 8,
	4, 6, 7,
	4, 3, 6,
	4, 0, 3
};

CroneGame::CroneGame(){
	windowWidth = 1024;
	windowHeight = 768;

	//default background color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	//wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int z = 0; z < chunkDimension; ++z){
		for (int x = 0; x < chunkDimension; ++x){
			chunk[z][x] = 0;
		}
	}

	GLfloat* ptr = chunkVerts;
	for (int z = 0; z < chunkDimension; ++z){
		for (int x = 0; x < chunkDimension; ++x){
			for (int row = 0; row < 3; ++row){
				for (int col = 0; col < 3; ++col){
					*ptr++ = (GLfloat)(tileDimension * x + col);
					*ptr++ = (GLfloat)(chunk[z][x]);
					*ptr++ = -(GLfloat)(tileDimension * z + row);
				}
			}
		}
	}

	for (int tileNum = 0; tileNum < chunkDimension*chunkDimension; ++tileNum){
		for (int tileVertIdx = 0; tileVertIdx < trisPerTile*idxsPerTri; ++tileVertIdx){
			chunkVertIndices[trisPerTile * idxsPerTri * tileNum + tileVertIdx] = 
				vertsPerTile * tileNum + stdTileVertIndices[tileVertIdx];
		}
	}

	terrainFloor = Shader("terrainFloor.vert", NULL, "terrainFloor.frag");

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(chunkVerts), chunkVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chunkVertIndices), chunkVertIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	cam = Camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
}

CroneGame::~CroneGame(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void CroneGame::update(userInput* inputs, GLfloat dT){
	cam.move(inputs, dT);
}

void CroneGame::render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	terrainFloor.useProgram();

	mat4 model; //identity
	//mat4 view = glm::lookAt(vec3(0.0f, 20.0f, 20.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection = glm::perspective(cam.fov, windowWidth/(GLfloat)windowHeight, 0.1f, 100.0f);

	GLuint modelLoc = glGetUniformLocation(terrainFloor.programHandle, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	GLuint viewLoc = glGetUniformLocation(terrainFloor.programHandle, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.getView()));

	GLuint projectionLoc = glGetUniformLocation(terrainFloor.programHandle, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, vertsPerTile*floatsPerVert*chunkDimension*chunkDimension, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}