#include "Platform.h"
#include "Shader.h"

#include <fstream>
using std::ifstream;
using std::string;

Shader::Shader(char* vertFileName, char* geometryFileName, char* fragFileName){
	GLuint vertShader, geomShader, fragShader;

	//compile the shaders
	try{
		vertShader = compileShaderFile(GL_VERTEX_SHADER, vertFileName);
		if (geometryFileName)
			geomShader = compileShaderFile(GL_GEOMETRY_SHADER, geometryFileName);
		fragShader = compileShaderFile(GL_FRAGMENT_SHADER, fragFileName);
	}
	catch (ErrorCondition e){
		throw e;
	}

	//link the shaders
	programHandle = glCreateProgram();
	glAttachShader(programHandle, vertShader);
	if (geometryFileName)
		glAttachShader(programHandle, geomShader);
	glAttachShader(programHandle, fragShader);
	glLinkProgram(programHandle);

	//check all ok
	GLint success;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
	if (!success){
		GLchar infoLog[512];
		glGetProgramInfoLog(programHandle, 512, NULL, infoLog);

		debugLog(string("Error linking shaders:\n") + infoLog);
		throw SHADER_LINK_ERROR;
	}

	//clean up
	glDeleteShader(vertShader);
	if (geometryFileName)
		glDeleteShader(geomShader);
	glDeleteShader(fragShader);
}

void Shader::useProgram(){
	glUseProgram(programHandle);
}

GLuint Shader::compileShaderFile(GLuint shaderType, string fileName){
	GLuint shaderHandle;

	ifstream shaderFile(CRONE_DIR_SHADERS"/"+fileName);
	if (shaderFile.is_open()){
		//read in the source file
		string currentLine;
		string allLines = "";

		while (getline(shaderFile, currentLine)){
			if(currentLine.find_first_not_of(" \n\t\r\f") != string::npos){
				allLines += currentLine + "\n";
			}
		}
		shaderFile.close();
		const char* shaderSource = allLines.c_str();

		//Compile the shader
		shaderHandle = glCreateShader(shaderType);
		glShaderSource(shaderHandle, 1, &shaderSource, NULL);
		glCompileShader(shaderHandle);

		//check compile was successful
		GLint success;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
		if (!success){
			GLchar infoLog[512];
			glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
			
			debugLog("Error compiling shader " + fileName + ":\n" + infoLog);
			throw SHADER_COMPILE_ERROR;
		}
	}
	else{
		debugLog("Could not read shader file " + fileName);
		throw SHADER_FILE_READ_ERROR;
	}

	return shaderHandle;
}