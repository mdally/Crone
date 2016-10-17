#include "Platform.h"
#include "Game.h"

using glm::vec3;
using glm::mat4;

CroneGame::CroneGame(){
	windowWidth = 1024;
	windowHeight = 768;

	//default background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

#define WIREFRAME 0
#if WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	map.generate(5, true);

	terrainFloor = Shader("terrainFloor.vert", nullptr, "terrainFloor.frag");

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, map.nTerrainVerts*sizeof(GLfloat)*6, map.terrainVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, map.nTerrainTris*sizeof(GLuint)*3, map.terrainVertIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

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
	mat4 projection = glm::perspective(cam.fov, windowWidth/(GLfloat)windowHeight, 0.1f, 100000.0f);

	GLuint modelLoc = glGetUniformLocation(terrainFloor.programHandle, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	GLuint viewLoc = glGetUniformLocation(terrainFloor.programHandle, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.getView()));

	GLuint projectionLoc = glGetUniformLocation(terrainFloor.programHandle, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, map.nTerrainTris*3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}