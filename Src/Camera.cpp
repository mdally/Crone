#include "Camera.h"
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/trigonometric.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera(glm::vec3 At, glm::vec3 Look, glm::vec3 WorldUp){
	atPos = At;
	look = glm::normalize(Look);
	worldUp = WorldUp;

	fov = (GLfloat)M_PI_4;	//45 degrees
	moveSpeed = 5.0f;
	rotateSpeed = (GLfloat)M_PI_2;	//90 degrees / second
	minCamDist = 5.0f;
	zoomOut = 0;

	recalcVectors();
}

void Camera::move(userInput* inputs, GLfloat dt){
	GLfloat dist = moveSpeed*dt;

	//WASD movement
	glm::vec3 moveDirection = { 0.0f, 0.0f, 0.0f };
	glm::vec3 tmp;

	if (inputs->keyboard[GLFW_KEY_W]){
		//move forward
		tmp = { look.x, 0, look.z };
		moveDirection += glm::normalize(tmp);
	}
	if (inputs->keyboard[GLFW_KEY_S]){
		//move backward
		tmp = { look.x, 0, look.z };
		moveDirection -= glm::normalize(tmp);
	}
	if (inputs->keyboard[GLFW_KEY_A]){
		//move left
		tmp = { right.x, 0, right.z };
		moveDirection -= glm::normalize(tmp);
	}
	if (inputs->keyboard[GLFW_KEY_D]){
		//move right
		tmp = { right.x, 0, right.z };
		moveDirection += glm::normalize(tmp);
	}

	if (moveDirection.x != 0 || moveDirection.y != 0 || moveDirection.z != 0)
		moveDirection = glm::normalize(moveDirection);
	atPos += dist * moveDirection;

	//Scroll in & out
	zoomOut -= inputs->scrollDY;
	inputs->scrollDY = 0;
	if (zoomOut < 0) zoomOut = 0;

	//Q&E Rotate
	if (inputs->keyboard[GLFW_KEY_Q]){
		rotateAroundAt(-rotateSpeed*dt);
	}
	if (inputs->keyboard[GLFW_KEY_E]){
		rotateAroundAt(rotateSpeed*dt);
	}
}

void Camera::rotateAroundAt(GLfloat angle){
	glm::mat4 rot = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 newLook4 = -(rot*glm::vec4(-look, 0.0f));
	look = { newLook4.x, newLook4.y, newLook4.z };
	look = glm::normalize(look);

	recalcVectors();
}

void Camera::changeFOV(GLfloat amount){
	fov -= (GLfloat)amount;

	if (fov < 1.0f){
		fov = 1.0f;
	}
	else if (fov > 45.0f){
		fov = 45.0f;
	}
}

void Camera::recalcVectors(){
	right = glm::normalize(glm::cross(look, worldUp));
	up = glm::normalize(glm::cross(right, look));
}

glm::mat4 Camera::getView(){
	glm::vec3 scrolledPos = atPos - look*(minCamDist + zoomOut);
	return glm::lookAt(scrolledPos, atPos, up);
}