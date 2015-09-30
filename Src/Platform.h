#ifndef _CRONE_WIN32_H_
#define _CRONE_WIN32_H_

//GLM - OpenGL Mathematics
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

//GLEW - OpenGL Extension Wrangler
#define GLEW_STATIC
#include <GL/glew.h>

#include<string>

//Game directories
#define CRONE_DIR_LOGS "Logs"
#define CRONE_DIR_SCREENSHOTS "Screenshots"
#define CRONE_DIR_SHADERS "Shaders"
#define CRONE_DIR_TEXTURES "Assets/Textures"
#define HOTLOAD_LOC "Hotload/Hotload"

enum ErrorCondition {
	NO_ERROR, 
	DEBUG_FILE_WRITE_ERROR, 
	GLFW_WINDOW_CREATE_ERROR, 
	GLEW_INIT_ERROR, 
	SHADER_FILE_READ_ERROR, 
	SHADER_COMPILE_ERROR, 
	SHADER_LINK_ERROR 
};

typedef struct {
	bool keyboard[1024];

	GLfloat mouseX, mouseY;
	GLfloat mouseDX, mouseDY;
	GLfloat mouseSensitivity;

	GLfloat scrollDX, scrollDY;
	GLfloat scrollSensitivity;
} userInput;

//Output a timestamped message to the debug log file
void debugLog(std::string msg);

#endif