#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>

class Shader{
public:
	Shader(){};
	Shader(char* vertexFileName, char* geometryFileName, char* fragmentFileName);

	void useProgram();

	GLuint programHandle;
private:
	GLuint compileShaderFile(GLuint shaderType, std::string fileName);
};

#endif