#version 430
// very simple and probably only used for debug systems
layout (location = 0) in vec3 aPos;
uniform mat4 MVP; 

void main()
{
	gl_Position = MVP*vec4(aPos, 1);
}