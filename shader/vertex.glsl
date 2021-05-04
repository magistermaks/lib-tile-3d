
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

out vec4 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float size;

void main() {
	gl_Position = projection * view * model * vec4(position / size, 1.0);
	col = color;
}

