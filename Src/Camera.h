#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Platform.h"


class Camera{
	public:
		GLfloat fov;
		GLfloat minCamDist;
		GLfloat zoomOut;

		GLfloat moveSpeed;		// meters/second
		GLfloat rotateSpeed;	// radians/second

		glm::vec3 atPos;
		glm::vec3 look;
		glm::vec3 right;

		glm::vec3 up;
		glm::vec3 worldUp;

		Camera(){};

		Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 worldUp);

		void move(userInput* inputs, GLfloat dt);

		void rotateAroundAt(GLfloat angle);

		void changeFOV(GLfloat amount);

		glm::mat4 getView();
	
	private:
		void recalcVectors();
};

#endif