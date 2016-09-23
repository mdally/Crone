#version 330 core

in vec3 vertColor;
//in vec2 TexCoord;

out vec4 color;

//uniform sampler2D texture1;
//uniform sampler2D texture2;

void main(){
	color = vec4(vertColor, 1.0f);
}