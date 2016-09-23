#version 330 core

layout (location = 0) in vec3 position; //the position variable has attribute position 0
layout (location = 1) in vec3 color;	//the color variable has attribute position 1
//layout (location = 2) in vec2 texCoord;

out vec3 vertColor; //specify a color output to the fragment shader
//out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertColor = color; //set vertColor to the input color we got from the vertex data
    //TexCoord = texCoord;
}