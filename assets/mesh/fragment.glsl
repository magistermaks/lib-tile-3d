
#version 330

out vec4 color;
in vec3 col;

void main() {
	color = vec4(col, 1.0f);
}

