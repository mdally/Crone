#include "Platform.h"
#include "Game.h"

//General includes
#include <Windows.h>
#include <stdio.h>
#include <ctime>
#include <fstream>
using std::ofstream;
#include <iostream>
using std::cout;
using std::endl;
#include <string>

//GLFW - for window, context, input
#include <GLFW/glfw3.h>

//SOIL - Simple OpenGL Image Library
#include <SOIL/SOIL.h>

//User input callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Take a screenshot, save it as a .bmp to the Screenshots folder
void takeScreenshot();

//Set up debug output, open a window, handle keyboard and mouse input, etc.
void setupResources();

//Clean up all the resources set up in setupResources() before finishing the program
void cleanUpAndClose();

//Globals
GLFWwindow* window;
ofstream debugFile;

GLuint windowWidth = 1024;
GLuint windowHeight = 768;

userInput inputs;

CroneGame* game;

int main(){
	try{
		setupResources();
	}
	catch (ErrorCondition e){
		return e;
	}

	game = new CroneGame();

	//disable vsync (allow over 60fps)
	//glfwSwapInterval(0);

	//game loop
	GLfloat lastFrame = (GLfloat)glfwGetTime();
	while (!glfwWindowShouldClose(window)){
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		GLfloat deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//print dT in ms to console
		//cout << deltaTime*1000 << endl;

		//check and call input events
		glfwPollEvents();

		game->update(&inputs, deltaTime);
		game->render();

		//swap buffers
		glfwSwapBuffers(window);
	}

	cleanUpAndClose();

	//wait before closing console window
	//char tmp = getchar();
	return NO_ERROR;
}

//Output a timestamped message to the debug log file
void debugLog(std::string msg){
	time_t currentTime = time(0);
	struct tm* now = localtime(&currentTime);

	char timeText[20];
	sprintf(timeText, "%02d:%02d:%02d - ", now->tm_hour, now->tm_min, now->tm_sec);

	debugFile << timeText << msg << endl;
}

//Handle key press and release
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (action == GLFW_PRESS){
		inputs.keyboard[key] = true;
	}
	else if (action == GLFW_RELEASE){
		inputs.keyboard[key] = false;
	}

	//exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//screenshot
	if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS){
		takeScreenshot();
	}
}

//Handle mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	static bool firstMouseLoc = true;
	if (firstMouseLoc){
		inputs.mouseSensitivity = 0.05f;
		inputs.mouseX = (GLfloat)xpos;
		inputs.mouseY = (GLfloat)ypos;

		inputs.mouseDX = 0;
		inputs.mouseDY = 0;
		firstMouseLoc = false;
	}
	else{
		inputs.mouseDX = (GLfloat)(xpos - inputs.mouseX)*inputs.mouseSensitivity;
		inputs.mouseDY = (GLfloat)(ypos - inputs.mouseY)*inputs.mouseSensitivity;

		inputs.mouseX = (GLfloat)xpos;
		inputs.mouseY = (GLfloat)ypos;
	}
}

//Handle mouse scroll
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	inputs.scrollSensitivity = 1.0f;
	inputs.scrollDX = (GLfloat)xoffset*inputs.scrollSensitivity;
	inputs.scrollDY = (GLfloat)yoffset*inputs.scrollSensitivity;
}

//Take a screenshot, save it as a .bmp to the Screenshots folder
void takeScreenshot(){
	static time_t lastScreenshot = 0;
	time_t currentTime = time(0);
	struct tm* now = localtime(&currentTime);

	//only allow 1 screenshot per second
	if (currentTime - lastScreenshot > 1){
		char screenshotFileName[50];
		sprintf(
			screenshotFileName, 
			"%s/%d-%02d-%02d-%02d-%02d-%02d.bmp",
			CRONE_DIR_SCREENSHOTS,
			now->tm_year + 1900,
			now->tm_mon,
			now->tm_mday,
			now->tm_hour,
			now->tm_min,
			now->tm_sec
		);

		SOIL_save_screenshot(
			screenshotFileName,
			SOIL_SAVE_TYPE_BMP,
			0,
			0,
			windowWidth,
			windowHeight
		);

		lastScreenshot = currentTime;
	}
}

//Set up debug output, open a window, handle keyboard and mouse input, etc.
void setupResources(){
	//set up debug output
	time_t currentTime = time(0);
	struct tm* now = localtime(&currentTime);
	char debugFileName[100];
	sprintf(
		debugFileName, 
		"%s/%d-%02d-%02d-%02d-%02d-%02d.txt",
		CRONE_DIR_LOGS,
		now->tm_year + 1900,
		now->tm_mon,
		now->tm_mday,
		now->tm_hour,
		now->tm_min,
		now->tm_sec
	);
	debugFile.open(debugFileName, ofstream::out|ofstream::trunc);
	if (!debugFile.is_open()) throw DEBUG_FILE_WRITE_ERROR;
	debugLog("starting up");

	//set up GLFW and open a window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Crone", nullptr, nullptr);
	if (!window){
		debugLog("Failed to create GLFW window");
		glfwTerminate();
		throw GLFW_WINDOW_CREATE_ERROR;
	}
	glfwMakeContextCurrent(window);
	//define the viewport dimensions
	glViewport(0, 0, windowWidth, windowHeight); //lower left x, lower left y, width, height

	//set up mouse & keyboard
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	//set up GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK){
		debugLog("Failed to initialize GLEW");
		throw GLEW_INIT_ERROR;
	}

	//init the keyboard
	for (int i = 0; i < 1024; ++i){
		inputs.keyboard[i] = false;
	}
}

//Clean up all the resources set up in setupResources() before finishing the program
void cleanUpAndClose(){
	if (!glfwWindowShouldClose(window)) glfwSetWindowShouldClose(window, GL_TRUE);
	glfwTerminate();
	debugLog("shutting down");
	debugFile.close();
}